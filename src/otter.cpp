/**
 * @author:xuen
 * moka基本分词器
 * 
 **/

#include "otter.h"
#include "strtools.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <set>

typedef struct path_node{
    size_t endpoint;//边结束点
    float score;//边权重
    const char* idstr;
}path_data;

struct node_dat{
    int pre;
    int vist;
    float val;
};



void select_path(std::map<size_t,std::list<path_data>* > &distance_mat,const size_t end_point,std::list<std::string> &result){
    //init data
    size_t v_size=distance_mat.size();
    node_dat* vdata=new node_dat[v_size];
    for(size_t i=0;i<v_size;++i){
        node_dat &x=vdata[i];
        x.pre=-1;
        x.vist=0;
        x.val=-1.0;
    }
    std::list<path_data> *tmp=distance_mat[0];
    std::list<path_data>::iterator it;
    for(it=tmp->begin();it!=tmp->end();++it){
        node_dat &x=vdata[it->endpoint];
        x.pre=0;
        x.val=it->score;
    }
   
    //dijkstra select
    float mindist;
    size_t u;
    while (!vdata[end_point].vist){
        mindist = -1.0;
        u = 0;
        for(size_t i=0;i<v_size;++i){
            node_dat &x=vdata[i];
            if((!x.vist) && (x.val>=0) && (x.val<mindist || mindist<0)){
                u=i;
                mindist=x.val;
            }
        }
        vdata[u].vist=1;
        if(vdata[end_point].vist)
            break;
        tmp=distance_mat[u+1];
        for(it=tmp->begin();it!=tmp->end();++it){
            node_dat &x=vdata[it->endpoint];
            if(x.vist)
                continue;
            float c=vdata[u].val+it->score;
            if(x.val<0 || c<x.val){
                x.val=c;
                x.pre=u+1;
            }

        }
    }
    

    //select
    std::list<int> path;
    path.push_back(end_point+1);
    while(path.back()>0){
        path.push_back(vdata[path.back()-1].pre);
    }
    path.reverse();

    

    std::list<int>::iterator pit=path.begin();
    int p=*pit;
    for(++pit;pit!=path.end();++pit){
        int n=*pit-1;
        tmp=distance_mat[p];
        for(it=tmp->begin();it!=tmp->end();++it)
            if((it->endpoint==n) && it->idstr)
                result.push_back(it->idstr);
        p=n+1;
    }

    delete []vdata;    
}

int type_ch(char ch){
    if (ch==' ')
        return 0;
    if ('a'<=ch && ch<='z')
        return 1;
    if ('A'<=ch && ch<='Z')
        return 1;
    if ('0'<=ch && ch<='9')
        return 2;
    return -1;
}


/**
 *
 * 进行enchant切分，如果可切分则返回1，并可以获取结果，否则0 
 */
int enchant_split(std::list<std::string> &tmp,const std::string &str,EnchantDict *en_dict){
    //is right word
    size_t max_index=str.length();
    if(!enchant_dict_check(en_dict,str.c_str(),max_index)){
        tmp.push_back(str);
        return 1;
    }
    size_t idx;
    for(idx=3;idx<max_index-2;++idx){
        std::string left=str.substr(0,max_index-idx);
        std::string right=str.substr(max_index-idx,idx);
        if(!enchant_dict_check(en_dict,right.c_str(),right.length())){
            if(enchant_split(tmp,left,en_dict)){
                tmp.push_back(right);
                return 1;
            }
        }
    }
    return 0;
}


void uplower_split(std::list<std::string> &tmp,const std::string &str){
    size_t idx;
    int pre=-1;
    int pre_t=-1;
    for(idx=0;idx<str.length();++idx){
        int type=str[idx]<'a';
        if(pre_t==type){
            continue;
        }
        if(pre_t<0){
            pre_t=type;
            pre=idx;
            continue;
        }

        if(idx>pre+1){
            tmp.push_back(str.substr(pre,idx-pre));
            pre=idx;
        }
        pre_t=type;
    }
    tmp.push_back(str.substr(pre,idx-pre));
}


/**
 *对英文进行简单的切分
 **/
void addtobuffer(std::vector<std::string> &result,const std::string &str,int type,EnchantDict *en_dict){
    if(str.length()<6||type!=1){
        result.push_back(str);
        return;
    }
    //up lower split
    std::list<std::string> tmp;
    std::list<std::string>::const_iterator it;
    uplower_split(tmp,str);
    if(tmp.size()>1){
        for(it=tmp.begin();it!=tmp.end();++it){
            result.push_back(*it);
        }
        return;
    }
    if(str.length()>32){
    //too long
        result.push_back(str);
        return;
    }
    tmp.clear();
    //enchant split
    if(enchant_split(tmp,str,en_dict) && (tmp.size()>1)){
        for(it=tmp.begin();it!=tmp.end();++it){
            result.push_back(*it);
        }
        return;
    }
    result.push_back(str);
}



