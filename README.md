# Intel® Media SDK Samples
The Intel® Media SDK Samples showcase the broad range of features in Intel® Media SDK, which must be installed first.  
For Windows client use Intel® Media SDK (https://software.intel.com/en-us/media-sdk).  For Linux and Windows Server install Intel® Media Server Studio (https://software.intel.com/en-us/intel-media-server-studio).
For detailed information about Media SDK Samples please refer to the respective sample guides in respective folders.

# How to build
```sh
git clone https://github.com/Intel-Media-SDK/samples.git
cd samples/samples
export MFX_HOME=/path/to/mediasdk/headers
perl build.pl --cmake=intel64.make.debug
make -j8 -C __cmake/intel64.make.debug
```

`MFX_HOME` for installed MSS is `/opt/intel/mediasdk`.
