#!/bin/bash
if
[ "$1" == "debug" ]; then
    export XRD_LOGLEVEL=Dump

fi

###Setup the test
cat > test/xrdopenlocal.conf << EOF
lib = $PWD/XrdRedir.Loc.so
redirectlocal = test.test|/tmp/
enable = true
EOF

export XRD_PLUGIN=$PWD/XrdRedir.Loc.so
export XrdRedirLocDEFAULTCONF=$PWD/test/xrdopenlocal.conf
mkdir /tmp/xrdcptest
echo "test123" > /tmp/xrdcptest/testfile


##Run the test
xrdcp /tmp/xrdcptest/testfile .
xrdcp root://test.test//xrdcptest/testfile ./testfile2
DIFF=$(diff testfile testfile2)
if [ "$DIFF" == "" ]; then
    echo -e "\e[92m SUCCESS \e[0m"

else

    echo -e "\e[91m FAILED \e[0m"
fi
###Cleanup the test
rm -r testfile testfile2 /tmp/xrdcptest test/xrdopenlocal.conf
