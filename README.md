# Simple Talk

This is a simple UDP-based chat app utilizing a client/server model with socket.h and pthreads. The app enables real-time communication between two users. 

Leveraging pthreads, each simple talk process runs four threads:
1. Keyboard thread to write a message
2. Sender thread to send a message to the other user's receiver thread
3. Receiver thread to receive a message from the other user's sender thread
4. Printer thread to read a message

## Setup
To initiate a session, two users must know two things:
1. The IP address each will use
2. The port number each will use

User 1:
```
./simple-talk <User 1's Port #> <User 2's IP address> <User 2's Port #>
```

User 2:
```
./simple-talk <User 2's Port #> <User 1's IP address> <User 1's Port #>
```