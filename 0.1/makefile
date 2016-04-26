

all:redir
redir: clean_o clean_lib
	g++ -g3 -fPIC  -I ./src/ -c *.cc -std=c++11
	g++ -shared -L/lustre/nyx/rz/jknedlik/xrootd_422/lib -Wl,-soname,XrdRedirLoc.so.1,--export-dynamic -o XrdRedir.Loc.so *.o -lXrdUtils -lXrdCl
	
clean:clean_o clean_lib clean_exe 

clean_o:
	-rm  *.o 
clean_lib:
	-rm *.so
clean_exe:
	-rm -rf *.exe
