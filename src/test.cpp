#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <string.h>
using namespace std;
#include "otter_i.h"

int main(){
    otter_dict_ptr dict=load_otter_dict("dict/words.dic",1);
    if(!dict){
        return 0;
    }
    const char* test_str="【据说还要有小剧场】： 　　“贱妾敬献此杯，祝贺王爷家族三百七十二人，今日同赴黄泉醉生梦死。”她十指纤纤，擎金樽一盏，笑得温软。 　　“";
    otter_result_ptr segr=otter_cut(dict,test_str,strlen(test_str),1);
    const char* c;
    while((c=iter_otter_result(segr))!=NULL){
        printf("%s|",c);
    }
    printf("\n");
    free_otter_result(segr);
    free_otter_dict(dict);
    return EXIT_SUCCESS;

}