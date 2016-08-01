pushd %0\..

set _EXE_PATH_=..\x64\Release\
set _EXE_=%_EXE_PATH_%waifu2x-converter-gls.exe

set _FILE_PATH_=..\image\

%_EXE_% --model_dir models -m scale -i %_FILE_PATH_%miku.jpg -o %_FILE_PATH_%miku_x2.jpg  

pause
popd
