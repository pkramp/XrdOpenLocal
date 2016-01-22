

all:redir

test: clean
	g++ -I/xr4i/myplug/src -I./src -c *.cpp ./src/*.cc 
	g++  -o test.exe *.o -L/lustre/nyx/rz/jknedlik/xrootd_install/lib -lpthread -lXrdUtils -lXrdCl -lXrdPosix -lXrdFfs
redir: clean
	g++ -fPIC  -I ./src/ -c *.cc -std=c++11
	g++ -shared -Wl,-soname,XrdRedirLoc.so.1 -o XrdRedir.Loc.so.1.0 *.o
	
clean: 
	rm -rf *.o *.so.* *.exe

install: redir
	cp XrdRedir.Loc.so.1.0 /xr4i/plugins/lib
