@echo off
pushd build
del *.pdb > NUL 2 > NUL
cl -O2 /Zi ../src/game.cpp /LD /link /PDB:game_%random%.pdb /EXPORT:game_update
cl -O2 /Zi ../src/main.cpp /link User32.lib Gdi32.lib
popd