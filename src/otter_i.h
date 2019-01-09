/*
 * gseg对外的封装主函数入口
 * Author: enxu
 */


#ifndef OTTER_OTTER_I_H_
#define OTTER_OTTER_I_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct seg_dict* seg_dict_ptr;
typedef struct seg_result* seg_result_ptr;

/**
 * 
 * 加载给定文件词典
 * 
 **/
seg_dict_ptr load_gseg_dict(const char* path,int basic_mode);

/***
 * 
 * 释放给定的模型文件
 * 
 **/
void free_gseg_dict(seg_dict_ptr dict_obj);

/**
 * 
 * 
 *切分给定的字符
 * 
 **/
seg_result_ptr cut(seg_dict_ptr dict_obj,const char* unincode_input,unsigned int len,int basic_mode);

/**
 * 迭代
 **/
const char* next(seg_result_ptr result);

/**
 *释放切词中间handel
 **/
void free_gseg_handle(seg_result_ptr result);















  













#ifdef __cplusplus
}
#endif

#endif /* OTTER_OTTER_I_H_ */