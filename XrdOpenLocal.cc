/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include "XrdOpenLocal.hh"
#include <exception>
#include <cstdlib>
#include <string>
#include <utility>
#include "XrdCl/XrdClUtils.hh"
#include <assert.h>
using namespace XrdCl;
XrdVERSIONINFO(XrdClGetPlugIn, OpenLocal);

namespace OpenLocal {
class OpenLocalFile : public XrdCl::FilePlugIn {
private:

	///@swapLocalMap a map for rewrite some server specific URL to using an local file
	// e.g. root://xrd-manager.your.site to /your/filesystem/xrdmanager (specified in the xrootd plugin config file)
	static std::map<std::string,std::string> swapLocalMap;
	///@file file for local access
	fstream* file;
	XrdCl::File xfile;
	std::string path;
public:
	static void printInfo() {
		XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
		log->Debug(1,"OpenLocalFile::printInfo");
		log->Debug(1,"Swap to Local Map:");
		for(auto i : swapLocalMap) {
			stringstream msg ;
			msg<<"\""<<i.first<<"\" to \""<<i.second<<"\""<<std::endl;
			log->Debug(1,msg.str().c_str());
		}
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

	std::string LocalPath(std::string url) {
		XrdCl::Log *log= XrdCl::DefaultEnv::GetLog();
		XrdCl::URL xUrl(url, false);

		string path=xUrl.GetPath();
		string servername=xUrl.GetHostName();
		std::stringstream out;

		out << "OpenLocal::setting  url:\"" <<url<<"\"";
		if(getLocalAdressMap(servername).compare("NotInside")!=0) {
			std::string   lpath=getLocalAdressMap(servername);
			lpath.append(path);
			this->path=lpath;

			out<<" to: \""<<lpath<<"\""<<std::endl;
			log->Debug(1,out.str().c_str());

			return lpath;
		}
	}

	//Constructor
	OpenLocalFile(std::string x):xfile(false){ //declare that xfile shall not recursively use plugins
	}

	//Destructor
	~OpenLocalFile() {
		xfile.Close();
	}

	virtual std::string ComputeURL(const std::string &url){
		XrdCl::Log *log=XrdCl::DefaultEnv::GetLog();
		
		std::string computedUrl=LocalPath(url);
		if(computedUrl.size()>0)
			return computedUrl;
		else return url;
	}

	//Open()
	virtual XRootDStatus Open( const std::string &url,
	                           OpenFlags::Flags   flags,
	                           Access::Mode       mode,
	                           ResponseHandler   *handler,
	                           uint16_t           timeout ) {
		return xfile.Open(url,flags,mode,handler,timeout);
		}

	virtual XRootDStatus Close(ResponseHandler *handler,uint16_t timeout) {
			xfile.Close(handler,timeout);
			return  XRootDStatus(XrdCl::stOK,0,0,"");


	}

	virtual bool IsOpen()  const    {
	  return xfile.IsOpen();
	}
	virtual XRootDStatus Stat(bool force,ResponseHandler *handler,uint16_t timeout) {	
		return xfile.Stat(force,handler,timeout);
	}

	virtual XRootDStatus Read(uint64_t offset,uint32_t length,
	                          void  *buffer,XrdCl::ResponseHandler *handler,
	                          uint16_t timeout ) {
		xfile.Read(offset,length,buffer,handler,timeout);
	}

	XRootDStatus Write( uint64_t         offset,
	                    uint32_t         size,
	                    const void      *buffer,
	                    ResponseHandler *handler,
	                    uint16_t         timeout = 0 ) {
	return xfile.Write(offset,size,buffer,handler,timeout);
	}
};
std::map<std::string,std::string> OpenLocalFile::swapLocalMap ;

class OpenLocalFs : public XrdCl::FileSystemPlugIn {
private:
public:
	XrdCl::FileSystem fs;
	//Constructor
	OpenLocalFs(std::string url):fs(url,false) {
	}
	//Destructor
	~OpenLocalFs() {
	}
};
};
namespace XrdOpenLocalFactory {
XOLFactory::XOLFactory( const std::map<std::string, std::string> &config ) :
	XrdCl::PlugInFactory() {
	XrdCl::Log *log = DefaultEnv::GetLog();
	log->Debug( 1, "XrdOpenLocalFactory::Constructor" );
	if(config.find("redirectlocal")!=config.end())OpenLocal::OpenLocalFile::parseIntoLocalMap(config.find("redirectlocal")->second);
	else{throw std::runtime_error("Config file does not contain any values for the redirectlocal key");}
	OpenLocal::OpenLocalFile::printInfo();
}

XOLFactory::~XOLFactory() {
}

XrdCl::FilePlugIn * XOLFactory::CreateFile( const std::string &url ) {
	return static_cast<XrdCl::FilePlugIn *> (new OpenLocal::OpenLocalFile(url)) ;
}

XrdCl::FileSystemPlugIn * XOLFactory::CreateFileSystem(const std::string &url) {
	return static_cast<XrdCl::FileSystemPlugIn *> (new OpenLocal::OpenLocalFs(url)) ;

}
}
extern "C" {
	void *XrdClGetPlugIn(const void *arg) {
		const std::map<std::string, std::string> &pconfig = *static_cast <const std::map<std::string, std::string> *>(arg);
		void * plug= new  XrdOpenLocalFactory::XOLFactory(pconfig);
		return plug;
	}
}
