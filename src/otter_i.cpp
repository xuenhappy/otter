#include "otter_i.h"
#include "otter.h"
#include <list>
#include <string>
#include <enchant/enchant.h>


struct seg_dict{
    trie_ptr dict;
    EnchantBroker *en_broker;
    EnchantDict *en_dict;
};

struct seg_result{
    std::list<std::string> *data;
    std::list<std::string>::const_iterator it;
};


otter_dict_ptr load_otter_dict(const char* path,int basic_mode){
    otter_dict_ptr dc=new struct seg_dict();
    dc->en_broker=enchant_broker_init();
    dc->en_dict=enchant_broker_request_dict(dc->en_broker,"en_US");
    dc->dict=load_dict(path,basic_mode,dc->en_dict);
    if(!(dc->en_broker&&dc->en_broker&&dc->en_dict)){
        free_otter_dict(dc);
        return NULL;
    }
    return dc;
}


void free_otter_dict(otter_dict_ptr dict_obj){
    if(dict_obj){
        free_trie_node(dict_obj->dict);
        enchant_broker_free_dict(dict_obj->en_broker,dict_obj->en_dict);
        enchant_broker_free(dict_obj->en_broker);
        delete dict_obj;
    }
}


otter_result_ptr otter_cut(otter_dict_ptr dict_obj,const char* utf_input,unsigned int len,int basic_mode){
    std::list<std::string> *data=new std::list<std::string>();
    std::vector<std::string> strlist;
    if(basic_mode){
        basic_split(utf_input,len,strlist,dict_obj->en_dict);
    }else{
        char_split(utf_input,len,strlist,dict_obj->en_dict);
    }
    split_list(dict_obj->dict,strlist,*data);
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
