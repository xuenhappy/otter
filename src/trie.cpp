/*
 * Author: enxu
 */
#include "trie.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

inline size_t hash_string(const char* __s){
    unsigned long __h = 0;
    for ( ; *__s; ++__s)
      __h = 5 * __h + *__s;
    return (size_t)__h;
}


struct trie_node {
    //self info
    char* utf8_str;//本节点字符串
    size_t hash_val;//hash取值
    int danger; //是否为最终节点

    //sub info
    int subsize; //真实的子节点的个数
	int caplity; //允许存放的子节点的个数
    struct trie_node* *next;//所有的子节点，按照hash排序
};


char* scpy(const char* srcstr){
    int real_length =strlen(srcstr)+1;
    char *tmp;
    if (NULL == (tmp=(char*) malloc(real_length * sizeof(char)))){
        printf("Memory overflow . \n");
        exit(0);
    }
    return strcpy(tmp,srcstr);
}


/**
 * 
 * 创建trie树
 **/
trie_ptr make_trie_node(const char* str,size_t hashval){
    trie_ptr tnode=(trie_ptr)malloc(sizeof(struct trie_node));
    tnode->utf8_str=scpy(str);
    tnode->hash_val=hashval;
    tnode->danger=0;

    tnode->subsize=0;
    tnode->caplity=0;
    tnode->next=NULL;
}





/**
 * 释放trie node
 */
void free_trie_node(trie_ptr tnode){
    if(!tnode)return;
    int site = 0;
    trie_ptr sp;
    while (site<tnode->subsize) {
        sp = tnode->next[site++];
        free_trie_node(sp);
    }
    tnode->subsize=tnode->caplity=0;
    free(tnode->next);
    tnode->next=NULL;
    free(tnode->utf8_str);
    tnode->utf8_str=NULL;
    tnode->danger=tnode->hash_val=0;
    free(tnode);
}

	

//在指定位置插入子节点
void add_trie_node(trie_ptr src_tnode,int mid,trie_ptr sub_node){
    if (src_tnode->subsize+1>= src_tnode->caplity) {
        src_tnode->caplity = src_tnode->caplity + 5;
        trie_ptr *temp_next=(trie_ptr *)malloc(src_tnode->caplity*sizeof(trie_ptr));
        memcpy(temp_next, src_tnode->next, src_tnode->subsize*sizeof(trie_ptr));
        free(src_tnode->next);
        src_tnode->next=temp_next;
    }
    //转移内容
    if(src_tnode->subsize>mid)
        memmove(src_tnode->next+mid+1,src_tnode->next+mid,(src_tnode->subsize-mid)*sizeof(trie_ptr));
    //设置取值
    src_tnode->next[mid]=sub_node;
    src_tnode->subsize++;
}

//二分查找找到对应的子节点,返回子节点所在的位置，否则需要插入的位置的相反数-1
int binary_search_sub(trie_ptr src_tnode,const char* utf8_str,size_t chash_val){
    int begin = 0, last = src_tnode->subsize - 1, mid;
    trie_ptr node;
    while (begin <= last) {
        mid = (begin + last) / 2;
        node = src_tnode->next[mid];
        if((node->hash_val)<chash_val){
            begin = mid + 1;
            continue;
        }
        if((node->hash_val)>chash_val){
            last = mid - 1;
            continue;
        }
        if(!strcmp(utf8_str,node->utf8_str))
            return mid;
        //反向查找
        for (int i=mid-1; i >= 0; i--) {
            node = src_tnode->next[i];
            if (node->hash_val != chash_val)
                break;

            if(!strcmp(utf8_str,node->utf8_str))
                return i;
        }
        //正向查找
        for (int i=mid+1; i < src_tnode->subsize; i++) {
            node = src_tnode->next[i];
            if (node->hash_val != chash_val)
                break;
            if(!strcmp(utf8_str,node->utf8_str))
                return i;
        }
        return -1-mid; //hash冲突
    }
    return -1-ceil((begin+last)/2.0);
}

/**
 * 打印节点
 */
void print_node(trie_ptr tnode,int deep){
    if (deep > 0) {
        int times = deep;
        while (times > 0) {
            printf(" |");
            times--;
        }
    }
    printf("-%s",tnode->utf8_str);
    if (tnode->danger) {
        printf("*");
    }
    printf("\n");
    deep++;
    trie_ptr node;
    for (int j = 0; j < tnode->subsize; j++) {
        node = tnode->next[j];
        print_node(node,deep);
    }
}




/**
 * 向trie树中加入一个序列
 */
trie_ptr insert_trie(trie_ptr tnode,const char* str,int danger){
    size_t hashval=hash_string(str);
    int index =binary_search_sub(tnode,str,hashval);
    trie_ptr tmp;
    if (index < 0){
        tmp=make_trie_node(str,hashval);
        add_trie_node(tnode,-index-1,tmp);
    } else
        tmp = tnode->next[index];
    if(!tmp->danger)//修改结尾信息
        tmp->danger = danger;
    return tmp;
}

	


/**
 * 查找序列包含的所有前缀
 */
void findseq(trie_ptr root,const std::vector<std::string> &seq, std::list<trie_match_result> &result){
    std::list<std::pair<trie_ptr, trie_match_result> > paths;
    std::list<std::pair<trie_ptr, trie_match_result> >::iterator pit;
    std::vector<std::string>::const_iterator cit;
    size_t index;
    size_t hash;
    int idx;
    trie_ptr tmp;
    trie_match_result match_tmp;
    for(cit=seq.begin(),index=0;cit!=seq.end();++cit,++index){
        hash =hash_string(cit->c_str());
        //检查path路径是否可以继续
        for(pit=paths.begin();pit!=paths.end();){
            tmp=pit->first;
            if(tmp->danger){
                trie_match_result &h=pit->second;
                h.et=index-1;
                result.push_back(h);
            }
            idx=binary_search_sub(tmp,cit->c_str(),hash);
            if(idx<0){
                pit=paths.erase(pit);
                continue;
            }
            tmp=tmp->next[idx];
            pit->first=tmp;
            ++pit;
        }
        //检查是否存在以ch开头的元素
        idx =binary_search_sub(root,cit->c_str(),hash);
        if(idx>=0){
            tmp=root->next[idx];
            match_tmp.st=index;
            match_tmp.et=0;
            paths.push_back(std::pair<trie_ptr, trie_match_result>(tmp,match_tmp));
        }
    }
    //检查path路径是否可以继续
    for(pit=paths.begin();pit!=paths.end();pit=paths.erase(pit)){
        tmp=pit->first;
        if(!tmp->danger)
            continue;
        trie_match_result &h=pit->second;
        h.et=index-1;
        result.push_back(h);
    }
}


