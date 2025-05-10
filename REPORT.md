# Project Report: NewApproachProgramming

## Overview
This report documents the implementation and testing process for the NewApproachProgramming project. The goal is to meet the requirements outlined in the README file.

---

## Implementation Steps

### 1. Log Extraction Scripts
#### Reasoning
To simplify the setup process, we implemented scripts (`extract_logs.bat` and `extract_logs.sh`) to extract the `LogFiles.zip` file automatically.

#### Implementation
- **Windows Script**: `extract_logs.bat`
- **Linux/Mac Script**: `extract_logs.sh`

#### Testing
- [ ] Verify the scripts extract `LogFiles.zip` to the `LogFiles` directory.

---

### 2. Statistics Calculation
#### Reasoning
The project requires calculating average, minimum, maximum, and median values from the log data.

#### Implementation
- [ ] Ensure the `LogProcessor` calculates these statistics.

#### Testing
- [ ] Verify the statistics are calculated correctly for sample log files.

---

### 3. Multi-Threading
#### Reasoning
The server must support multi-threading for concurrent data processing.

#### Implementation
- [ ] Review and implement multi-threading in the `TCPServer`.

#### Testing
- [ ] Verify the server handles multiple client connections simultaneously.

---

### 4. Optional Features
#### Reasoning
To enhance functionality, we added support for multiple log file formats and date range filtering.

#### Implementation
- [ ] Add support for `.txt` and `.csv` log files.
- [ ] Implement date range filtering in the `LogProcessor`.

#### Testing
- [ ] Verify the optional features work as expected.

---

## Testing Results
- [ ] Log extraction scripts tested successfully.
- [ ] Statistics calculation verified.
- [ ] Multi-threading tested with multiple clients.
- [ ] Optional features tested.





# Project Report: NewApproachProgramming

## Overview
This report documents the implementation and testing process for the NewApproachProgramming project. The goal is to meet the requirements outlined in the README file.

---

## Troubleshooting and Getting the Project Running

During development, I encountered several issues getting the TCP client and server to compile and run, especially when working without CMake. Here’s a summary of the troubleshooting steps and solutions, written in a conversational tone for clarity:

### CMake Not Recognized

At first, I tried to use CMake to build the project, but the terminal kept saying `'cmake' is not recognized as the name of a cmdlet, function, script file, or operable program.` This usually means CMake isn’t installed or isn’t on the system PATH. The fix is to download and install CMake from the official website, making sure to select the option to add CMake to the system PATH during installation. After installing, it’s important to restart the terminal so the new PATH is recognized.

### Building Without CMake

Since CMake wasn’t working right away, I decided to build the project using the Visual Studio Developer Command Prompt. This environment sets up all the necessary paths for compiling C++ code on Windows. I navigated to the project directory and used the following command to compile:

```cmd
cl /EHsc /std:c++17 TCPServer.cpp TCPClient.cpp main.cpp ws2_32.lib
```

If you don’t specify an output name, the compiler will name the executable after your main source file (e.g., `main.cpp` produces `main.exe`). To control the output name, add `/Fe:YourProgramName.exe` to the command.

### Fixing Compilation Errors

I ran into a lot of errors related to missing files and undeclared identifiers, especially from ZIP and filesystem code in `main.cpp`. To get things working, I temporarily replaced `main.cpp` with a minimal version that only starts the TCP server or client. This allowed me to focus on testing the networking features without worrying about missing libraries.

### Running the Program

After compiling, I noticed that the executable was named after the main source file (e.g., `maintesting.exe`). To run the program, I simply typed:

```cmd
maintesting.exe
```

or, if I specified the output name:

```cmd
NewApproachProgramming.exe
```

The program then prompted me to choose between starting the server or client. I was able to start the server in one terminal and the client in another, and successfully send messages between them.

### Key Takeaways

- Make sure CMake is installed and on your PATH if you want to use it.
- The Visual Studio Developer Command Prompt is essential for compiling C++ code on Windows.
- If you run into missing library errors, simplify your `main.cpp` to isolate and test core features.
- The output executable will be named after your main source file unless you specify otherwise.
- Always check for and resolve compilation errors before trying to run the executable.

---

## Implementation Steps

### 1. Log Extraction Scripts
#### Reasoning
To simplify the setup process, we implemented scripts (`extract_logs.bat` and `extract_logs.sh`) to extract the `LogFiles.zip` file automatically.

#### Implementation
- **Windows Script**: `extract_logs.bat`
- **Linux/Mac Script**: `extract_logs.sh`

#### Testing
- [ ] Verify the scripts extract `LogFiles.zip` to the `LogFiles` directory.

---

### 2. Statistics Calculation
#### Reasoning
The project requires calculating average, minimum, maximum, and median values from the log data.

#### Implementation
- [ ] Ensure the `LogProcessor` calculates these statistics.

#### Testing
- [ ] Verify the statistics are calculated correctly for sample log files.

---

### 3. Multi-Threading
#### Reasoning
The server must support multi-threading for concurrent data processing.

#### Implementation
- [ ] Review and implement multi-threading in the `TCPServer`.

#### Testing
- [ ] Verify the server handles multiple client connections simultaneously.

---

### 4. Optional Features
#### Reasoning
To enhance functionality, we added support for multiple log file formats and date range filtering.

#### Implementation
- [ ] Add support for `.txt` and `.csv` log files.
- [ ] Implement date range filtering in the `LogProcessor`.

#### Testing
- [ ] Verify the optional features work as expected.

---

## Testing Results
- [ ] Log extraction scripts tested successfully.
- [ ] Statistics calculation verified.
- [ ] Multi-threading tested with multiple clients.
- [ ] Optional features tested.