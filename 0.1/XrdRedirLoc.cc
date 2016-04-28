#include "XrdRedirLoc.hh"
#include <exception>
#include <cstdlib>
#include <string>
#include <utility>
#include "XrdCl/XrdClUtils.hh"
#include <assert.h>
using namespace XrdCl;
XrdVERSIONINFO(XrdClGetPlugIn, Locfile);

namespace Locfile {
enum Mode {Local,Default,Undefined};
class Locfile : public XrdCl::FilePlugIn {


private:

	///@swapLocalMap a map for rewrite some server specific URL to using an local file
	// e.g. root://xrd-manager.your.site to /your/filesystem/xrdmanager (specified in the xrootd plugin config file)
	static std::map<std::string,std::string> swapLocalMap;
	///@proxyPrefix The prefix that will be added to any root query that cannot use local available files
	static std::string proxyPrefix;
	std::string path;
	Mode mode;
	///@file file for local access
	fstream* file;
	//(@xfile Xrootd Client File to use the proxyfied URLs
	XrdCl::File xfile;
public:
	static void setProxyPrefix(std::string toProxyPrefix) {
		proxyPrefix=toProxyPrefix;
	}
	static void printMaps() {
		XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"Locfile::printmaps");
		log->Debug(1,"Swap to Local Map:");
		for(auto i : swapLocalMap) {
			stringstream msg ;
			msg<<"\""<<i.first<<"\" to \""<<i.second<<"\""<<std::endl;
			log->Debug(1,msg.str().c_str());
		}

		stringstream msg;
		if(proxyPrefix.compare("UNSET")==0) {
			msg<<"proxyPrefix: is unset and not used"<<std::endl;
		} else {
			msg<<"proxyPrefix: " << proxyPrefix<<std::endl;
		}
		log->Debug(1,msg.str().c_str());

	}
	static void setSwapLocalMap(std::pair<std::string,std::string>toadd) {
		swapLocalMap.insert(toadd);
	}
	static void parseIntoLocalMap(std::string configline) {
		std::istringstream ss(configline);
		std::string token;

		while(std::getline(ss,token,';')) {
			std::istringstream sub(token);
			std::string lpath;
			std::string rpath;
			std::getline(sub,lpath,'|');
			std::getline(sub,rpath,'|');
			setSwapLocalMap(std::make_pair(lpath,rpath));
		}
	}

	std::string  getLocalAdressMap( std::string servername) {
		auto addr=swapLocalMap.find(servername);
		if(addr==swapLocalMap.end()) {
			return "NotInside";
		} else {
			return addr->second;
		}
	}
	std::string rewrite_path(std::string url) {
		XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();

		XrdCl::URL xUrl(url);
		string path=xUrl.GetPath();
		string servername=xUrl.GetHostName();
		std::stringstream out;

		out << "Locfile::setting  url:\"" <<url<<"\"";
		if(getLocalAdressMap(servername).compare("NotInside")!=0) {
			mode=Local;
			std::string   lpath=getLocalAdressMap(servername);
			lpath.append(path);
			this->path=lpath;

			log->Debug(1,"Locfile::rewrite Setting plugIn to \"local\"- mode");
			out<<" to: \""<<lpath<<"\""<<std::endl;
			log->Debug(1,out.str().c_str());

			return lpath;
		}


		mode=Default;
		if(proxyPrefix.compare("UNSET")==0) {
			this->path=url;
			return url;
		} else {

			string proxy="root://";
			std::stringstream protocol;
			proxy.append(protocol.str().c_str());
			proxy.append(proxyPrefix);
			proxy.append(url);
			log->Debug(1,"Locfile::rewrite Setting plugIn to \"proxy - prefix\"- mode");
			out<<" to: "<<proxy<<"\""<<std::endl;
			log->Debug(1,out.str().c_str());

		this->path=proxy;
		return proxy;
		}


	}

