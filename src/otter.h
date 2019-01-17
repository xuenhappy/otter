/*
 * Author: enxu
 */
#ifndef OTTER_OTTER_H_
#define OTTER_OTTER_H_

#include <vector>
#include <list>
#include <map>
#include <string>
#include <enchant/enchant.h>
#include "trie.h"



#ifdef __cplusplus
extern "C" {
#endif


/**
 *对原始的数据进行基本的切分 
 **/
void basic_split(const char* input_str,size_t len,std::vector<std::string> &result,EnchantDict *en_dict);

/**
 * 
 *按照单个字符切分字符串 
 **/
void char_split(const char* input_str,size_t len,std::vector<std::string> &result,EnchantDict *en_dict);



/**
 * 加载dict文件
 * @param:basic_mode是否采用basic切分方式加载
 **/
trie_ptr load_dict(const char* path,int basic_mode,EnchantDict *en_dict,std::map<std::string,float> &single_data);


/**
 * 对给定的list利用dict进行组合切分，并按照最短覆盖路径进行选择
 **/
void split_list(trie_ptr dict,std::map<std::string,float> &single_data,std::vector<std::string> &src_list,std::list<std::string> &result);



#ifdef __cplusplus
}
#endif

#endif /* OTTER_OTTER_H_ */