:: filepath: c:\Users\Student\Documents\Module Dev Containers\NewApproachProgramming\extract_logs.bat
@echo off
set ZIP_FILE=LogFiles.zip
set OUTPUT_DIR=LogFiles

if not exist "%ZIP_FILE%" (
    echo ERROR: %ZIP_FILE% not found in the current directory.
    exit /b 1
)

ZipExtractor.exe "%ZIP_FILE%" "%OUTPUT_DIR%"
if %errorlevel% neq 0 (
    echo ERROR: Failed to extract %ZIP_FILE%.
    exit /b 1
)

echo Extraction complete. Logs are in the %OUTPUT_DIR% directory.