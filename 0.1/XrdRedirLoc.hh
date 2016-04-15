#ifndef __XRDREDIRCT_TOLOCAL_PLUGIN_HH___
#define __XRDREDIRCT_TOLOCAL_PLUGIN_HH___
#include <stdio.h>
#include "XrdCl/XrdClPlugInInterface.hh"
#include <algorithm>
#include "XrdCl/XrdClFile.hh"
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClPlugInInterface.hh"
#include "XrdCl/XrdClLog.hh"
#include "XrdCl/XrdClDefaultEnv.hh"
#include "XrdCl/XrdClURL.hh"
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
#include <utility>
extern XrdCl::Log XrdClRFSLog;


namespace XrdRedirectToLocal {
//----------------------------------------------------------------------------
// Plugin factory
//----------------------------------------------------------------------------
class RedLocalFactory: public XrdCl::PlugInFactory {
	public:
		//------------------------------------------------------------------------
		// Constructore
		//------------------------------------------------------------------------
		RedLocalFactory(const std::map<std::string, std::string> &config);
		//------------------------------------------------------------------------
		// Destructor
		virtual ~RedLocalFactory();

		//------------------------------------------------------------------------
		// Create a file plug-in for the given URL
		//------------------------------------------------------------------------
		virtual XrdCl::FilePlugIn *CreateFile( const std::string &url );

		//------------------------------------------------------------------------
		// Create a file system plug-in for the give/Lon URL
		virtual XrdCl::FileSystemPlugIn *CreateFileSystem(const std::string &url);
		//------------------------------------------------------------------------
		// load default config:
		void loadDefaultConf(std::map<std::string,std::string>& config);
		//------------------------------------------------------------------------
		

	private:
};
};

#endif // __XRDREDIRCT_TOLOCAL_PLUGIN_HH___
