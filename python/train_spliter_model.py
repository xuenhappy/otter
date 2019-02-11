#!/usr/bin/env python  
# -*- coding: utf-8 -*- 
import torch
import torch.nn.functional as F
from torch import optim
from torch.autograd import Variable
import h5py
import numpy as np
import os
import time
import re



def chr_type(ch):
    if ch==u" ":
        return 0
    if u'A'<=ch<=u'Z':
        return 1
    if u'a'<=ch<=u"z":
        return 1
    if u'0'<=ch<=u'9':
        return 2
    return -1


def word_len(strs):
    num=0
    bt=-1
    for ch in strs:
        x=chr_type(ch) 
        if bt!=x and bt>0:
            num+=1
        if x<=0:
            num+=1
            bt=-1
            continue
        bt=x;
    return num
        


class SegDataSetIter():
    def __init__(self,dpath,max_size,batch_size):
        self.dpath=dpath
        self.batch_size=batch_size
        self.max_size=max_size
        self.split_reg=re.compile(ur"\s+")
    
    def __iter__(self):
        with open(self.dpath) as fobj:
            data=[]
            for line in fobj:
                line=line.decode("utf-8")
                line=line.strip()
                if not line:
                    continue
                lines=self.split_reg.split(line)
                while len(lines)>self.max_size:
                    data.append(lines[:self.max_size])
                    lines=lines[self.max_size:]
                if len(lines)<2:
                    continue
                data.append(lines)
                if len(data)>self.batch_size:
                    yield data
                    data=[]

            if data:
                yield data



class WordDict():
    def __init__(self,dpath):
        h5file=h5py.File(dpath,'r')
        words=h5file['wds']
        vec=h5file['ary']
        wnum,wszie=vec.shape
        assert wnum==words.shape[0],"bad dict file!"
        self.obj=h5file
        self._wnum=wnum
        self._wsize=wszie
        self.words_idx=dict((v,i) for (i,v) in enumerate(words[()]))
        self._maxlen=max(word_len(w) for w in self.words_idx.keys())
        self.vec=vec 
        if wnum*wszie<2e6*300:
            self.vec=vec[()]#if less data then cache them all for speed!
    
    def embeding(self,uni_word):
        if not uni_word.strip():
            return self.vec[self.words_idx[u"<#S1>"]].astype(np.float32)
        if uni_word in self.words_idx:
            return self.vec[self.words_idx[uni_word]].astype(np.float32)
        x=[self.vec[self.words_idx[w]] for w in uni_word if w in self.words_idx]
        if len(x)>0:
            return np.average(x,axis=0).astype(np.float32)
        return np.zeros(self._wsize,np.float32)
       
    def hasw(self,words):
        return words in self.words_idx
    
    def wv_size(self):
        return self._wsize

    def wordnum(self):
        return self._wnum

    def max_wlen(self):
        return self._maxlen

    def close(self):
        self.obj.close()



class Solver():
    def __init__(self,model,dataset,lr=0.002,epoch_num=2,iter_num=4):
        self.model=model
        self.dataset=dataset
        self.epoch_num=epoch_num
        self.iter_num=iter_num
        self.lr=lr
        self._build()

    def _build(self):
        params=self.model.parameters()
        if torch.cuda.is_available():
            self.model=self.model.cuda()
            #params=[p.cuda() for p in params]
        params=self.model.parameters()
        self.optimizer = optim.SGD(params, lr=self.lr,weight_decay=1e-4)
        timestamp = str(int(time.time()))
        out_dir = os.path.abspath(os.path.join(os.path.curdir,"model_dat", timestamp))
        print("Write model to {}".format(out_dir))
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)
        self.out_dir = out_dir

    def save_torch_model(self,epoch_num):
        #save model for torch
        torch.save(self.model, os.path.join(self.out_dir, "model-%d.pkl"%epoch_num))
        #save model for numpy
        state_dict=dict((k,v.cpu().numpy()) for (k,v) in self.model.state_dict().items())
        np.savez(os.path.join(self.out_dir, "model-%d"%epoch_num),**state_dict)
        
    def adjust_learning_rate(self,epoch):
        """Sets the learning rate to the initial LR decayed by 10 every 30 epochs"""
        lr = self.lr *(0.1 **(epoch/4000.0))+1e-4
        for param_group in self.optimizer.param_groups:
            param_group['lr'] = lr


    def solve(self):
        step = 0
        epoch_num = 0
        sample_num= 0
        while epoch_num < self.epoch_num:
            epoch_num += 1
            for td in self.dataset:
                sample_num+=1
                iter_num = 0
                while iter_num < self.iter_num:
                    iter_num += 1
                    step += 1
                    loss=self.model.loss_fuc(td)
                    self.optimizer.zero_grad()  
                    loss.backward()
                    self.optimizer.step()
                    print('train [epoch|sample|step]:[%d|%d|%d] loss:%g' % (epoch_num,sample_num,step, float(loss)))
                #self.adjust_learning_rate(sample_num)
                if sample_num%10000==0:
                    self.save_torch_model(sample_num)
        print("finish train job...")
            





