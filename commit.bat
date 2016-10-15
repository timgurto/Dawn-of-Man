@echo off
set project=Dawn of Man



set yyyy=%date:~6,4%
set mm=%date:~0,2%
set dd=%date:~3,2%
set hh=%time:~0,2%
set mn=%time:~3,2%
set ss=%time:~6,2%

if %hh% lss 10 (set hh=0%time:~1,1%)
set datetime=%yyyy%-%mm%-%dd%_%hh%.%mn%.%ss%

set path="C:\Rep\%project%\%datetime%\"

set /P change="Change made: "
set change=%datetime%  %change%
echo %change% >> "C:\Rep\%project%\changelog.txt"

%SystemRoot%\system32\xcopy *.dll %path% /V /I
%SystemRoot%\system32\xcopy *.h %path% /V /I
%SystemRoot%\system32\xcopy *.cpp %path% /V /I
%SystemRoot%\system32\xcopy *.fon %path% /V /I
%SystemRoot%\system32\xcopy *.vcproj %path% /V /I
%SystemRoot%\system32\xcopy *.sln %path% /V /I
%SystemRoot%\system32\xcopy Images\*.* %path%Images\ /V /I /S
@echo on