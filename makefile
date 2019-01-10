all:compile-lib;
	rm -rf *.bin
	g++ -g -fPIC  src/test.cpp -L./ -lotter -lenchant -o otter.bin
	rm -rf *.a
	
compile-lib:
	g++ -fPIC -c -g -O2 -I./src src/trie.cpp src/strtools.c src/otter.cpp src/otter_i.cpp
	rm -rf *.a
	ar cr libotter.a *.o 
	rm -rf *.o

tmp_init:
	rm -rf python/dist
	mkdir -p python/dist
	cp -rf python/otter python/dist
	cp -rf python/setup.py python/dist
	cp -rf dict python/dist/otter
	cp -rf src/otter_i.h python/dist



python:compile-lib tmp_init;
	mv libotter.a python/dist
	cp python/libenchant.a python/dist
	cd src && swig -python -outdir ../python/dist/otter -module otter_funcs_py2  -o ../python/dist/_otter_funcs_py2.c  otter.i
	cd src && swig -python -py3 -outdir ../python/dist/otter -module otter_funcs_py3 -o ../python/dist/_otter_funcs_py3.c  otter.i
	cd python/dist && python setup.py bdist_wheel
	mv python/dist/dist/*.whl .
	rm -rf python/dist
	



