#!/bin/bash
if
[ "$1" == "debug" ]; then
    export XRD_LOGLEVEL=Dump

fi
export XRD_PLUGIN=$PWD/XrdRedir.Loc.so
export XrdRedirLocDEFAULTCONF=$PWD/client.plugins.d/xrdopenlocal.conf
mkdir /tmp/xrdcptest
echo "test123" > /tmp/xrdcptest/testfile
xrdcp /tmp/xrdcptest/testfile .
xrdcp root://test.test//xrdcptest/testfile ./testfile2
DIFF=$(diff testfile testfile2)
if [ "$DIFF" == "" ]; then
    echo -e "\e[92m SUCCESS \e[0m"

else

    echo -e "\e[91m SUCCESS \e[0m"
fi
rm testfile testfile2
rm -r /tmp/xrdcptest
