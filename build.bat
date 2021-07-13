@echo off
set commonCompilerFlags=-WX -W4 -wd4201 -wd4100 -wd4189 -MTd -nologo -Gm- -GR- -EHa- -O2 -Oi -FC -Z7


IF NOT EXIST build mkdir build
pushd build
del *.pdb > NUL 2> NUL
@REM del *.gmi > NUL 2> NUL
cl %commonCompilerFlags% ../src/game.cpp -Fmgame.map -LD /link -incremental:no -opt:ref -PDB:handmade_%random%%random%%random%.pdb -EXPORT:game_update
cl %commonCompilerFlags% ../src/main.cpp -Fmmain.map /link -incremental:no -opt:ref User32.lib Gdi32.lib
popd