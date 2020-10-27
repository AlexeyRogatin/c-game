@echo off
pushd build
cl -O2 /Zi ../src/main.cpp /link User32.lib Gdi32.lib
popd