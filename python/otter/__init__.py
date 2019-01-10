import os
import platform
if platform.python_version().startswith('2'):
    import otter_funcs_py2 as otter_func
else:
    import otter_funcs_py3 as otter_func

SEG_DICT_DIR =os.path.join(os.path.split(os.path.realpath(__file__))[0], 'dict')




MAIN_DICT=otter_func.load_otter_dict(os.path.join(SEG_DICT_DIR,"words.dic"),1)
EN_DICT=otter_func.load_otter_dict(os.path.join(SEG_DICT_DIR,"en_root.dic"),0)

def cut(unincode_input,use_basic =True):
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