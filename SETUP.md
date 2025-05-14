Simple Guide: Using Your Log Parser

1. Compile the Test Parser

cd C:\Users\ethan\OneDrive\Documents\GitHub\NewApproachProgramming

cl /EHsc /std:c++17 test_parser.cpp src\LogEntry.cpp /I"include" /Fe:test_parser.exe

**SERVER SETUP INSTRUCTIONS**

Server: .\simple_server.exe
You should see: "Server started on port 8080"

Run the Client (in a separate command window)
For user analysis:
Client: simple_client.exe --log-folder "Logs\logs" --analysis user


2. Parse JSON Files

.\test_parser.exe "Logs\logs\client#1\log_file.json" json

This will display the parsed contents of the JSON file.

3. Parse TXT Files 

.\test_parser.exe "Logs\logs\client#2\log_file.txt" txt

4. Parse XML Files 

.\test_parser.exe "Logs\logs\client#3\log_file.xml" xml

5. Test with Different Clients
Try parsing logs from different client folders:

.\test_parser.exe "Logs\logs\client#1\log_file.json" json
.\test_parser.exe "Logs\logs\client#2\log_file.json" json
.\test_parser.exe "Logs\logs\client#3\log_file.json" json

6. View All Available Log Files
To see what log files are available:

Get-ChildItem -Path "Logs\logs" -Recurse -File | Where-Object {$_.Extension -match "json|txt|xml"}


This will help you identify all the log files you can parse.


"Redundant" files are my old versions which I have kept in here to show what I was working on. Sadly I ran into so many issues and had to somewhat start again. I wanted to keep the code in here to show was I was attempting to do to match the brief. I know I was close with getting it working. It ended up having connection issues between all the files in the src folder. Files outside, with some exceptions, were used in my final version. 