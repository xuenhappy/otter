/*
 * Author: enxu
 */
#ifndef OTTER_STRTOOLS_H_
#define OTTER_STRTOOLS_H_
#include <stdlib.h>
#define ITER_UTF8_EOF    0
#define ITER_UTF8_SUCC   1
#define ITER_UTF8_VAILED  -1

/**
 * 
 *截取字符串 
 **/
char* substr(const char* srcstr, size_t start, size_t end);

/**
 * 
 * utf8游标迭代
 **/
int iter_utf8str(const unsigned char* utf8_str, const size_t max_len, size_t *pos);




#endif /* OTTER_STRTOOLS_H_ */