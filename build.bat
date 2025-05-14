@echo off
rem compile all .cpp under src\, include headers from src\ and link ws2_32
cl /EHsc /std:c++17 src\*.cpp /I src /link ws2_32.lib
if exist *.exe (
  echo Build succeeded.
) else (
  echo Build failed.
)