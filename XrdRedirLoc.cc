
#include "XrdRedirLoc.hh"
#include <exception>
#include <cstdlib>
#include <string>
#include <utility>
#include "XrdCl/XrdClUtils.hh"
using namespace XrdCl;
XrdVERSIONINFO(XrdClGetPlugIn, Locfile);

namespace Locfile
{
//A Plugin that let the Client call to real storage if available without the xrootd redirector
enum Mode{Local,Proxy,Undefined};
    class Locfile : public XrdCl::FilePlugIn{
            
            
            private:
            static std::map<std::string,std::string> swapLocalMap ;
            static std::string proxyPrefix;
            std::string path;
            Mode mode;
            fstream* file;
            XrdCl::File file2;
          public:
            static void setProxyPrefix(std::string toProxyPrefix){
            proxyPrefix=toProxyPrefix;
            }
            static void printMaps(){
                        XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Locfile");
                log->Debug(1,"Swap to Local Map:");
            for(auto i : swapLocalMap){stringstream msg ; msg<<"\""<<i.first<<"\" to \""<<i.second<<"\""<<std::endl;log->Debug(1,msg.str().c_str());}
            
            stringstream msg; msg<<"proxyPrefix: " << proxyPrefix<<std::endl;log->Debug(1,msg.str().c_str());
            }
            static void setSwapLocalMap(std::pair<std::string,std::string>toadd){
            swapLocalMap.insert(toadd);
            }
            static void parseIntoLocalMap(std::string configline){
                //std::cout<<"parsing: "<<configline<<std::endl;
                std::string delim = ";";
                std::string subdelim="§§";   
                auto start = 0;
                auto end = configline.find(delim);
            do{
             
            std::string sub= configline.substr(start, end - start); 
                auto startx = 0;       
                auto endx = sub.find(subdelim);
                std::string lpath=sub.substr(0,endx);
                std::string rpath=sub.substr(endx+subdelim.length(),sub.length()-endx);
                setSwapLocalMap(std::make_pair(lpath,rpath)); 
                start = end + delim.length();
                end = configline.find(delim, start);
    } while (end != std::string::npos);
            } 
           
            std::string  getLocalAdressMap( std::string servername){
                auto addr=swapLocalMap.find(servername);
                if(addr==swapLocalMap.end()){
                return "NotInside";
                }
                else{
                return addr->second;
                }
            }
            std::string rewrite_path(std::string url){
                        XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
                XrdCl::URL xUrl(url);
                string path=xUrl.GetPath();
                string servername=xUrl.GetHostName();
                std::stringstream out;
                out<<"setting  url:\""<<url<<"\"";
                if(getLocalAdressMap(servername).compare("NotInside")!=0){
            mode=Local;
                        log->Debug(1,"Setting plugIn to \"local\"- mode");
                    out<<" to: \""<<path<<"\""<<std::endl;
                        log->Debug(1,out.str().c_str());
                        return path;
            }
            {
            mode=Proxy;
            
            string proxy="root://";
            proxy.append(proxyPrefix);
            proxy.append(url);
                    
                        log->Debug(1,"Setting plugIn to \"proxy-prefix\"-mode");
            out<<" to: "<<proxy<<"\""<<std::endl;
                        log->Debug(1,out.str().c_str());
            
            return proxy;
            }
            
            if(mode==Undefined){
                throw std::runtime_error("Undefined Mode in Plugin Line: __LINE__ ");
            }
            }

                //Constructor
                Locfile():file2(false){
                        XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Locfile");
                       // pFile=new File(false);
                       file=new fstream();
                       mode=Undefined;
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
                auto newurl= rewrite_path(url)    ;  
          if(this->mode==Proxy){return file2.Open(newurl,flags,mode,handler,timeout);}         
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Open");
                  
                  file->open(newurl.c_str(),std::ios::in  | std::ios::out| std::ios::trunc );

              if(file->fail()) return XRootDStatus( XrdCl::stError,
                                                    XrdCl::errOSError,
                                                    1,
                                                 "file could not be opened");
             if(file->fail())std::cout<<"fail!"<<std::endl; 
              return XRootDStatus(XrdCl::stOK,0,0,"");
                    }
              
