Log Analysis System
A multi-threaded C++ server-client application for processing and analyzing log files.

Setup Instructions


Prerequisites

Windows OS
Visual Studio C++ Build Tools (MSVC compiler)
LogFiles.zip containing your log data

Quick Start
1. Clone the repository

git clone https://github.com/yourusername/NewApproachProgramming.git
cd NewApproachProgramming

2. Place log files

Download LogFiles.zip
Place it in the project root directory
Extract using the included script:

extract_logs.bat

3. Build the application

Open a Developer Command Prompt for Visual Studio
Run:

cl /EHsc /std:c++17 src\*.cpp ws2_32.lib /I. /Fe:NewApproachProgramming.exe

4. Run the server

NewApproachProgramming.exe server

5. Run the client (in a separate terminal)

NewApproachProgramming.exe client --log-folder LogFiles --analysis user


Available Analysis Types

user - Analyze logs by username
ip - Analyze logs by IP address
level - Analyze logs by severity level


Command Examples

# User analysis
NewApproachProgramming.exe client --log-folder LogFiles --analysis user

# IP analysis
NewApproachProgramming.exe client --log-folder LogFiles --analysis ip

# Log level analysis
NewApproachProgramming.exe client --log-folder LogFiles --analysis level

# With date filtering
NewApproachProgramming.exe client --log-folder LogFiles --analysis user --start "2023-01-01 00:00:00" --end "2023-12-31 23:59:59"



Project Structure
src - Source code files
LogFiles - Extracted log files for analysis
nlohmann - JSON library for C++
extract_logs.bat - Script to extract log files from zip


Troubleshooting
Ensure you're using the Visual Studio Developer Command Prompt when building
Verify that LogFiles.zip has been extracted correctly
Start the server before running any client commands





QUICK USE

Client Mode
The client mode is used for analyzing log files with several required and optional parameters:

Basic Usage

.\NewApproachProgramming.exe client --log-folder "C:\Logs" --analysis user


Analysis Types
You can specify one of three analysis types:

# For user-based analysis
.\NewApproachProgramming.exe client --log-folder "C:\Logs" --analysis user

# For IP-based analysis
.\NewApproachProgramming.exe client --log-folder "C:\Logs" --analysis ip

# For level-based analysis
.\NewApproachProgramming.exe client --log-folder "C:\Logs" --analysis level

Date Filtering
To analyze logs within a specific date range:

.\NewApproachProgramming.exe client --log-folder "C:\Logs" --analysis user --start "2025-05-01 00:00:00" --end "2025-05-14 23:59:59"


Tips
Use quotes around folder paths that contain spaces
Date format must be 'YYYY-MM-DD HH:MM:SS'
You can use just the start date without the end date (or vice versa)
Make sure the log folder path points to a directory containing valid log files