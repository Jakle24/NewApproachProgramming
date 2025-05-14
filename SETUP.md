Use compile_env.bat to navigate compilation process. Ensure regluar cleaning to make sure everything is updated upon use. 
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