def insrt_space(dlist):
    """
    恢复英文之间的空白
    """
    new_list=[dlist[0]]
    for i in range(1,len(dlist)):
        x=dlist[i]
        if chr_type(new_list[-1][-1])>0 and chr_type(x[0])>0:
            new_list.append(u" ")
        new_list.append(x)
    return new_list


def basic_cut(dlist):
    new_list=[]
    for strs in dlist:
        if len(strs)==1:
            new_list.append(strs)
            continue
        buf=[]
        bt=-1
        for ch in strs:
            xt=chr_type(ch)
            if buf and xt!=bt:
                new_list.append("".join(buf))
                buf=[]
            if xt<=0:
                new_list.append(ch)
                bt=-1
                continue
            buf.append(ch)
            bt=xt

        if buf:
            new_list.append("".join(buf))
        
    return new_list



def make_graph(dlist,user_dict,distance_func,other_words=set([])):
    basic_grap=[]
    num=1
    for i in range(len(dlist)):
        h=[(num,i+1,user_dict.embeding(dlist[i]))]
        num+=1
        for j in range(i+1,min(len(dlist),i+user_dict.max_wlen())):
            d="".join(dlist[i:j+1])
            if d in other_words or user_dict.hasw(d):
                h.append((num,j+1,user_dict.embeding(d)))
                num+=1
        basic_grap.append(h)


    

    union_grap=[[(x,torch.zeros((1,1))) for x,_,_ in basic_grap[0]]]
    if torch.cuda.is_available():
        union_grap[0]=[(l[0],l[1].cuda()) for l in union_grap[0]]
    for r in basic_grap:
        for _,k,e in r:
            if k<len(basic_grap):
                union_grap.append([(x,distance_func(e,n)) for x,_,n in basic_grap[k]])
            else:
                if torch.cuda.is_available():
                    union_grap.append([(num,torch.zeros((1,1)).cuda())])
                else:
                    union_grap.append([(num,torch.zeros((1,1)))])

    re_shape_g=[[] for _ in range(len(union_grap))]
    for x,l in enumerate(union_grap):
        for y,d in l:
            re_shape_g[y-1].append((x,d))
    return re_shape_g


def mine_exp(i):
    return torch.exp(torch.clamp(i,-80,20))

class SpliterModel(torch.nn.Module):
    def __init__(self,worddict, n_hidden):
        torch.nn.Module.__init__(self)
        self.worddict=worddict
        self.hidden = torch.nn.Linear(worddict.wv_size()*2, n_hidden, bias=True)
        self.predict = torch.nn.Linear(n_hidden,1,bias=False) 
        self.hd = torch.nn.Hardtanh(0,8)




    def distance(self,x,y):  
        x=np.reshape(np.concatenate([x,y],-1),(1,-1)) 
        x=torch.from_numpy(x)
        if torch.cuda.is_available():
            x=x.cuda()#copy data to cuda
        x = self.predict(torch.tanh(self.hidden(x)))
        return self.hd(x)


    def _seq_score(self,dat):
        embs=[self.worddict.embeding(d) for d in dat]
        score=self.distance(embs[0],embs[1])
        for i in range(1,len(embs)-1):
            score+=self.distance(embs[i],embs[i+1])
        return score



    def _log_sum_norm(self,dat):
        #make graph
        union_g=make_graph(basic_cut(dat),self.worddict,self.distance,set(dat))
        #init data
        crossed=set([0])
        unpassed=set(i for i in range(1,len(union_g)+1))
        esum=[0]*(len(union_g)+1)
        esum[0]=torch.zeros((1,1))
        if torch.cuda.is_available():
            esum[0]=esum[0].cuda()
        #calculate the prob
        while unpassed:
            for idx in unpassed.copy():
                l=union_g[idx-1]
                if any(e[0] not in crossed for e in l):
                    continue
                esum[idx]=torch.log(sum(mine_exp(esum[c]-d) for c,d in l))
                crossed.add(idx)
                unpassed.remove(idx)
        return esum[-1]


    def _mse_loss(self,dat):
        dat=insrt_space(dat)
        return self._seq_score(dat)+self._log_sum_norm(dat)




    def loss_fuc(self,td):
        all_loss =torch.cat([self._mse_loss(d) for d in td],0)
        return torch.mean(all_loss,0)
        
        

        
        

if __name__ == "__main__":
    dataset=SegDataSetIter("seg_text.txt",8,10)
    worddict=WordDict("dict.h5")
    model=SpliterModel(worddict,40)
    solver=Solver(model,dataset)
    solver.solve()
    worddict.close()
