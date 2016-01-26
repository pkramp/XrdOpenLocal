

all:redir

test: clean_exe   
	g++ -I/xr4i/myplug/src -I./src -c *.cpp ./src/*.cc -std=c++11
	g++  -o test.exe main.o  -L/lustre/nyx/rz/jknedlik/xrootd_install/lib -lpthread -lXrdUtils -lXrdCl -lXrdPosix -lXrdFfs 
redir: clean_o clean_lib
	g++ -fPIC  -I ./src/ -c *.cc -std=c++11
	g++ -shared -Wl,-soname,XrdRedirLoc.so.1 -o XrdRedir.Loc.so *.o
	
clean:clean_o clean_lib clean_exe 

clean_o:
	-rm  *.o 
clean_lib:
	-rm *.so
clean_exe:
	-rm -rf *.exe
install: redir
	cp XrdRedir.Loc.so.1.0 /xr4i/plugins/lib
