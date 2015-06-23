#include "XrdRedirLoc.cc"
int main(){
  std::map<std::string,std::string> x;
  std::cout<<"stuff1"<<std::endl;
  XrdRedirectToLocal::RedLocalFactory fac(x);
  std::cout<<"stuff2"<<std::endl;
  XrdCl::FilePlugIn * fp=fac.CreateFile("root://xrd-server///home/xrduser/stuff");
  XrdCl::OpenFlags::Flags f;
  Access::Mode mode;
const char * test="teststuff";

  fp->Open("root://xrd-server///xrdmount/data/i", f , mode , new XrdCl::ResponseHandler,167);
  fp->Write(13,9,(void *)test,new XrdCl::ResponseHandler,166);
  fp->Close(new XrdCl::ResponseHandler,166);
  char wait;
cin>>wait;
}
