#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on Sep 26, 2018
多进程数据获取
@author: enxu
'''
import multiprocessing
GET_CMD="get"
STOP_CMD="stop"
ITER_STOP_CMD="<ET>"
      
class GETTER(multiprocessing.Process):
    def __init__(self,data_queue,cmd_queue,smaple_reader):
        multiprocessing.Process.__init__(self,name="getter")
        self.data_queue=data_queue
        self.cmd_queue=cmd_queue
        self.smaple_reader=smaple_reader 
        
        
    def run(self):
        while True:
            cmd_str=None
            try:
                cmd_str=self.cmd_queue.get()
            except:
                cmd_str=None
            if cmd_str is None:
                continue
            cmd_str=cmd_str.strip()
            if GET_CMD==cmd_str:
                for smaple in self.smaple_reader:
                    self.data_queue.put(smaple)
                self.data_queue.put(ITER_STOP_CMD)
            elif STOP_CMD==cmd_str:
                self.data_queue.put(ITER_STOP_CMD)
                break
            else:
                print ("not soupported cmd %s"%cmd_str)
       
                
            
            
class MultiTaskBatchWapper():
    def __init__(self,smaple_reader,cache_size):
        self.data_queue=multiprocessing.Queue(cache_size+10)
        self.cmd_queue=multiprocessing.Queue(5)
        self.getter=GETTER(self.data_queue, self.cmd_queue, smaple_reader)
        self.getter.start()
        
    def __iter__(self):
        self.cmd_queue.put(GET_CMD)
        while True:
            data=None
            try:
                data=self.data_queue.get()
            except:
                data=None
            if data is None:
                continue
            if isinstance(data, str) and ITER_STOP_CMD==data:
                break    
            yield data
    
        
    def close(self):
        self.cmd_queue.put(STOP_CMD)
        #帮助清空数据队列防止输入端阻塞
        while True:
            data=None
            try:
                data=self.data_queue.get(block=False)
            except:
                data=None
            if data is None:
                continue
            if isinstance(data, str) and ITER_STOP_CMD==data:
                break
        self.getter.join()
        self.getter.terminate()    
    