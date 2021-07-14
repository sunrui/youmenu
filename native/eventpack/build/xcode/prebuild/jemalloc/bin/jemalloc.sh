#!/bin/sh

prefix=/Users/sunrui/Documents/Develop/Project/honeysense/wx.youmenu/eventpack/3rdparty/jemalloc/out_osx
exec_prefix=/Users/sunrui/Documents/Develop/Project/honeysense/wx.youmenu/eventpack/3rdparty/jemalloc/out_osx
libdir=${exec_prefix}/lib

DYLD_INSERT_LIBRARIES=${libdir}/libjemalloc.1.dylib
export DYLD_INSERT_LIBRARIES
exec "$@"
