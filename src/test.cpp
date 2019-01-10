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
    const char* test_str="hi software 北京武汉 这是一个中文测试testmax?卡哪款";
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