@echo off
set CLANGFORMAT="clang-format.exe"
set FILEMASK=*.c,*.cc,*.cpp,*.h,*.hh,*.hpp,*.inl
set folder = %1

call:doformat %1
goto:eof

:doformat
echo Processing directory "%1"

echo Apply .clang-format
pushd %1
for /R %%f in (%FILEMASK%) do (
    echo "%%f"
    %CLANGFORMAT% -i %%f
)

:: Add a line after public, protected or private (no option in clang-format)
:: -i edit file in place but creates an unwanted backup on Windows
::for /R %%f in (*.h) do (
::	perl -p -i -e "s/public:/public:\n/g" %%f && del %%f.bak
::	perl -p -i -e "s/protected:/protected:\n/g" %%f && del %%f.bak
::	perl -p -i -e "s/private:/private:\n/g" %%f && del %%f.bak
::)
popd


goto:eof
