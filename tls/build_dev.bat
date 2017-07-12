SET msbuild="C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe"
SET solution=../src/demo/demo.sln
SET platform=x64
SET switches=/t:rebuild /v:m

%msbuild% %solution% %switches% /p:Configuration=Debug;Platform=%platform%

exit /B 0
