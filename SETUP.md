Use compile_env.bat to navigate compilation process. Ensure regluar cleaning to make sure everything is updated upon use. 

Use compile_env.bat to navigate compilation process. Ensure regluar cleaning to make sure everything is updated upon use. 

## Log File Extraction
1. Download the logs.zip file from the project repository or shared location
2. Extract the contents using any archive tool:
   ```
   Right-click logs.zip > Extract All... 
   ```
   Or using PowerShell:
   ```powershell
   Expand-Archive -Path logs.zip -DestinationPath .\Logs
   ```
3. Ensure the logs are extracted to the "Logs" folder in the project root directory
4. Verify the folder structure matches what's referenced in the commands (e.g., "Logs\logs\client#1\")


Then use commands that direct to logfiles.

**Sample Client Commands

## Basic Parsing Commands

# Parse JSON log file
.\simple_client.exe --parse --file "Logs\logs\client#1\log_file.json" --type json

# Parse TXT log file
.\simple_client.exe --parse --file "Logs\logs\client#1\log_file.txt" --type txt

# Parse logs from different clients
.\simple_client.exe --parse --file "Logs\logs\client#2\log_file.json" --type json
.\simple_client.exe --parse --file "Logs\logs\client#3\log_file.txt" --type txt


## Analysis Commands (When Implemented)

# Analyze logs by user activity
.\simple_client.exe --analysis --log-folder "Logs\logs" --type user

# Analyze logs by IP address
.\simple_client.exe --analysis --log-folder "Logs\logs" --type ip

# Analyze logs by severity level
.\simple_client.exe --analysis --log-folder "Logs\logs" --type level


## Advanced Usage

# Analyze specific client's logs
.\simple_client.exe --analysis --log-folder "Logs\logs\client#1" --type user

# Parse custom log file
.\simple_client.exe --parse --file "path\to\your\custom_log.json" --type json

# Run with output redirection to save results
.\simple_client.exe --parse --file "Logs\logs\client#1\log_file.json" --type json > results.txt