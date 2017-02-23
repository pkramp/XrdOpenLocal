#!/bin/bash
if
[ "$1" == "debug" ]; then
    export XRD_LOGLEVEL=Dump

fi

    export XRD_LOGLEVEL=Dump
###Setup the test
cat > test/XrdOpenLocal.conf << EOF
url = root://test.test
lib = $PWD/XrdOpenLocal.so
redirectlocal = a|b
enable = true
EOF
export XRD_PLUGINCONFDIR=$PWD/test
mkdir /tmp/xrdcptest
echo "test_NODEFAULT" > /tmp/xrdcptest/testfile


##Run the test
echo -e "\e[93m xrdcp a local file \e[0m"
xrdcp /tmp/xrdcptest/testfile .
echo -e "\e[93m xrdcp a file from a non existing "test.test" remote and redirecting it to local \e[0m"
xrdcp root://test.test//xrdcptest/testfile ./testfile2
DIFF=$(diff testfile testfile2)
if  [ $? -eq 0 ] && [ "$DIFF" == "" ]; then
    echo -e "\e[92m SUCCESS \e[0m"
else
    echo -e "\e[91m FAILED \e[0m"
fi
###Cleanup the test
rm -r testfile testfile2 /tmp/xrdcptest test/XrdOpenLocal.conf
