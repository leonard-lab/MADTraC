@echo off

set header=include\%1.h
set pname=%1
set PNAMEU=%1

echo 
echo ----------------------------
echo Executing build phase script for %1

cd ..\..

echo Copying header files:
echo %pname%.h >> temp_exc.txt
echo pyhungarian.h >> temp_exc.txt
echo YASegmenter.h >> temp_exc.txt
echo SerialMFC.h >> temp_exc.txt
echo SerialEx.h >> temp_exc.txt
echo SerialWnd.h >> temp_exc.txt
xcopy MT\%1\*.h include\MT\%1\ /S /I /C /Y /EXCLUDE:temp_exc.txt > temp.txt
del temp_exc.txt

xcopy MT\MT.h include\ /S /I /C /Y

call :toUpper PNAMEU

del %header%

echo Generating %header%:

echo #ifndef MT_HAVE_%PNAMEU:~3,100% >> %header%
echo #define MT_HAVE_%PNAMEU:~3,100% >> %header%
echo. >> %header%

type MT\%1\%1.h >> %header%
echo. >> %header%
type misc\build_scripts\MSVC_defs.h >> %header%
echo. >> %header%

for /F "tokens=*" %%i in (temp.txt) do call :insertheader %%i
echo. >> %header%

echo #endif /* MT_HAVE_%PNAMEU:~3,100% */ >> %header%

del temp.txt

echo.
echo Done %pname% build phase script.
echo ----------------------------

goto :EOF

:insertheader
set t=%~1
if %t:~0,2%==MT (
echo #include "%t%" >> %header%
)
goto :EOF

:toUpper str -- converts lowercase character to uppercase
::           -- str [in,out] - valref of string variable to be converted
:$created 20060101 :$changed 20080219 :$categories StringManipulation
:$source http://www.dostips.com
if not defined %~1 EXIT /b
for %%a in ("a=A" "b=B" "c=C" "d=D" "e=E" "f=F" "g=G" "h=H" "i=I"
            "j=J" "k=K" "l=L" "m=M" "n=N" "o=O" "p=P" "q=Q" "r=R"
            "s=S" "t=T" "u=U" "v=V" "w=W" "x=X" "y=Y" "z=Z" "ä=Ä"
            "ö=Ö" "ü=Ü") do (
    call set %~1=%%%~1:%%~a%%
)
EXIT /b