/*
 * Author: enxu
 */
#include "strtools.h"
#include <stdio.h>
#include <string.h>

char* substr(const char* srcstr, size_t start, size_t end){
    int real_length = end-start+1;
    char *tmp;
    if (NULL == (tmp=(char*) malloc(real_length * sizeof(char)))){
        printf("Memory overflow . \n");
        exit(0);
    }
    strncpy(tmp,srcstr+start, real_length-1);
    tmp[real_length - 1] = '\0';
    return tmp;
}


/**
 * 确定utf8 char头字节数
 **/
int getutf8_cs(const unsigned char c){
	if (!(c&0x80))
		return 1;
	if (!(c&0x40))
		return 0;
	if (!(c&0x20))
		return 2;
	if (!(c&0x10))
		return 3;
	if (!(c&0x8))
		return 4;
	if (!(c&0x4))
		return 5;
	if (!(c&0x2))
		return 6;
	return -1;
}

/**
 * 
 * utf8游标迭代
 **/
int iter_utf8str(const unsigned char* utf8_str, const size_t max_len, size_t *pos) {
	unsigned char c;
	int clen=0,slen=0,cs=1;
	size_t posn;
	while (slen<7) {
		posn=(*pos)+slen;
		if(posn>=max_len)break;//长度判断，防止越界
		c =utf8_str[posn];
		if (!c)break;//字符判断
		slen++;
		//确定字节数.找到头部
		cs=getutf8_cs(c);
		if(cs<0){//非法字符失败
			if(slen>1)slen--;
			break;
		}
		//10xxxxxx 连续情形
		if(cs==0)continue;
		//找到头部大小
		if ((clen==0)&&(slen<2)) {
			if(cs==1)break;//单个情形
			clen = cs;
			continue;
		}
		//头部切换
		slen--;
		break;
	}
	if(slen<1)return ITER_UTF8_EOF;//结束
	(*pos)+=(slen-1);//偏移指针
	if(slen==clen)return ITER_UTF8_SUCC;
	return ITER_UTF8_VAILED;
}