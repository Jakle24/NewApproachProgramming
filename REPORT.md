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

