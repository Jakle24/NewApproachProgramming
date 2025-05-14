@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cd /d C:\Users\ethan\OneDrive\Documents\GitHub\NewApproachProgramming
echo Environment ready for compilation!

:menu
echo.
echo ===== Log Analysis System Compilation Menu =====
echo.
echo 1. Compile test_parse (json/txt parser)
echo 2. Compile simple_server
echo 3. Compile simple_client
echo 4. Compile everything
echo 5. Clean up executable files
echo 6. Exit
echo.

set /p choice=Enter your choice (1-6): 

if "%choice%"=="1" goto compile_test_parse
if "%choice%"=="2" goto compile_server
if "%choice%"=="3" goto compile_client
if "%choice%"=="4" goto compile_all
if "%choice%"=="5" goto clean
if "%choice%"=="6" goto end

echo Invalid choice. Please try again.
goto menu

:compile_test_parse
echo.
echo === Compiling test_parse.exe ===
cl /EHsc /std:c++17 test_parse.cpp /I"include" /Fe:test_parse.exe
if %errorlevel% equ 0 (
    echo test_parse.exe compiled successfully.
) else (
    echo Error compiling test_parse.exe.
)
goto menu

:compile_server
echo.
echo === Compiling simple_server.exe ===
taskkill /F /IM simple_server.exe 2>nul
cl /EHsc /std:c++17 simple_server.cpp /I"include" ws2_32.lib /Fe:simple_server.exe
if %errorlevel% equ 0 (
    echo simple_server.exe compiled successfully.
) else (
    echo Error compiling simple_server.exe.
)
goto menu

:compile_client
echo.
echo === Compiling simple_client.exe ===
taskkill /F /IM simple_client.exe 2>nul
cl /EHsc /std:c++17 simple_client.cpp /I"include" ws2_32.lib /Fe:simple_client.exe
if %errorlevel% equ 0 (
    echo simple_client.exe compiled successfully.
) else (
    echo Error compiling simple_client.exe.
)
goto menu

:compile_all
echo.
echo === Compiling all components ===
echo.

echo 1/3: Compiling test_parse.exe...
cl /EHsc /std:c++17 test_parse.cpp /I"include" /Fe:test_parse.exe
if %errorlevel% neq 0 (
    echo Error compiling test_parse.exe.
    pause
    goto menu
)

echo 2/3: Compiling simple_server.exe...
taskkill /F /IM simple_server.exe 2>nul
cl /EHsc /std:c++17 simple_server.cpp /I"include" ws2_32.lib /Fe:simple_server.exe
if %errorlevel% neq 0 (
    echo Error compiling simple_server.exe.
    pause
    goto menu
)

echo 3/3: Compiling simple_client.exe...
taskkill /F /IM simple_client.exe 2>nul
cl /EHsc /std:c++17 simple_client.cpp /I"include" ws2_32.lib /Fe:simple_client.exe
if %errorlevel% neq 0 (
    echo Error compiling simple_client.exe.
    pause
    goto menu
)

echo.
echo All components compiled successfully!
echo.
pause
goto menu

:clean
echo.
echo === Cleaning up executable files ===
taskkill /F /IM test_parse.exe 2>nul
taskkill /F /IM simple_server.exe 2>nul
taskkill /F /IM simple_client.exe 2>nul
del *.exe 2>nul
del *.obj 2>nul
echo Cleanup complete.
goto menu

:end
echo Exiting...
exit /b