	//Constructor
	Locfile():xfile(false) { //declare that xfile shall not recursively use plugins
		XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"Locfile::Locfile");
		file=new fstream();
		mode=Undefined;

	}


	//Destructor
	~Locfile() {
		if(this->mode==Local && file!=NULL) delete file;
	}

	//Open()
	virtual XRootDStatus Open( const std::string &url,
	                           OpenFlags::Flags   flags,
	                           Access::Mode       mode,
	                           ResponseHandler   *handler,
	                           uint16_t           timeout ) {
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		XRootDStatus* ret_st;

		auto newurl= rewrite_path(url);
		log->Debug(1,"Locfile::Open");
		if(this->mode==Default) {
			return xfile.Open(newurl,flags,mode,handler,timeout);
		}
		if(this->mode==Local) {
			file->open(newurl.c_str(),std::ios::in  | std::ios::out| std::ios::app );
			if(file->fail()) {
				ret_st=new XRootDStatus( XrdCl::stError,XrdCl::errOSError, 1,"file could not be opened");
				handler->HandleResponse(ret_st,0);
				return *ret_st;
			} else {
				ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
				handler->HandleResponse(ret_st,0);
				return *ret_st;
			}
		}
	}

	virtual XRootDStatus Close(ResponseHandler *handler,uint16_t timeout) {
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		if(mode==Default) {
			return xfile.Close(handler,timeout);
		}

		if(mode==Local) {
			file->close();
			XRootDStatus* ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
			handler->HandleResponse(ret_st,0);
			return  XRootDStatus(XrdCl::stOK,0,0,"");
		}


	}

	virtual bool IsOpen()  const    {
		if(this->mode==Default)             return xfile.IsOpen();
		if(this->mode==Local && file!=NULL) return file->is_open();
		return false;

	}
	virtual XRootDStatus Stat(bool force,ResponseHandler *handler,uint16_t timeout) {
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"Locfile::Stat");

		if(this->mode==Default) {
			return xfile.Stat(force,handler,timeout);
		}
		if(this->mode==Local) {
			if(file->is_open() && file!=NULL) {
				struct stat s;
				stat(path.c_str(),&s);
				StatInfo* sinfo = new StatInfo();
				std::ostringstream data;
				data<<s.st_dev <<" "<< s.st_size <<" "<<s.st_mode<<" "<<s.st_mtime ;
				std::string output ="Locfile::Stat, stats are: (_dev,_size,_mode,_mtime) ";
				output.append(data.str().c_str());
				log->Debug(1,output.c_str());

				if(!sinfo->ParseServerResponse(data.str().c_str())) {
					delete sinfo;
					return XRootDStatus(XrdCl::stError, errDataError);
				} else {
					XRootDStatus* ret_st = new XRootDStatus(XrdCl::stOK, 0, 0, "");
					AnyObject* obj = new AnyObject();
					obj->Set(sinfo);
					handler->HandleResponse(ret_st, obj);
					log->Debug( 1, "Locfile::Stat returning stat structure");
					return XRootDStatus( XrdCl::stOK,0,0,"");
				}


			} else {
				log->Debug(1,"Locfile::Stat::Error No file opened");
				return XRootDStatus( XrdCl::stError,XrdCl::errOSError,-1,"no file opened error");
			}
		}
				return XRootDStatus( XrdCl::stError,XrdCl::errOSError,-1,"no file opened error");
	}

	virtual XRootDStatus Read(uint64_t offset,uint32_t length,
	                          void  *buffer,XrdCl::ResponseHandler *handler,
	                          uint16_t timeout ) {
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"Locfile::Read");
		if(mode==Default) {
			assert(xfile.IsOpen()==true);
			return xfile.Read(offset,length,buffer,handler,timeout);

		}
		if(mode==Local) {
			file->seekp(offset);
			file->read( (char*)buffer,length);
			XRootDStatus* ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
			ChunkInfo* chunkInfo=new ChunkInfo(offset,length,buffer );
			AnyObject* obj=new AnyObject();
			obj->Set(chunkInfo);
			handler->HandleResponse(ret_st,obj);
			return  XRootDStatus(XrdCl::stOK,0,0,"");
		}
	}

	XRootDStatus Write( uint64_t         offset,
	                    uint32_t         size,
	                    const void      *buffer,
	                    ResponseHandler *handler,
	                    uint16_t         timeout = 0 ) {
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"Locfile::Write");
		if(mode==Local) {
			file->seekg(offset);
			file->write((char*)buffer,size);
			XRootDStatus* ret_st=new XRootDStatus(XrdCl::stOK,0,0,"");
			handler->HandleResponse(ret_st,0);
			return  XRootDStatus(XrdCl::stOK,0,0,"");

		}
		if(mode==Default) {
			assert(xfile.IsOpen()==true);
			return xfile.Write(offset,size,buffer,handler,timeout);
		}
	    
			throw std::runtime_error("Locfilesys:: undefined mode");
	}
};
std::map<std::string,std::string> Locfile::swapLocalMap ;
std::string Locfile::proxyPrefix="UNSET";

