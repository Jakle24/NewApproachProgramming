# NewApproachProgramming

# Project Setup

## Prerequisites
- Ensure you have `LogFiles.zip` downloaded and placed in the root directory of the project.
- You can obtain it from [link to your shared file].

## How to Setup the Logs

1. Download the `LogFiles.zip` from the link provided.
2. Place it in the root directory of the project (where the `README.md` file is located).
3. Run the `extract_logs.sh` (for Linux/Mac) or `extract_logs.bat` (for Windows) script to unzip the logs automatically.

### Automatically Extract Logs (Optional)
To extract the logs after placing the `LogFiles.zip` file:
- **Linux/Mac:**  
  Run the following command in the terminal:
  
  ```bash
  ./extract_logs.sh


The above was a lesson learned in adding many large files into a Repository...it wont allow commits and syncing! The work around, currently, is to make a tool that will follow the given criteria:

## Project Brief

## Overview
This project is designed to process and analyze large log files using a multi-threaded C++ server-client architecture. The server receives data, calculates statistics (average, minimum, maximum, median), and sends the results back to the client. The client sends the log data to the server for processing.

## Key Components
TCPServer: A multi-threaded server that listens for incoming connections, processes the received log data, calculates statistics, and returns the results.

TCPClient: A client that connects to the server, sends log data, and receives statistical results.

LogProcessor: Handles reading, processing, and extracting relevant information from large log files efficiently.

Log File Handling: The project utilizes a zipped file of log data (LogFiles.zip), which is extracted automatically using a script for easier access.

## Features
Multi-threaded server and client for concurrent data processing.

Statistics calculation: average, minimum, maximum, and median for a list of numbers.

Easy log extraction and setup with automated scripts.




## CMP9133M - Advanced Programming - Assessment Item 2 Checklist
## Learning Outcome 2: Propose and Implement Solutions for a Large-Scale Programming Problem (60%)

Design and Implement the Application:

✅ Log File Loading & Parsing:

The application loads log files in different formats.

The application parses the log files correctly.

The required log file analyses are applied to the loaded files.

✅ Client-Server Paradigm:

The solution is built around the client-server paradigm.

✅ Output on Client Side:

The application outputs the analysis results on the client side.

✅ Multi-threading/Processing:

Multi-threading/processing is integrated for performance improvement.

Functionality in Demo:
✅ Demonstration of Functionality:

At least two required functionalities are working well in the demo.

The functionality is clearly described in the report.

✅ Log File Parsing:

At least one log file format is parsed.

✅ Analyses Applied:

At least two specified analyses are applied to the parsed log file.

Code Quality:
✅ Clear Structure and Comments:

The program code is structured clearly with adequate comments.

✅ Error-Free Design:

There are no major errors or conceptual mistakes in the design.

Advanced Merit:
✅ Additional Functionality:

At least three functionalities are working well in the demo.

✅ Log File Formats:

At least two log file formats are handled.

✅ Analyses Applied:

At least three analyses are applied to the log files.

Distinction Criteria:
✅ Full Functionality:

All four required functionalities are working very well in the demo.

✅ Log File Parsing:

All three log file formats are parsed.

✅ Analyses Applied:

At least three analyses are applied to the log files.

✅ Date Range Option:

The client optionally takes a date range for performing analyses.

Code Maintenance and Extensibility:
✅ Code Quality:

The code is clearly written with appropriate comments.

✅ Maintainability and Extensibility:

Code maintainability and extensibility have been considered.

Learning Outcome 3: Critically Evaluate Different Solutions for Large-Scale Programming Problems (40%)
Written Report:
✅ Solution Description:

The report is structured to describe solutions to the tasks presented.

✅ Briefing Document Addressed:

The specific aspects outlined in the briefing document are addressed.

✅ Design Diagrams & Class Descriptions:

The report includes appropriate design diagrams and class descriptions.

✅ Problem Solving:

The report demonstrates a clear understanding of the relevant concepts and skills.

Report Quality:
✅ Professional Writing:

The report is professionally written and well-worded.

✅ Design Details:

The report provides an appropriate level of detail about the design choices.

✅ Use of Diagrams:

Diagrams are carefully presented and used effectively.

✅ Problem Solutions:

Solutions to problems are well explained.

Learning Outcome 3: Recorded Demonstration
✅ Video Submission:

The recorded video is submitted with the application running.

✅ Student's Camera & Desktop:

The video shows the student’s camera and desktop during the demonstration.

✅ Design Explanation:

The student describes the design choices and the working principle of the application.

✅ Final Output:

The final output generated by the application is shown in the video.

✅ Fulfillment of Requirements:

All the requirements for the recorded video are met.

Final Checklist:
✅ Ensure that all required functionalities are demonstrated and described.

✅ Double-check that the report includes sufficient diagrams, class descriptions, and explanations.

✅ Confirm that the video meets all requirements, including the application demonstration and explanations.

✅ Test the application thoroughly to make sure all log file formats are parsed and the analyses are correctly applied.

✅ Review the code for maintainability and clarity.





🛠️ Setup Instructions
1. Download & Prepare Log Files
Place your LogFiles.zip in the root of the project directory (next to extract_logs.bat).

Run extract_logs.bat (or just run the program — it will auto-extract if needed).

2. Build the Project (Windows – Manual Compile)
If you’re using the Visual Studio Developer Command Prompt:

cmd
Copy
Edit
cl /EHsc /std:c++17 src\*.cpp ws2_32.lib /I.\nlohmann /Fe:NewApproachProgramming.exe
This will generate NewApproachProgramming.exe.

3. Run the App
You can now launch the program in one of three modes:

bash
Copy
Edit
NewApproachProgramming.exe server     # Start the server
NewApproachProgramming.exe client     # Start the client and send requests
NewApproachProgramming.exe process    # Run log analysis locally (no network)
