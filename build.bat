@echo off
pushd build
cl -O2 /Zi ../src/game.cpp /LD /link /EXPORT:game_update
cl -O2 /Zi ../src/main.cpp /link User32.lib Gdi32.lib
popd