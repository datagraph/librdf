#/bin/sh
export CC='ccache clang -std=c11 -Qunused-arguments -fcolor-diagnostics'
export CXX='ccache clang++ -std=c++11 -Qunused-arguments -fcolor-diagnostics'
export CPPFLAGS=''
export CFLAGS='-g -Os'
export CXXFLAGS="$CFLAGS"
export LDFLAGS=''
./configure $*
