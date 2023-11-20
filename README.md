# Programming Assignment 2 README

## Overview

This socket program is building on top of Assignment 1 for for requesting identification from server for access permission to the cellular network.

- The client programmer simulates a software module sending the request for identification of its device in a packet to the server. Total 11 requests are emulated and will be sent.

- The server verifies requests information against its database, and sends ok or reject response.

- The client then sends another five packets to the server, emulating one correct packet and four packets with errors.

- The server acknowledges with ACK receive of correct packet from client, and with corresponding Reject sub codes for packets with errors.

- The client retransmit the packet if the ACK for each packet has not been received before expiration of timer (3 seconds).

- If no ACK was received from the server after resending the same packet 3 times, the client will stop the process.

## Setup and Usage

1. **Server**
   ```
   cd ~/server
   cc -o server server.c
   ./server
   ```

2. **Client**
   ```
   cd ~/client
   cc -o client client.c
   ./client
   ```
## File Structure

- `client/`: Contains client's source code, and datapacket.txt that contains 10 packets the client will be sending.
- `server/`: Contains server's source code, and verification_database.txt that contains subscribers' information.
- `README.md`: This file.
- `client_err_msg.png`: simulated client error response messages displayed
- `server_err_msg.png`: simulated server error response messages displayed