class Locfilesys : public XrdCl::FileSystemPlugIn {
private:
	static string proxyPrefix;
public:
	Mode mode;
	std::string origURL;

	XrdCl::FileSystem fs;
	std::string rewrite_path(std::string url) {
		origURL=url;
		XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
		XrdCl::URL xUrl(url);
		string path=xUrl.GetPath();
		string servername=xUrl.GetHostName();
		string protocol=xUrl.GetProtocol();
		std::stringstream out;

		mode=Default;
		std::string proxy="";
		out.clear();

		proxy=proxyPrefix;
		log->Debug(1,"Locfilesys::rewrite Setting fs plug-In to \"Default\"-mode (includes a proxy-prefix if set)");
		out<<"Setting";
		out<<xUrl.GetURL();
		out<<" to: "<<proxy<<"\""<<std::endl;
		log->Debug(1,out.str().c_str());

		return proxy;


		if(mode==Undefined) {
			throw std::runtime_error("Locfilesys::rewrite undefined mode");
		}
	}

	std::string orig_url(std::string toadd) {
		std::stringstream x;
		x<<"/x"<<origURL<<toadd;
		return x.str();
	}

	static void setProxyPrefix(std::string toProxyPrefix) {
		proxyPrefix=toProxyPrefix;
	}
	//Constructor
	Locfilesys(std::string url):fs(rewrite_path(url),false) {
		origURL=url;
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"Locfilesys::Locfilesys");
		mode=Undefined;

	}
	//Destructor
	~Locfilesys() {
	}


	virtual XRootDStatus Locate( const std::string &path,
	                             OpenFlags::Flags   flags,
	                             ResponseHandler   *handler,
	                             uint16_t           timeout ) {
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"Locfilesys::Locate");
		return fs.Locate(orig_url(path),flags,handler,timeout);
	}


	//Constructor
	virtual XRootDStatus Stat( const std::string &path,
	                           ResponseHandler   *handler,
	                           uint16_t           timeout ) {

		XrdCl::Log *log = DefaultEnv::GetLog();
		log->Debug(1,"Locfilesys::Stat");
		return fs.Stat(orig_url(path),handler,timeout);
	}
};

