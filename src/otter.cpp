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

typedef struct path_node{
    int endpoint;//边结束点
    float score;//边权重
    int idx;//边的编号
}path_node;

typedef struct node_info{
    int pre;
    int vist;
    float val;
}node_dat;



void select_path(std::map<int,std::list<path_node>* > &distance_mat,const int end_point,std::list<int> &result){
    //init data
    size_t v_size=distance_mat.size();
    node_dat vdata[v_size];
    for(size_t i=0;i<v_size;++i){
        node_dat x=vdata[i];
        x.pre=-1;
        x.vist=0;
        x.val=-1.0;
    }
    std::list<path_node> *tmp=distance_mat[0];
    std::list<path_node>::iterator it;
    for(it=tmp->begin();it!=tmp->end();++it){
        node_dat x=vdata[it->endpoint];
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
            node_dat x=vdata[i];
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
            node_dat x=vdata[it->endpoint];
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
    while(path.back()>0)
        path.push_back(vdata[path.back()-1].pre);
    path.reverse();

    std::list<int>::iterator pit=path.begin();
    int p=*pit;
    for(++pit;pit!=path.end();++pit){
        int n=*pit-1;
        tmp=distance_mat[p];
        for(it=tmp->begin();it!=tmp->end();++it)
            if((it->endpoint==n) && (it->idx>=0))
                result.push_back(it->idx);
        p=n+1;
    }

    
}

int type_ch(char ch){
    if (ch==' ')
        return 0;
    if ('a'<=ch && ch<='z')
        return 1;
    if ('A'<=ch && ch<='Z')
        return 1;
    if (u'0'<=ch && ch<='9')
        return 2;
    return -1;
}


/**
 *
 * 进行enchant切分，如果可切分则返回1，并可以获取结果，否则0 
 */
int enchant_split(std::list<std::string> &result,const std::string &str,EnchantDict *en_dict){
    //is right word
    size_t max_index=str.length();
    if(!enchant_dict_check(en_dict,str.c_str(),max_index)){
        result.push_back(str);
        return 1;
    }
    size_t idx;
    for(idx=2;idx<max_index-2;++idx){
        std::string left=str.substr(0,max_index-idx);
        std::string right=str.substr(max_index-idx,idx);
        if(!enchant_dict_check(en_dict,right.c_str(),right.length())){
            if(enchant_split(result,left,en_dict)){
                result.push_back(right);
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
void addtobuffer(std::list<std::string> &result,const std::string &str,int type,EnchantDict *en_dict){
    if(str.length()<5||type!=1){
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
    tmp.clear();
    //enchant split
    if(enchant_split(tmp,str,en_dict) && (tmp.size()>1)){
        tmp.reverse();
        for(it=tmp.begin();it!=tmp.end();++it){
            result.push_back(*it);
        }
        return;
    }
    result.push_back(str);
}



void basic_split(const char* input_str,size_t len,std::list<std::string> &result,EnchantDict *en_dict){
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


void char_split(const char* input_str,size_t len,std::list<std::string> &result,EnchantDict *en_dict){
    size_t start=0,end=0;
	while(iter_utf8str((const unsigned char*)input_str,len,&end)!=ITER_UTF8_EOF){
        result.push_back(std::string(input_str+start,end-start+1));
		end++,start=end;
	}
}

trie_ptr load_dict(const char* path,int basic_mode){
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
    std::list<std::string> word_list;
    std::list<std::string>::const_iterator it;
    trie_ptr dict=make_trie_node("trie",0);
    trie_ptr tmp;
    EnchantBroker *en_broker=enchant_broker_init();
    EnchantDict *en_dict=enchant_broker_request_dict(en_broker,"en_US");
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
    enchant_broker_free_dict(en_broker,en_dict);
    enchant_broker_free(en_broker);      
    return dict;
}




void split_list(trie_ptr dict,std::list<std::string> &src_list,std::list<std::string> &result){




    
}

