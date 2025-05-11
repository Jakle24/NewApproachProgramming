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