@ECHO OFF

call build.bat ../src/demo/demo.sln x86
if errorlevel 1 goto fail_x86
call build.bat ../src/demo/demo.sln x64
if errorlevel 1 goto fail_x64

echo "All builds succeeded"
goto end

:fail_x86
echo "Failed to build x86"
goto end

:fail_x64
echo "Failed to build x64"
goto end

:end
pause
