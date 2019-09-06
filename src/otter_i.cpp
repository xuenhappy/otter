#include "otter_i.h"
#include "otter.h"
#include <list>
#include <string>
#include <stdio.h>
#include <string.h>
#include <enchant/enchant.h>
#include <map>
size_t MAX_SEG_LEN=30000;

struct seg_dict{
    trie_ptr dict;
    std::map<std::string,float> *single_data;
    EnchantBroker *en_broker;
    EnchantDict *en_dict;
};

struct seg_result{
    std::list<std::string> *data;
    std::list<std::string>::const_iterator it;
};


otter_dict_ptr load_otter_dict(const char* path,int basic_mode){
    otter_dict_ptr dc=new struct seg_dict();
    dc->single_data=new std::map<std::string,float>();
    dc->en_broker=enchant_broker_init();
    dc->en_dict=enchant_broker_request_dict(dc->en_broker,"en_us");
    dc->dict=load_dict(path,basic_mode,dc->en_dict,*(dc->single_data));
    if(!(dc->en_broker&&dc->en_broker&&dc->en_dict)){
        printf("load dict failed!\n");
        free_otter_dict(dc);
        return NULL;
    }
    return dc;
}






void free_otter_dict(otter_dict_ptr dict_obj){
    if(dict_obj){
        delete dict_obj->single_data;
        free_trie_node(dict_obj->dict);
        if(dict_obj->en_dict)
            enchant_broker_free_dict(dict_obj->en_broker,dict_obj->en_dict);
        if(dict_obj->en_broker)
            enchant_broker_free(dict_obj->en_broker);
        delete dict_obj;
    }
}


void add_str2dict(otter_dict_ptr dict_obj,const char* utf_input,unsigned int len,int basic_mode){
    add_item2dict(dict_obj->dict,utf_input,len,basic_mode,dict_obj->en_dict,*(dict_obj->single_data));
}



otter_result_ptr otter_cut(otter_dict_ptr dict_obj,const char* utf_input,unsigned int len,int basic_mode){
    size_t x=strlen(utf_input);
    if(len<1||len>x)len=x;
    if(len>MAX_SEG_LEN)len=MAX_SEG_LEN;
    
    std::list<std::string> *data=new std::list<std::string>();
    std::vector<std::string> strlist;
    if(basic_mode){
        basic_split(utf_input,len,strlist,dict_obj->en_dict);
    }else{
        char_split(utf_input,len,strlist,dict_obj->en_dict);
    }
    split_list(dict_obj->dict,*(dict_obj->single_data),strlist,*data);
    otter_result_ptr res=new struct seg_result();
    res->data=data;
    res->it=data->begin();
    return res;
}


const char* iter_otter_result(otter_result_ptr res){
    if(res->it!=res->data->end()){
        const char* out=res->it->c_str();
        ++(res->it);
        return out;
    }
    return NULL;
}


void free_otter_result(otter_result_ptr res){
    if(res){
        delete res->data;
        delete res;
    }
}

