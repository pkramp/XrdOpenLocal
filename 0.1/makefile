
ifdef DEBUG
DBG="debug"
endif

ifndef XRD_PATH
$(error XRD_PATH is not set, set XRD_PATH to your XRootD installation)
endif
all:XrdRedir.Loc.so
 	
XrdRedir.Loc.so: 
	g++ -g3 -fPIC  -I$(XRD_PATH)/include/xrootd -I./src/ -c *.cc -std=c++11
	g++ -shared  -L$(XRD_PATH)/lib -Wl,-soname,XrdRedirLoc.so.1,--export-dynamic -o XrdRedir.Loc.so *.o -lXrdUtils -lXrdCl
	
test: XrdRedir.Loc.so
	@./test/xrdcp.test.sh $(DBG)

clean:clean_o clean_lib clean_exe 

clean_o:
	@-rm  *.o 
clean_lib:
	@-rm *.so
clean_exe:
	@-rm -rf *.exe

.PHONY: test
