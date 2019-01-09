#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <string.h>
using namespace std;

#include "trie.h"
#include "otter.h"

int main(){
    trie_ptr root=make_trie_node("root",100);
    trie_ptr c=insert_trie(root,"as",0);
    c=insert_trie(c,"bn",1);
    c=insert_trie(c,"l",1);


    c=insert_trie(root,"as",0);
    c=insert_trie(c,"bn",0);
    c=insert_trie(c,"kk",1);

    c=insert_trie(root,"jj",0);
    c=insert_trie(c,"bn",1); 

    print_node(root,0);
    free_trie_node(root);


    const char* test_str="sd  a32142你好啊!te st";
    list<string> res;
    basic_split(test_str,strlen(test_str),res,NULL);
    printf("\nsrc: %s\n",test_str);
    list<string>::const_iterator it;
    for(it=res.begin();it!=res.end();++it){
        printf("%s|",it->c_str());
    }
    printf("\n");


    printf("-------------------\n");

    EnchantBroker *en_broker=enchant_broker_init();
    EnchantDict *en_dict=enchant_broker_request_dict(en_broker,"en_us");
    trie_ptr nn=load_dict("dict/words.dic",1,en_dict);
    if(nn){
        print_node(nn,0);
        free_trie_node(nn);
    }

    enchant_broker_free_dict(en_broker,en_dict);
    enchant_broker_free(en_broker);
    return EXIT_SUCCESS;

}