std::string Locfilesys::proxyPrefix="UNSET";
}
namespace XrdRedirectToLocal {

void ReadLocalFactory::loadDefaultConf(std::map<std::string,std::string>& config) {
	XrdCl::Log *log = DefaultEnv::GetLog();
	log->Debug( 1, "RedLocalFactory::loadDefaultConf" );
	if(const char* env_p = std::getenv("XrdRedirLocDEFAULTCONF")) {
		std::string confFile=env_p;
		std::stringstream msg;
		msg<<"XrdRedirLocDEFAULTCONF file is: "<<env_p<<std::endl;
		log->Debug( 1,msg.str().c_str() );

		Status st = XrdCl::Utils::ProcessConfig( config, confFile );
		if(config.size() ==0 )throw std::runtime_error("LocFile cannot be loaded as the default plugin since the config file does not seem to have any content");
		if( !st.IsOK() ) {
			return;
		}

		const char *keys[] = { "url", "lib", "enable", 0 };
		for( int i = 0; keys[i]; ++i ) {
			if( config.find( keys[i] ) == config.end() ) {
				return;
			}
		}

		//--------------------------------------------------------------------------
		// Attempt to load the plug-in config-file and place it into the config map
		//--------------------------------------------------------------------------
		std::string url = config["url"];
		std::string lib = config["lib"];
		std::string enable = config["enable"];
		if( enable == "false" ) {
			throw std::runtime_error("Locfile cannot be loaded as the default plugin, since \"enable\" is set \"false\"  in the XrdRedirLocDEFAULTCONF file ");
		}
	} else {
		throw std::runtime_error("Locfile cannot be loaded as the default plugin, since XrdRedirLocDEFAULTCONF is not set in the environment");
	}
}

ReadLocalFactory::ReadLocalFactory( const std::map<std::string, std::string> &config ) :
	XrdCl::PlugInFactory() {
	XrdCl::Log *log = DefaultEnv::GetLog();
	log->Debug( 1, "RedLocalFactory::Constructor" );

	if(config.find("proxyPrefix")!=config.end())Locfile::Locfile::setProxyPrefix(config.find("proxyPrefix")->second);

	if(config.find("redirectlocal")!=config.end())Locfile::Locfile::parseIntoLocalMap(config.find("redirectlocal")->second);

	if(config.size()==0) {
		std::map<std::string,std::string> defaultconfig;
		log->Debug(1,"config size is zero... This is a default plugin call -> loading default config file @ XrdRedirLocDEFAULTCONF Environment Variable ");
		loadDefaultConf(defaultconfig);
		//load config for Fileplugin
		if(defaultconfig.find("proxyPrefix")!=defaultconfig.end())Locfile::Locfile::setProxyPrefix(defaultconfig.find("proxyPrefix")->second);
		if(defaultconfig.find("redirectlocal")!=defaultconfig.end())Locfile::Locfile::parseIntoLocalMap(defaultconfig.find("redirectlocal")->second);
		//load config for Filesystemplugin
		if(defaultconfig.find("proxyPrefix")!=defaultconfig.end())Locfile::Locfilesys::setProxyPrefix(defaultconfig.find("proxyPrefix")->second);
	}
	Locfile::Locfile::printMaps();
}
ReadLocalFactory::~ReadLocalFactory() {
	XrdCl::Log *log = XrdCl::DefaultEnv::GetLog();
	log->Debug( 1, "ReadLocalFactory::~RedLocalFactory" );
}

XrdCl::FilePlugIn * ReadLocalFactory::CreateFile( const std::string &url ) {
	XrdCl::Log *log = XrdCl::DefaultEnv::GetLog();
	log->Debug( 1, "ReadLocalFactory::CreateFile" );
	return static_cast<XrdCl::FilePlugIn *> (new Locfile::Locfile()) ;
}

XrdCl::FileSystemPlugIn * ReadLocalFactory::CreateFileSystem(const std::string &url) {
	XrdCl::Log *log = XrdCl::DefaultEnv::GetLog();
	log->Debug( 1, "ReadLocalFactory::CreateFilesys" );
	return static_cast<XrdCl::FileSystemPlugIn *> (new Locfile::Locfilesys(url)) ;

}
}
extern "C" {

	void *XrdClGetPlugIn(const void *arg) {
		const std::map<std::string, std::string> &pconfig = *static_cast <const std::map<std::string, std::string> *>(arg);
		void * plug= new  XrdRedirectToLocal::ReadLocalFactory(pconfig);
		return plug;
	}

}
