@echo off
REM filepath: c:\Users\ethan\OneDrive\Documents\GitHub\NewApproachProgramming\extract_logs.bat
echo Extracting log files...

REM Check if LogFiles.zip exists
if not exist LogFiles.zip (
    echo Error: LogFiles.zip not found!
    echo Please download LogFiles.zip and place it in the project root directory.
    exit /b 1
)

REM Create LogFiles directory if it doesn't exist
if not exist LogFiles mkdir LogFiles

REM Use PowerShell to extract the ZIP file
powershell -command "Expand-Archive -Path 'LogFiles.zip' -DestinationPath 'LogFiles' -Force"

echo Log files extracted successfully to LogFiles directory.
echo.
echo You can now run the log analyzer.