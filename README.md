# Agoric-system-performance-analysis-tool
Analyzes resource usage of a system

This program is developed in C++(QT Creator IDE). QT allows you to easily port the program between Windows and Linux.

Algorithm of the program:

After launch, the user enters the IP, login, password from his server, as well as the chat ID and bot ID for telegram, from which he wants to receive notifications. The program connects to the server via SSH.
Then the program runs the following commands every 30 seconds:

1) curl -s 127.0.0.1:26657/net_info  | grep n_peers       #return active peers

2) ag-cosmos-helper status       #checks last block time

3) cat /proc/loadavg             #cpu avarage usage on different intervals 
        
4) cat /proc/meminfo             #memory usage

and parses the result of their execution. In case of critical values, a notification is sent by telegram.

Thus, the validator can use this utility as an additional monitoring tool in conjunction with prometheus + grafana. Or as the main simple way to monitor the state of the system and receive prompt notifications.
To run the ready build fo Windows x64, unzip win_x64.rar

More updates soon. 
