SET msbuild="C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe"
SET solution=%1
SET platform=%2
SET switches=/t:rebuild /v:m

%msbuild% %solution% %switches% /p:Configuration=Debug;Platform=%platform%
if errorlevel 1 exit /B 1
%msbuild% %solution% %switches% /p:Configuration=Release;Platform=%platform%
if errorlevel 1 exit /B 1
%msbuild% %solution% %switches% /p:Configuration=Production;Platform=%platform%
if errorlevel 1 exit /B 1

exit /B 0
