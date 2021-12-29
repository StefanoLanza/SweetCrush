@echo off
setlocal ENABLEDELAYEDEXPANSION
set AndroidCmdDir=%PROGRAMFILES(X86)%\Android\android-sdk\build-tools\30.0.3
set projectFolder=SDL2_GradleApp

call :sign SDL2_GradleApp , ARM64, SweetCrush
REM call :sign SDL2_GradleApp , ARM, SweetCrush
REM call :sign SDL2_GradleApp , x64, SweetCrush
REM call :sign SDL2_GradleApp , x86, SweetCrush
goto :end

:sign
SETLOCAL
set title=%1
set configuration=%2
set outTitle=%3
set dstFolder=..\bin
set inAPK=..\build\%projectFolder%\%configuration%\Release\%title%.apk
set alignedAPK=..\build\%projectFolder%\%title%_%configuration%_a.apk
set outAPK=%dstFolder%\%outTitle%_%configuration%.apk

if not exist %inAPK% GOTO file_missing_error

REM Blank line
echo:
if not exist %dstFolder% mkdir %dstFolder%

REM ZIP align
REM -f to overwrite
echo Aligning %inAPK% to %alignedAPK%
call "%AndroidCmdDir%\zipalign" -f 4 %inAPK% %alignedAPK%
IF %ERRORLEVEL% NEQ 0 goto align_error

REM SIGN
echo Signing %alignedAPK% -> %outAPK%
REM call "jarsigner" -sigalg SHA256withRSA -digestalg SHA256 -storepass android -keypass android -keystore release.jks -signedjar %signedAPK% %inAPK% androidreleasekey
call "%AndroidCmdDir%\apksigner" sign -ks release.jks --out %outAPK% %alignedAPK% 
IF %ERRORLEVEL% NEQ 0 goto sign_error

echo Successfully signed %outAPK%
goto :end

:sign_error
echo Failed to sign %alignedAPK%
goto :end

:align_error
echo Failed to align %inAPK%
goto :end

:file_missing_error
echo Cannot open file %inAPK%

:end

ENDLOCAL
REM :sign

REM eof