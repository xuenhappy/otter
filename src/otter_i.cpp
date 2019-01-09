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


seg_dict_ptr load_gseg_dict(const char* path,int basic_mode){
    seg_dict_ptr dc=new struct seg_dict();
    dc->dict=load_dict(path,basic_mode);
    if(!dc->dict){
        delete dc;
        return NULL;
    }
    dc->en_broker=enchant_broker_init();
    dc->en_dict=enchant_broker_request_dict(dc->en_broker,"en_US");
    if(!dc->en_dict){
        free_trie_node(dc->dict);
        enchant_broker_free(dc->en_broker);
        delete dc;
        return NULL;
    }
    return dc;
}


void free_gseg_dict(seg_dict_ptr dict_obj){
    if(dict_obj){
        free_trie_node(dict_obj->dict);
        enchant_broker_free_dict(dict_obj->en_broker,dict_obj->en_dict);
        enchant_broker_free(dict_obj->en_broker);
    }
}


seg_result_ptr cut(seg_dict_ptr dict_obj,const char* utf_input,unsigned int len,int basic_mode){
    std::list<std::string> *data=new std::list<std::string>();
    std::list<std::string> strlist;
    if(basic_mode){
        basic_split(utf_input,len,strlist,dict_obj->en_dict);
    }else{
        char_split(utf_input,len,strlist,dict_obj->en_dict);
    }
    split_list(dict_obj->dict,strlist,*data);
    seg_result_ptr res=new struct seg_result();
    res->data=data;
    res->it=data->begin();
    return res;
}


const char* next(seg_result_ptr res){
    if(res->it!=res->data->end()){
        const char* out=res->it->c_str();
        ++(res->it);
        return out;
    }
    return NULL;
}


void free_gseg_handle(seg_result_ptr res){
    if(res){
        delete res->data;
        delete res;
    }
}

