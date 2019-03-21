#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on date
分词数据读取脚本
@author: enxu
'''
import h5py
import numpy as np
import os
import time
import re

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


def get_idx(words,user_dict,idx_cache,emb_cache):
    if words in idx_cache:
        return idx_cache[words]
    emb_cache.append(user_dict.embeding(words))
    idx_cache[words]=len(emb_cache)-1
    return len(emb_cache)-1


def make_graph(data,user_dict):
    data=insrt_space(data)#restore space
    must_include_words=set(data)
    dlist=basic_cut(data)
    words_embs=[]
    words_idx_map={}
    basic_graph=[]
    num=1
    for st in range(len(dlist)):
        #single
        wordidx=get_idx(dlist[st],user_dict,words_idx_map,words_embs)
        row=[(num,st+1,wordidx)]
        num+=1
        #union
        et=min(len(dlist),st+user_dict.max_wlen())
        for j in range(st+1,et):
            word="".join(dlist[st:j+1])
            if (word in must_include_words) or user_dict.hasw(word):
                wordidx=get_idx(word,user_dict,words_idx_map,words_embs)
                row.append((num,j+1,wordidx))
                num+=1
        basic_graph.append(row)

    edges=[]
    union_graph=[[(x,0) for x,_,_ in basic_graph[0]]]
    for r in basic_graph:
        for _,k,e in r:
            if k<len(basic_graph):
                row=[]
                for x,_,n in basic_graph[k]:
                    edges.append((e,n))
                    row.append((x,len(edges)))
                union_graph.append(row)
            else:
                union_graph.append([(num,0)])

    reshape_g=[[] for _ in range(len(union_graph))]
    for x,l in enumerate(union_graph):
        for y,d in l:
            reshape_g[y-1].append((x,d))
    
    best_idxs=[get_idx(w,user_dict,words_idx_map,words_embs) for w in data]
    #last data
    words_embs=np.asarray(words_embs,dtype=np.float32)
    best_idxs=np.asarray(best_idxs,dtype=np.int32)
    edges=np.asarray(edges,dtype=np.int32)
    return (words_embs,best_idxs,edges,reshape_g)


class SegDataSetIter():
    def __init__(self,dpath,max_size,batch_size,worddict):
        self.worddict=worddict
        self.dpath=dpath
        self.max_size=max_size
        self.batch_size=batch_size
        self.split_reg=re.compile(ur"\s+")


    def __iter__(self):
        with open(self.dpath) as fobj:
            datas=[]
            for line in fobj:
                line=line.decode("utf-8")
                line=line.strip()
                if not line:
                    continue
                lines=self.split_reg.split(line)
                while len(lines)>self.max_size:
                    datas.append(make_graph(lines[:self.max_size],self.worddict))
                    lines=lines[self.max_size:]
                if len(lines)<2:
                    continue
                datas.append(make_graph(lines[:self.max_size],self.worddict))
                if len(datas)>self.batch_size:
                    yield datas
                    datas=[]
            if datas:
                yield datas

    def close(self):
        pass
