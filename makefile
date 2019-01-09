all:
	g++ -g -c -I./src src/trie.cpp src/strtools.c src/otter.cpp src/otter_i.cpp
	rm python/*.a
	ar cr python/libotter.a *.o 
	rm -rf *.o
	g++ -g src/test.cpp -L./python -lotter -lenchant -o otter


swig-python2:
	cd src && swig -python -outdir ../python/2 -o ../python/2/otter_py2_wrap.c  otter.i
	

swig-python3:
	cd src && swig -python -py3 -outdir ../python/3 -o ../python/3/otter_py3_wrap.c  otter.i