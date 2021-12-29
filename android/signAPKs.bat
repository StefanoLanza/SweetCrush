@echo off
setlocal ENABLEDELAYEDEXPANSION
set AndroidCmdDir=%PROGRAMFILES(X86)%\Android\android-sdk\build-tools\30.0.3
set projectFolder=SDL2_GradleApp

call:sign SDL2_GradleApp , ARM, SweetCrush
call:sign SDL2_GradleApp , ARM64, SweetCrush
call:sign SDL2_GradleApp , x86, SweetCrush
call:sign SDL2_GradleApp , x64, SweetCrush
goto:eof

:sign
SETLOCAL
set title=%1
set configuration=%2
set outTitle=%3
set dstFolder=..\bin
set inAPK=..\build\%projectFolder%\%configuration%\Release\%title%.apk
set signedAPK=..\build\%projectFolder%\%title%_%configuration%_ua.apk
set outAPK=%dstFolder%\%outTitle%_%configuration%.apk

if not exist %inAPK% GOTO file_missing_error

REM Blank line
echo:
echo Signing %inAPK%
if not exist %dstFolder% mkdir %dstFolder%

REM SIGN
REM call "jarsigner" -sigalg SHA256withRSA -digestalg SHA256 -storepass android -keypass android -keystore release.jks -signedjar %signedAPK% %inAPK% androidreleasekey
call "%AndroidCmdDir%\apksigner" sign -ks release.jks --out %signedAPK% %inAPK% 
IF %ERRORLEVEL% NEQ 0 goto sign_error
echo %inAPK% signed as %signedAPK%

REM ZIP align
REM -f to overwrite
echo Aligning %signedAPK% to %outAPK%
call "%AndroidCmdDir%\zipalign" -f 4 %signedAPK% %outAPK%
IF %ERRORLEVEL% NEQ 0 goto align_error

echo Successfully signed %outAPK%
goto :end

:sign_error
echo Failed to sign %inAPK%
goto :end

:align_error
echo Failed to align %signedAPK%
goto :end

:file_missing_error
echo Cannot open file %inAPK%
:end

ENDLOCAL
REM :sign

REM eof