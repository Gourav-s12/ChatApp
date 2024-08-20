this need to be installed-
sudo apt-get install uuid-dev   # for Debian-based systems

to run server-
gcc -o server server.c -luuid
./server

to run client-
server must be running
gcc -o client client.c
./client


pdf said - The IP address and port number will be provided by Command Line Interface (if required).
it is not required for this code , so to run client no need to provide those

when chatbot is on , client would still recive messge from other client, as it is a design choice (not mentioned in pdf)
if you dont want it then , uncommend server.c line number 149 - 151