@echo off

REM --- kill any running server so we can overwrite the exe
taskkill /IM NewApproachProgramming.exe /F >nul 2>&1

REM --- delete any existing exe
if exist NewApproachProgramming.exe del /q NewApproachProgramming.exe

REM --- now compile…
where cl >nul 2>&1
if %ERRORLEVEL%==0 (
  echo [build] Using MSVC (cl)...
  cl /EHsc /std:c++17 /I src src\*.cpp /FeNewApproachProgramming.exe /link ws2_32.lib
  if %ERRORLEVEL% EQU 0 ( echo [build] MSVC build succeeded. & exit /b 0 ) 
  else ( echo [build] MSVC build failed. & exit /b 1 )
)

where g++ >nul 2>&1
if %ERRORLEVEL%==0 (
  echo [build] Using MinGW (g++)...
  g++ -std=c++17 src\*.cpp -I src -o NewApproachProgramming.exe -lws2_32 -pthread
  if %ERRORLEVEL% EQU 0 ( echo [build] MinGW build succeeded. & exit /b 0 )
  else ( echo [build] MinGW build failed. & exit /b 1 )
)

echo ERROR: Neither cl.exe nor g++ found on PATH.
exit /b 1