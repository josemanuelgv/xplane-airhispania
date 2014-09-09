SET VERSION=030_AHSr112
SET ROOT=X-IvAp_%VERSION%
SET FOLDER=X-IvAp AHS
SET RESFOLDER=X-IvAp AHS Resources

mkdir %ROOT%

REM  Core README
xcopy ..\doc\*.txt %ROOT%

REM  Introduction
mkdir %ROOT%\introduction
copy ..\doc\*.htm %ROOT%\introduction
copy ..\doc\*.gif %ROOT%\introduction

REM  Program
mkdir "%ROOT%\for_plugins_folder\%FOLDER%"
mkdir "%ROOT%\for_plugins_folder\%FOLDER%\32"
mkdir "%ROOT%\for_plugins_folder\%FOLDER%\64"

REM 64bit version
copy "%XPlaneDir%\Resources\Plugins\%FOLDER%\64\win.xpl" "%ROOT%\for_plugins_folder\%FOLDER%\64"
copy ..\src\FMOD\FMODWindows\api\fmodex64.dll "%ROOT%\for_plugins_folder\%FOLDER%\64"
copy .\msvc_64\msvcp110.dll "%ROOT%\for_plugins_folder\%FOLDER%\64"
copy .\msvc_64\msvcr110.dll "%ROOT%\for_plugins_folder\%FOLDER%\64"

REM 32bit version
copy "%XPlaneDir%\Resources\Plugins\%FOLDER%\32\win.xpl" "%ROOT%\for_plugins_folder\%FOLDER%\32"
copy ..\src\FMOD\FMODWindows\api\fmodex.dll "%ROOT%\for_plugins_folder\%FOLDER%\32"
copy .\msvc_32\msvcp110.dll "%ROOT%\for_plugins_folder\%FOLDER%\32"
copy .\msvc_32\msvcr110.dll "%ROOT%\for_plugins_folder\%FOLDER%\32"
copy .\TSRemote.dll "%ROOT%\for_plugins_folder\%FOLDER%\32"


mkdir "%ROOT%\for_plugins_folder\%FOLDER%\%RESFOLDER%\"
xcopy /E "X-IvAp Resources" "%ROOT%\for_plugins_folder\%FOLDER%\%RESFOLDER%\"

REM Compress
del /s /q xivap_*.zip
7za.exe a xivap_%VERSION%.zip %ROOT%
del /s /q %ROOT%
