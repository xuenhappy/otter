/*
 * gseg对外的封装主函数入口
 * Author: enxu
 */


#ifndef OTTER_OTTER_I_H_
#define OTTER_OTTER_I_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct seg_dict* otter_dict_ptr;
typedef struct seg_result* otter_result_ptr;

/**
 * 
 * 加载给定文件词典
 * 
 **/
otter_dict_ptr load_otter_dict(const char* path,int basic_mode);



/***
 * 
 * 释放给定的模型文件
 * 
 **/
void free_otter_dict(otter_dict_ptr dict_obj);




/***
 * 
 * 增加字符串到数组
 * 
 **/
void add_str2dict(otter_dict_ptr dict_obj,const char* utf_input,unsigned int len,int basic_mode);

/**
 * 
 * 
 *切分给定的字符
 * 
 **/
otter_result_ptr otter_cut(otter_dict_ptr dict_obj,const char* unincode_input,unsigned int len,int basic_mode);

/**
 * 迭代
 **/
const char* iter_otter_result(otter_result_ptr result);

/**
 *释放切词中间handel
 **/
void free_otter_result(otter_result_ptr result);















  













#ifdef __cplusplus
}
#endif

#endif /* OTTER_OTTER_I_H_ */