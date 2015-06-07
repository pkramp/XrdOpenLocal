

all:redir

test: clean
	g++ -I/xr4i/myplug/src -c *.cpp ./src/*.cc
	g++  -o test.exe *.o -L/xr4i/lib -lpthread -lXrdUtils -lXrdCl -lXrdPosix -lXrdFfs
redir: clean
	g++ -fPIC  -I /home/xrootd/xr4/xrootd-4.2.1/src/ -I /xr4i/include/xrootd -c *.cc
	g++ -shared -Wl,-soname,XrdRedirLoc.so.1 -o XrdRedir.Loc.so.1.0 *.o
	
clean: 
	rm -rf *.o *.so.* *.exe

install: redir
	cp XrdRedir.Loc.so.1.0 /xr4i/plugins/lib
