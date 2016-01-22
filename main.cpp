#include "XrdRedirLoc.cc"
int main(int argc, char** argv){

  std::map<std::string,std::string> x;
  XrdCl::File * fp=new XrdCl::File(true);
  XrdCl::OpenFlags::Flags f;
  f=XrdCl::OpenFlags::Write;
  Access::Mode mode;
  const char * test="i have been written from the local client";
  XrdCl::ResponseHandler* handler=new XrdCl::ResponseHandler;
  fp->Open(argv[1], f , mode ,handler,0);
  fp->Write(13,9,(void *)test,handler,0);
  fp->Close(handler,0);
}
