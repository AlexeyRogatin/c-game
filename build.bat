@echo off
pushd build
cl -Od /Zi ../src/main.cpp /link User32.lib Gdi32.lib
popd