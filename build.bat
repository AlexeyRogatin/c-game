@echo off
set commonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -FC -Z7


IF NOT EXIST build mkdir build
pushd build
del *.pdb > NUL 2> NUL
cl %commonCompilerFlags% ../src/game.cpp -Fmgame.map -LD /link -incremental:no -opt:ref -PDB:handmade_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb -EXPORT:game_update
cl %commonCompilerFlags% ../src/main.cpp -Fmmain.map /link -incremental:no -opt:ref User32.lib Gdi32.lib
popd