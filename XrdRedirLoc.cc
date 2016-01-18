
#include "XrdCl/XrdClFile.hh"
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClPlugInInterface.hh"
#include "XrdCl/XrdClLog.hh"
#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClURL.hh"
#include "XrdCl/XrdClPostMaster.hh"
#include "XrdCl/XrdClJobManager.hh"
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
            std::string path;
            fstream* file;
            XrdCl::File file2;
          public:
                //Constructor
                Locfile(std::string filepath){
			exit(123);
                        XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Locfile()");
                       // pFile=new File(false);
                       //
                        XrdCl::URL xurl(filepath);
                        path=xurl.GetPath();
                 //       path.prepend("file://");
			file=new fstream();        
                //file2=XrdCl::File(path);
XrdCl::PostMaster *postMaster = DefaultEnv::GetPostMaster();
XrdCl::JobManager *jmr=postMaster->GetJobManager();
jmr->Stop();
}       

               
                //Destructor
                ~Locfile(){
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::~Locfile()");
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
                        log->Debug(1,"Locfile::Open()");
                  XrdCl::URL xUrl(url);
                  path="file://";
		 path.append(xUrl.GetPath());
path.erase( remove( path.begin(), path.end(), ' ' ), path.end() );
struct timespec tim,tim2;
tim.tv_sec=5;
nanosleep(&tim,NULL) ;
                        log->Debug(1,"Locfile::file2.Open()");
		XRootDStatus ll= file2.Open(path,flags,mode,handler,timeout);      
std::cout<<"stuffx"<<std::endl;                
        log->Debug(1,"Locfile::file.Open().ready");
		return    ll;            
  xUrl=URL(path);
std::string debuginfo="trying local path: ";
debuginfo.append(path);
                           log->Debug(2,debuginfo.c_str()); 
                  //Work for different file types

              file->open(path.c_str(),std::fstream::in|std::fstream::out|std::fstream::app);
              if(file->fail())
				{ return XRootDStatus( XrdCl::stError,
                                                    XrdCl::errOSError,
                                                    1,
                                                 "file could not be opened");
              }
	XRootDStatus st=XRootDStatus(XrdCl::stOK,0,0,"");              
XRootDStatus* ret_st = new XRootDStatus(st);
            handler->HandleResponse(ret_st, 0);
return st;
                    }
              
                        virtual XRootDStatus Close(ResponseHandler *handler,uint16_t timeout){
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"LocFile::Close()");
	return	file2.Close(handler,timeout);
                file->close();
              XRootDStatus* ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
              handler->HandleResponse(ret_st,0);
              return  XRootDStatus(XrdCl::stOK,0,0,"");

              }

                        virtual XRootDStatus Stat(bool force,ResponseHandler *handler,uint16_t timeout){
                          
                          
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"RedLocalFile::Stat");
			return file2.Stat(force,handler,timeout);                        
if(file!=NULL){
                        struct stat s;
                        stat(path.c_str(),&s);
//build all POSIX stat here;
             std::cout<<"File: "<< path <<"\n"<<
                        "length: "<< s.st_size <<"\n"<<
                        "last mod: "<< s.st_mtime <<"\n"<<std::endl;
XRootDStatus st=XRootDStatus(XrdCl::stOK,0,0,"");              
XRootDStatus* ret_st = new XRootDStatus(st);
handler->HandleResponse(ret_st, 0);
return st;
}
                        else{

return XRootDStatus( XrdCl::stError,XrdCl::errOSError,-1,"no file opened error");
                        }
                          }
                        
                        virtual XRootDStatus Read(uint64_t offset,uint32_t size,
                                                  void  *buffer,XrdCl::ResponseHandler *handler, 
                                                  uint16_t timeout ){
              	return file2.Read(offset,size,buffer,handler,timeout);          
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"RedLocalFile::Read()");
                       file->seekp(offset);
                       file->read((char*)buffer,size);
              return  XRootDStatus(XrdCl::stOK,0,0,"");

                        }
                        
                        virtual XRootDStatus Write(
                            uint64_t offset, uint32_t size,const void *buffer, 
                             XrdCl::ResponseHandler *handler,uint16_t timeout){
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"RedLocalFile::Write()");
			return	file2.Write(offset,size,buffer,handler,timeout);
                            file->seekp(offset);
                          file->write((const char *)buffer,size);
              return  XRootDStatus(XrdCl::stOK,0,0,"");
                        }
        };

        class Locfilesys : public XrdCl::FileSystemPlugIn{
          public:
                //Constructor
                Locfilesys(std::string){
                        XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::LocfileSystem");
                       // pFile=new File(false);
                               
                }       
                
                //Destructor
                ~Locfilesys(){
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::~LocfileSystem");
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
              log->Debug( 1, "RedLocalFactory::CreateFile" );
                  return static_cast<XrdCl::FilePlugIn *> (new Locfile::Locfile(url)) ;
                    }

XrdCl::FileSystemPlugIn * RedLocalFactory::CreateFileSystem(const std::string &url){
          XrdCl::Log *log = XrdCl::DefaultEnv::GetLog();
              log->Debug( 1, "RedLocalFactory::CreateFileSystem" );
                  return static_cast<XrdCl::FileSystemPlugIn *> (new Locfile::Locfilesys(url)) ;

}
}
extern "C"{
  
  void *XrdClGetPlugIn(const void *arg){
 //const std::map<std::string, std::string> &pconfig = *static_cast <const std::map<std::string, std::string> *>(arg);
    const std::map<std::string, std::string> pconfig;

    void * plug= new  XrdRedirectToLocal::RedLocalFactory(pconfig);
    return plug;
  }

}
