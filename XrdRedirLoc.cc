
#include "XrdCl/XrdClFile.hh"
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClPlugInInterface.hh"
#include "XrdCl/XrdClLog.hh"
#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClURL.hh"
#include "XrdRedirLoc.hh"
#include "XrdOuc/XrdOucEnv.hh"
#include "XrdOuc/XrdOucString.hh"
#include "XrdVersion.hh"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept> 
#include <iostream>
#include <sstream>
#include <fstream>


using namespace XrdCl;
XrdVERSIONINFO(XrdClGetPlugIn, Locfile);

namespace Locfile
{
//A Plugin that let the Client call to real storage if available without the xrootd redirector
        class Locfile : public XrdCl::FilePlugIn{
            
            
            private:
            static std::map<std::string,std::string> swapAdressMap;
            std::string path;
            fstream* file;
            XrdCl::File file2;
          public:
                //Constructor
                Locfile(){
                        XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Locfile");
                       // pFile=new File(false);
                       file=new fstream();        
                       std::cout<<"creating new LocFile"<<std::endl;
                }       

               
                //Destructor
                ~Locfile(){
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::~Locfile");
                        //delete pFile;
                        delete file;
                }

                //Open()

                        virtual XRootDStatus Open( const std::string &url,
                                 OpenFlags::Flags   flags,
                                 Access::Mode       mode,
                                 ResponseHandler   *handler,
                                 uint16_t           timeout )
      {
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"open!");
                  XrdCl::URL xUrl(url);
                  path=xUrl.GetPath();
                  std::cout<<"path:"<< path<<std::endl;
                  //Work for different file types
              file->open(path.c_str(),std::ios::in  | std::ios::out| std::ios::trunc );

              if(file->fail()) return XRootDStatus( XrdCl::stError,
                                                    XrdCl::errOSError,
                                                    1,
                                                 "file could not be opened");
             if(file->fail())std::cout<<"fail!"<<std::endl; 
              return XRootDStatus(XrdCl::stOK,0,0,"");
                    }
              
                        virtual XRootDStatus Close(ResponseHandler *handler,uint16_t timeout){
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"close!");
                file->close();
              XRootDStatus* ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
              handler->HandleResponse(ret_st,0);
              return  XRootDStatus(XrdCl::stOK,0,0,"");

              }

                        virtual XRootDStatus Stat(bool force,ResponseHandler *handler,uint16_t timeout){
                          
                          
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"RedLocalFile::Stat");
                        if(file!=NULL){
                        struct stat s;
                        stat(path.c_str(),&s);
//build all POSIX stat here;
             std::cout<<"File: "<< path <<"\n"<<
                        "length: "<< s.st_size <<"\n"<<
                        "last mod: "<< s.st_mtime <<"\n"<<std::endl;
              return  XRootDStatus(XrdCl::stOK,0,0,"");
                        }
                        else{return XRootDStatus( XrdCl::stError,XrdCl::errOSError,-1,"no file opened error");
                        }
                          }
                        
                        virtual XRootDStatus Read(uint64_t offset,uint32_t size,
                                                  void  *buffer,XrdCl::ResponseHandler *handler, 
                                                  uint16_t timeout ){
                        
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"RedLocalFile::Read");
                       file->seekp(offset);
                       file->read((char*)buffer,size);
              return  XRootDStatus(XrdCl::stOK,0,0,"");

                        }
                        
      XRootDStatus Write( uint64_t         offset,
                          uint32_t         size,
                          const void      *buffer,
                          ResponseHandler *handler,
                          uint16_t         timeout = 0 )
                {
                    std::cout<<"write!"<<std::endl;
                    file->seekg(offset);
                file->write((char*)buffer,size);
                    XRootDStatus* ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
              handler->HandleResponse(ret_st,0);
              return  XRootDStatus(XrdCl::stOK,0,0,"");
                }
        };

        class Locfilesys : public XrdCl::FileSystemPlugIn{
          public:
                //Constructor
                Locfilesys(){
                        XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Locfile");
                       // pFile=new File(false);
                               
                }       
                
                //Destructor
                ~Locfilesys(){
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::~Locfile");
                        //delete pFile;
                        
                }

                //Open()

                        virtual XRootDStatus Locate( const std::string &path,
                                 OpenFlags::Flags   flags,
                                 ResponseHandler   *handler,
                                 uint16_t           timeout )
      {
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"working1");
                       return  XrdCl::FileSystemPlugIn::Locate(path,flags,handler,timeout);
        }

        };
}

namespace XrdRedirectToLocal
{
  RedLocalFactory::RedLocalFactory( const std::map<std::string, std::string> &config ) : 
    XrdCl::PlugInFactory(){
     XrdCl::Log *log = DefaultEnv::GetLog();
         log->Debug( 1, "RedLocalFactory::Constructor" );
    
    }

XrdCl::FilePlugIn * RedLocalFactory::CreateFile( const std::string &url )
    {
          XrdCl::Log *log = XrdCl::DefaultEnv::GetLog();
              log->Debug( 1, "RadosFsFactory::CreateFile" );
                  return static_cast<XrdCl::FilePlugIn *> (new Locfile::Locfile()) ;
                    }

XrdCl::FileSystemPlugIn * RedLocalFactory::CreateFileSystem(const std::string &url){
          XrdCl::Log *log = XrdCl::DefaultEnv::GetLog();
              log->Debug( 1, "RadosFsFactory::CreateFileSystem" );
                  return static_cast<XrdCl::FileSystemPlugIn *> (new Locfile::Locfilesys()) ;

}
}
extern "C"{
  
  void *XrdClGetPlugIn(const void *arg){
 //const std::map<std::string, std::string> &pconfig = *static_cast <const std::map<std::string, std::string> *>(arg);
 const std::map<std::string, std::string> *pconfig = static_cast <const std::map<std::string, std::string>*>(arg);
    void * plug= new  XrdRedirectToLocal::RedLocalFactory(pconfig);
    return plug;
  }

}
