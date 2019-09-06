import os
import platform
if platform.python_version().startswith('2'):
    import otter_funcs_py2 as otter_func
else:
    from . import otter_funcs_py3 as otter_func

__all__=['cut']

SEG_DICT_DIR =os.path.join(os.path.split(os.path.realpath(__file__))[0], 'dict')




MAIN_DICT=otter_func.load_otter_dict(os.path.join(SEG_DICT_DIR,"words.dic"),1)
EN_DICT=otter_func.load_otter_dict(os.path.join(SEG_DICT_DIR,"en_root.dic"),0)

def cut_2(unincode_input,use_basic =True):
    """ do segmentation
    """
    _input=unincode_input.encode('utf-8', 'ignore')
    handle=None
    if use_basic:
        handle=otter_func.otter_cut(MAIN_DICT,_input,len(_input),1)
    else:
        handle=otter_func.otter_cut(EN_DICT,_input,len(_input),0)

    while True:
        word=otter_func.iter_otter_result(handle)
        if word is None:
            break
        yield word.decode('utf-8', 'ignore')
    otter_func.free_otter_result(handle)


def cut_3(unincode_input,use_basic =True):
    """ do segmentation
    """
    lens=len(unincode_input.encode('utf-8', 'ignore'))
    handle=None
    if use_basic:
        handle=otter_func.otter_cut(MAIN_DICT,unincode_input,lens,1)
    else:
        handle=otter_func.otter_cut(EN_DICT,unincode_input,lens,0)

    while True:
        word=otter_func.iter_otter_result(handle)
        if word is None:
            break
        yield word
    otter_func.free_otter_result(handle)



def add_str2dict(unincode_input, use_basic=True):
    """
    add strs to dict
    """
    lens=len(unincode_input.encode('utf-8', 'ignore'))
    if use_basic:
        otter_func.add_str2dict(MAIN_DICT,unincode_input,lens,1)
    else:
        otter_func.add_str2dict(EN_DICT,unincode_input,lens,0)



cut=None
if platform.python_version().startswith('2'):
    cut=cut_2
else:
    cut=cut_3