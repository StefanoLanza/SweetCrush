@echo off
setlocal 
set _7z="\Program Files\7-Zip\7z.exe"
set zip=game.zip
cd bin
del %zip%
call %_7z% a -tzip %zip% *.exe *.dll assets
endlocal
