#coding=utf-8
from data_reader import basic_cut,WordDict
import numpy as np
import sys

class Model():
    def __init__(self,mpath):
        param=np.load("mpath")
        self.hw=np.transpose(param["hidden.weight"])
        self.hb=param["hidden.bias"]
        self.pw=np.transpose(param["predict.weight"])


    def distance(self,x,y):
        x=np.reshape(np.concatenate([x,y],-1),(1,-1)) 
        y=np.tanh(np.dot(x,self.hw)+self.hb)
        y=np.dot(y,self.pw)
        return np.clip(y,0,8)[0][0]



def make_graph(strlit,user_dict,distance_func):
    dlist=basic_cut([strlit])
    basic_grap=[]
    widx=[]
    for i in range(len(dlist)):
        widx.append(dlist[i])
        h=[(len(widx),i+1,user_dict.embeding(dlist[i]))]
        for j in range(i+1,min(len(dlist),i+user_dict.max_wlen())):
            d="".join(dlist[i:j+1])
            if  user_dict.hasw(d):
                widx.append(d)
                h.append((len(widx),j+1,user_dict.embeding(d)))
        basic_grap.append(h)

    union_grap=[[(x[0],0) for x in basic_grap[0]]]
    for r in basic_grap:
        for _,k,e in r:
            if k<len(basic_grap):
                union_grap.append([(x,distance_func(e,n)) for x,_,n in basic_grap[k]])
            else:
                union_grap.append([(len(widx)+1,0)])

    return widx,union_grap



def choice_path(widx,union_grap):
     # init
    num=len(widx)+1
    psize=num
    dist, prev, S = [-1.0] * psize, [-1] * psize, [False] * psize
    for e, d in union_grap[0]:
        dist[e-1] = d
        prev[e-1] = 0

    # dijkstra
    while not S[num-1]:
        mindist = -1
        u = 0
        for i, (s, d) in enumerate(zip(S, dist)):
            if (not s) and d >= 0 and (d < mindist or mindist < 0):
                u = i
                mindist = d
        S[u] = True
        if S[num-1]:  # end point
            break
        for j, d in union_grap[u + 1]:
            j=j-1
            if S[j]: continue
            c = dist[u] + d
            if dist[j] < 0 or c < dist[j]:
                dist[j] = c
                prev[j] = u + 1

    # select
    path = [num]
    while path[-1] > 0:
        path.append(prev[path[-1] - 1])
    path.reverse()
    return [widx[i-1] for i in path[1:-1]]
    











if __name__ == "__main__":
    wd=WordDict("dict.h5")
    md=Model("model_dat/1548997996/model-20000.npz")
    for line in sys.stdin:
        line=line.strip()
        if not line:
            continue
        line=line.decode("utf-8")
        widx,union_grap=make_graph(line,wd,md.distance)
        listi=choice_path(widx,union_grap)
        print u"|".join(listi).encode("utf-8")


       