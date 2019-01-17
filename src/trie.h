/*
 * Author: enxu
 */
#ifndef OTTER_TRIE_H_
#define OTTER_TRIE_H_

#include <list>
#include <vector>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct trie_node* trie_ptr;

typedef struct trie_match_node{
    size_t st;//开始位置
    size_t et;//结束位置
    float tag;//其他基本信息
}trie_match_result;


/***
 * 构建一个Trie树节点
 ***/
trie_ptr make_trie_node(const char* str,size_t hashval);

/***
 * 释放Trie
 ***/
void free_trie_node(trie_ptr tnode);

/***
 * 打印Trie
 ***/
void print_node(trie_ptr tnode,int deep);

/***
 * 通过字符串构建Trie
 ***/
trie_ptr insert_trie(trie_ptr tnode,const char* str,float tag);


/**
 * 
 *匹配数据中出现的字符串
 *
 **/
void findseq(trie_ptr root,const std::vector<std::string> &seq, std::list<trie_match_result> &result);



#ifdef __cplusplus
}
#endif


#endif /* OTTER_TRIE_H_ */