void basic_split(const char* input_str,size_t len,std::vector<std::string> &result,EnchantDict *en_dict){
    result.reserve(len);
    size_t start=0,end=0;
    int buffer_st=-1,buffer_type=-1;
	while(iter_utf8str((const unsigned char*)input_str,len,&end)!=ITER_UTF8_EOF){
		if(start-end>0){//!非中文字符
            if(buffer_st>=0)//截取英文
                addtobuffer(result,std::string(input_str+buffer_st,start-buffer_st),buffer_type,en_dict);
            buffer_st=-1;
            buffer_type=-1;
            result.push_back(std::string(input_str+start,end-start+1));
        }else{
            int t=type_ch(input_str[start]);
            if(buffer_st>=0){
                if (t<=0||buffer_type!=t){
                    addtobuffer(result,std::string(input_str+buffer_st,start-buffer_st),buffer_type,en_dict);
                    if(t<=0){
                        buffer_st=-1;
                        buffer_type=-1;
                        result.push_back(std::string(input_str+start,end-start+1));
                    }else{
                        buffer_st=start;
                        buffer_type=t;
                    }
                }
            }else{
                if(t<=0){
                    buffer_st=-1;
                    buffer_type=-1;
                    result.push_back(std::string(input_str+start,end-start+1));
                }else{
                    buffer_st=start;
                    buffer_type=t;
                }
            }
        }
		end++,start=end;
	}
    if(buffer_st>=0)
        addtobuffer(result,std::string(input_str+buffer_st,start-buffer_st),buffer_type,en_dict);
	
}


void char_split(const char* input_str,size_t len,std::vector<std::string> &result,EnchantDict *en_dict){
    result.reserve(len);
    size_t start=0,end=0;
	while(iter_utf8str((const unsigned char*)input_str,len,&end)!=ITER_UTF8_EOF){
        result.push_back(std::string(input_str+start,end-start+1));
		end++,start=end;
	}
}

trie_ptr load_dict(const char* path,int basic_mode,EnchantDict *en_dict){
    FILE *fp;
    if((fp = fopen(path,"r")) == NULL){
        printf("open file %s error!\n",path);
        return NULL;
    }
    size_t bsize=1024*1024;
    char* buffer=(char*)malloc(bsize*sizeof(char));
    if(buffer==NULL){
        exit(0);
        printf("Memory overflow . \n");
    }
    std::vector<std::string> word_list;
    std::vector<std::string>::const_iterator it;
    trie_ptr dict=make_trie_node("trie",0);
    trie_ptr tmp;
    while (fgets(buffer,bsize,fp)){
        word_list.clear();
        size_t slen=strlen(buffer)-1;
        if(slen<1){
            continue;
        }
        buffer[slen]='\0';
        if(basic_mode){
            basic_split(buffer,slen,word_list,en_dict);
        }else{
            char_split(buffer,slen,word_list,en_dict);
        }
        tmp=dict;
        int s=word_list.size();
        for(it=word_list.begin();it!=word_list.end();++it){
            tmp=insert_trie(tmp,it->c_str(),--s<=0);
        }
    }
    fclose(fp);
    free(buffer);    
    return dict;
}


/**
 *合并字符串
 **/
void join_str(std::vector<std::string> &src_list,size_t st,size_t et,std::list<std::string> &dist_list){
    dist_list.push_back("");
    std::string &last=dist_list.back();
    last.clear();
    last.reserve((et-st+1)*3+5);
    for(int i=st;i<=et;i++){
        last.append(src_list[i]);
    }
}

void split_list(trie_ptr dict,std::vector<std::string> &src_list,std::list<std::string> &result){
    std::list<trie_match_result> tmp;
    findseq(dict,src_list,tmp);
    if(tmp.size()<1){//!nothing to do
        result.insert(result.end(),src_list.begin(),src_list.end());
        return;
    }
    std::map<size_t,std::list<path_data>* > distance_mat;
    std::vector<std::string>::const_iterator sit;
    std::list<trie_match_result>::const_iterator it;
    std::map<size_t,std::list<path_data>* >::iterator mit;
    path_data tnode;
    //add basic data
    size_t index=-1;
    for(sit=src_list.begin();sit!=src_list.end();++sit){
        ++index;
        tnode.score=1.0;
        tnode.endpoint=index;
        tnode.idstr=sit->c_str();
        std::list<path_data>* c=new std::list<path_data>();
        c->push_back(tnode);
        distance_mat.insert(std::pair<size_t,std::list<path_data>* >(index,c));
    }
    

    //store and add tmp join data
    std::list<std::string> join_data;
    for(it=tmp.begin();it!=tmp.end();++it){
        tnode.score=1.0;
        tnode.endpoint=it->et;
        join_str(src_list,it->st,it->et,join_data);
        tnode.idstr=join_data.back().c_str();

        mit=distance_mat.find(it->st);
        mit->second->push_back(tnode);
    }
    
    // std::list<std::string>::const_iterator v;
    // for(v=join_data.begin();v!=join_data.end();++v){
    //      printf("%s|",v->c_str());
    // } 
    // printf("\n");
    select_path(distance_mat,src_list.size()-1,result);
    //clear
    for(mit=distance_mat.begin();mit!=distance_mat.end();++mit){
        mit->second->clear();
        delete mit->second;
        mit->second=NULL;
    }
}

