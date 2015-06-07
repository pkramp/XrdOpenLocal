#include "XrdRedirLoc.cc"
int main(){
  std::map<std::string,std::string> x;
  std::cout<<"stuff1"<<std::endl;
  XrdRedirectToLocal::RedLocalFactory fac(x);
  std::cout<<"stuff2"<<std::endl;
//  XrdCl::FilePlugIn * fp=fac.CreateFile("root://xrd-server///home/xrduser/stuff");
  XrdCl::OpenFlags::Flags f;
  Access::Mode mode;
  //fp->Open("root://xrd-server///home/xrduser/stuff", f , mode , new XrdCl::ResponseHandler,167);
}
