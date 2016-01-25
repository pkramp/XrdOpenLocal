#include "XrdRedirLoc.cc"
int main(int argc, char** argv){

  std::map<std::string,std::string> x;
  XrdCl::File * fp=new XrdCl::File(true);
  XrdCl::OpenFlags::Flags f;
  f=XrdCl::OpenFlags::Write;
  Access::Mode mode;
  string test="i have been written from the local client";
  XrdCl::ResponseHandler* handler=new XrdCl::ResponseHandler;
  fp->Open(argv[1], f , mode ,handler,0);
  fp->Write(0,test.size(),(void  *)test.data(),handler,0);
  fp->Close(handler,0);
}
