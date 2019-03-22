#!/usr/bin/env python  
# -*- coding: utf-8 -*- 
import torch
import torch.nn.functional as F
from torch import optim
from torch.autograd import Variable
from muti_batch_feed import MultiTaskBatchWapper
import os
import numpy as np
import time
from data_reader import WordDict,SegDataSetIter


    
class Solver():
    def __init__(self,model,dataset,lr=1e-4,epoch_num=2,iter_num=4):
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
        params=self.model.parameters()
        self.optimizer = optim.SGD(params, lr=self.lr,weight_decay=2e-5)
        timestamp = str(int(time.time()))
        out_dir = os.path.abspath(os.path.join(os.path.curdir,"model_dat", timestamp))
        print("Write model to {}".format(out_dir))
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)
        self.out_dir = out_dir

    def save_torch_model(self,epoch_num):
        #save model for torch
        torch.save(self.model, os.path.join(self.out_dir, "model-%d.pkl"%epoch_num))
        state_dict=dict((k,v.cpu().numpy()) for (k,v) in self.model.state_dict().items())
        np.savez(os.path.join(self.out_dir, "model-%d"%epoch_num),**state_dict)
        


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
                    self.optimizer.zero_grad() 
                    loss=self.model.loss_fuc(td)
                    loss.backward()
                    self.optimizer.step()
                    print('train [epoch|sample|step]:[%d|%d|%d] loss:%g' % (epoch_num,sample_num,step, float(loss)))
                if sample_num%10000==0:
                    self.save_torch_model(sample_num)
        print("finish train job...")
            


class SplitModel(torch.nn.Module):
    def __init__(self,input_size, n_hidden):
        torch.nn.Module.__init__(self)
        self.predict =torch.nn.Sequential(
            torch.nn.Linear(input_size, n_hidden),
            torch.nn.Tanh(),
            torch.nn.Linear(n_hidden,1),
            torch.nn.Softplus()
        )

    def distance(self,x,y):
        return self.predict(torch.cat([x,y],-1))

    def _score_seqs(self,words_embs,best_idxs):
        feats=words_embs.index_select(0,best_idxs)
        feats=feats.contiguous()
        dists=self.distance(feats[:-1],feats[1:])
        return dists.sum(0)
    
    def _logsumexp(self,vec):
        vec=torch.cat(vec,0)
        m,_=vec.max(0)
        rs=m+torch.log(torch.exp(vec-m).sum(0))
        return rs.view(1,)

    def _forward_alg(self,words_embs,edges,graph):
        #init data
        edges_st,edges_et=edges.split([1,1],dim=1)
        semb=words_embs.index_select(0,edges_st.view(-1,))
        eemb=words_embs.index_select(0,edges_et.view(-1,))
        distances=self.distance(semb,eemb)
        zeros_=torch.zeros((1,1)).to(distances.device)
        distances=torch.cat([zeros_,distances],0)
        #calc
        crossed=set([0])
        unpassed=set(i for i in range(1,len(graph)+1))
        esum=[0]*(len(graph)+1)
        esum[0]=zeros_[0]
        #calculate the prob
        while unpassed:
            for idx in unpassed.copy():
                l=graph[idx-1]
                if any(e[0] not in crossed for e in l):
                    continue
                esum[idx]=self._logsumexp([esum[c]-distances[d] for c,d in l])
                crossed.add(idx)
                unpassed.remove(idx)
        return esum[-1]


    def _single_loss(self,words_embs,best_idxs,edges,reshape_g):
        words_embs=torch.from_numpy(words_embs).float()
        best_idxs=torch.from_numpy(best_idxs).long()
        edges=torch.from_numpy(edges).long()
        if torch.cuda.is_available():
            words_embs=words_embs.cuda()
            best_idxs=best_idxs.cuda()
            edges=edges.cuda()
        gold_score=self._score_seqs(words_embs,best_idxs)
        forward_score =self._forward_alg(words_embs,edges,reshape_g)
        return gold_score+forward_score

    def loss_fuc(self,datas):
        all_loss=[self._single_loss(*data) for data in datas]
        return torch.cat(all_loss).mean()
         
        
        

if __name__ == "__main__":
    worddict=WordDict("dict.h5")
    dataset=SegDataSetIter("seg_text.txt",15,10,worddict)
    dataset=MultiTaskBatchWapper(dataset,10)
    model=SplitModel(worddict.wv_size()*2,40)
    solver=Solver(model,dataset)
    solver.solve()
    worddict.close()
    dataset.close()