                        virtual XRootDStatus Close(ResponseHandler *handler,uint16_t timeout){
                            if(mode==Proxy){return file2.Close(handler,timeout);}         
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Close");
                file->close();
              XRootDStatus* ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
              handler->HandleResponse(ret_st,0);
              return  XRootDStatus(XrdCl::stOK,0,0,"");

              }

                        virtual XRootDStatus Stat(bool force,ResponseHandler *handler,uint16_t timeout){
                          
                          
                        XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
                        log->Debug(1,"Locfile::Stat");
                        if(this->mode==Proxy)return file2.Stat(force,handler,timeout);
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
                            if(mode==Proxy){return file2.Read(offset,size,buffer,handler,timeout);}         
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
                if(mode==Proxy){return file2.Write(offset,size,buffer,handler,timeout); }
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
            std::map<std::string,std::string> Locfile::swapLocalMap ;
             std::string Locfile::proxyPrefix;
}

namespace XrdRedirectToLocal
{

   void RedLocalFactory::loadDefaultConf(std::map<std::string,std::string>& config){
     XrdCl::Log *log = DefaultEnv::GetLog();
         log->Debug( 1, "RedLocalFactory::loadDefaultConf" );
if(const char* env_p = std::getenv("XrdRedirLocDEFAULTCONF")){
    std::string confFile=env_p;
    std::stringstream msg;
msg<<"XrdRedirLocDEFAULTCONF file is: "<<env_p<<std::endl;
    log->Debug( 1,msg.str().c_str() );

    Status st = XrdCl::Utils::ProcessConfig( config, confFile );
        if(config.size() ==0 )throw std::runtime_error("LocFile cannot be loaded as the default plugin since it does not seem to have any content");
    if( !st.IsOK() )
    {
      return;
    }

    const char *keys[] = { "url", "lib", "enable", 0 };
    for( int i = 0; keys[i]; ++i )
    {
      if( config.find( keys[i] ) == config.end() )
      {
        return;
      }
    }

    //--------------------------------------------------------------------------
    // Attempt to load the plug in and place it in the map
    //--------------------------------------------------------------------------
    std::string url = config["url"];
    std::string lib = config["lib"];
    std::string enable = config["enable"];


    if( enable == "false" )
    {
    throw std::runtime_error("Locfile cannot be loaded as the default plugin, since \"enable\" is set \"false\"  in the XrdRedirLocDEFAULTCONF file ");
    
    }
}   

    else{throw std::runtime_error("Locfile cannot be loaded as the default plugin, since XrdRedirLocDEFAULTCONF is not set in the environment");
  }
} 
    
    RedLocalFactory::RedLocalFactory( const std::map<std::string, std::string> &config ) : 
    XrdCl::PlugInFactory(){
     XrdCl::Log *log = DefaultEnv::GetLog();
         log->Debug( 1, "RedLocalFactory::Constructor" );
        
         if(config.find("proxyPrefix")!=config.end())Locfile::Locfile::setProxyPrefix(config.find("proxyPrefix")->second);
         if(config.find("redirectlocal")!=config.end())Locfile::Locfile::parseIntoLocalMap(config.find("redirectlocal")->second);    
        if(config.size()==0){
            std::map<std::string,std::string> defaultconfig;
                log->Debug(1,"config size is zero... This is a default plugin call -> loading default config File @ XrdRedirLocDEFAULTCONF Environment Variable ");
            loadDefaultConf(defaultconfig);

         if(defaultconfig.find("proxyPrefix")!=defaultconfig.end())Locfile::Locfile::setProxyPrefix(defaultconfig.find("proxyPrefix")->second);
         if(defaultconfig.find("redirectlocal")!=defaultconfig.end())Locfile::Locfile::parseIntoLocalMap(defaultconfig.find("redirectlocal")->second);    
        } 
         Locfile::Locfile::printMaps();    
    }

XrdCl::FilePlugIn * RedLocalFactory::CreateFile( const std::string &url )
    {
          XrdCl::Log *log = XrdCl::DefaultEnv::GetLog();
              log->Debug( 1, "RedLocalFactory::CreateFile" );
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
 const std::map<std::string, std::string> &pconfig = *static_cast <const std::map<std::string, std::string> *>(arg);
void * plug= new  XrdRedirectToLocal::RedLocalFactory(pconfig);
    return plug;
  }

}
