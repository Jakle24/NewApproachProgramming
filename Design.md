# System Design

## Architecture Overview
Client-Server architecture for distributed log processing

## Class Diagram

+----------------+ +----------------+ | TCPClient |------>| TCPServer | +----------------+ +----------------+ | - server_ip | | - port | | - server_port | | - running | +----------------+ +----------------+ | + send_request | | + start() | | + receive_resp | | + handle_client| +----------------+ +----------------+ | | | v | +----------------+ +--------------->| LogProcessor | +----------------+ | - log_folder | +----------------+ | + analyze_by_* | | + parse_* | | + load_logs | +----------------+


## Data Flow
1. Client sends request with log folder and analysis type
2. Server processes request using LogProcessor
3. LogProcessor reads and parses logs from specified location
4. Analysis results are returned to client
5. Client displays results to user

