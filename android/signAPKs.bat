@echo off
setlocal ENABLEDELAYEDEXPANSION
REM TODO
set AndroidCmdDir=%PROGRAMFILES(X86)%\Android\android-sdk\build-tools\29.0.2
set projectFolder=SDL2_GradleApp

call:sign SDL2_GradleAPP , ARM
call:sign SDL2_GradleAPP , x86
goto:eof

:sign
SETLOCAL
set title=%1
set configuration=%2
set dstFolder=..\build\%configuration%
set inAPK=..\build\%projectFolder%\%configuration%\Release\%title%.apk
set unalignedAPK=%dstFolder%\%title%_ua.apk
set outAPK=%dstFolder%\%title%.apk

echo Signing %title% %configuration%
if not exist %dstFolder% mkdir %dstFolder%

REM SIGN
jarsigner -sigalg SHA1withRSA -digestalg SHA1 -storepass android -keypass android -keystore release.keystore -signedjar %unalignedAPK% %inAPK% androidreleasekey

REM ZIP align
REM -f to overwrite
echo Aligning %unalignedAPK% to %outAPK%
call "%AndroidCmdDir%\zipalign" -f 4 %unalignedAPK% %outAPK%

echo Succesfully signed %outAPK%


ENDLOCAL
REM :sign

REM eof