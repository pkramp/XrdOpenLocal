#ifndef __XRDREDIRCT_TOLOCAL_PLUGIN_HH___ 
#define __XRDREDIRCT_TOLOCAL_PLUGIN_HH___ 
#include <stdio.h>
#include "XrdCl/XrdClPlugInInterface.hh"
#include <algorithm>
extern XrdCl::Log XrdClRFSLog;


namespace XrdRedirectToLocal
{
  //----------------------------------------------------------------------------
  // Plugin factory
  //----------------------------------------------------------------------------
  class RedLocalFactory: public XrdCl::PlugInFactory
  {
    public:
      //------------------------------------------------------------------------
      // Constructore
      //------------------------------------------------------------------------
     RedLocalFactory(const std::map<std::string, std::string> &config);
      //------------------------------------------------------------------------
      // Destructor

  //------------------------------------------------------------------------
     virtual ~RedLocalFactory() {}

      //------------------------------------------------------------------------
      // Create a file plug-in for the given URL
      //------------------------------------------------------------------------
      virtual XrdCl::FilePlugIn *CreateFile( const std::string &url );

      //------------------------------------------------------------------------
      // Create a file system plug-in for the give/Lon URL
    virtual XrdCl::FileSystemPlugIn *CreateFileSystem(const std::string &url);
      //------------------------------------------------------------------------
    
  private:
  };
};

#endif // __XRDREDIRCT_TOLOCAL_PLUGIN_HH___
