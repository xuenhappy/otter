%module otter_funcs
%include "stdint.i"
%{
#include "otter_i.h"
%}

%newobject load_gseg_dict;
%delobject free_gseg_dict;

%newobject cut;
%delobject free_gseg_handle; 


%include otter_i.h