#include "XrdRedirLoc.hh"
#include <thread>
#include <chrono>
int main(int argc, char** argv)
{

	std::map<std::string,std::string> x;
	XrdCl::File * fp=new XrdCl::File(true);
	XrdCl::OpenFlags::Flags f;
	f=XrdCl::OpenFlags::Write;
	XrdCl::Access::Mode mode;
	string test="i have been written from the local client";
	XrdCl::ResponseHandler* handler=new XrdCl::ResponseHandler;
	fp->Open(argv[1], f , mode ,handler,0);
// fp->Write(0,test.size(),(void  *)test.data(),handler,0);
	char * buffer;
	//
	buffer=new char [5000] {'X'};
	fp->Read(0,5000,(void*)buffer,handler,0);
	fp->Sync();
	std::cout<<buffer<<std::endl;
	fp->Close(handler,0);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	XrdCl::File * fg=new XrdCl::File(true);
	fg->Open(argv[2], f ,mode,handler,0);
	fg->Write(0,15,(void *)buffer,handler,0);
	fg->Close( handler,0);
}
