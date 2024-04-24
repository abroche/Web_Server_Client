# Web Server and Client in C/C++

### To compile the code:

There is a Makefile in the directory that compiles the programs for you when you call `make` or `make all`. 
There is also a `make clean` command to remove the executables if any errors should occur. Run `make` or `make all` again after cleaning.

### To run the client:

Once you have executed `make` or `make all`, executables will be produced. These will be used to run the client and server.
To run the client execute: `./client [-options] server_url port_number` and the program will run.

The client will get back the HTTP response once the command has been executed.
The only options available for `-options` are `-p`, which prints the Round-trip Time (in milliseconds) of the request that was made.

### To run the server:

Once `make` or `make all` has been executed, then you can run the server program using `./server port_number`.
This should put the server to listen for a connection.
Once a connection has been found, then it prints the IP address of the client, followed by the HTTP response.


## Specific information on execution of the client and server

### The HTTP Client  

_Execution_: Client takes command line arguments and the GET method to download a web-page. The arguments are as follows:

- The **server URL** (http://www.server.com/path/to/file) or an IP address
- The **port number** on which to contact the server
- The **options** that which add functionality to the client
-    `–p`: prints the RTT for accessing the URL on the terminal before server’s response (i.e., the webpage)
For example
`./http_client [-options] server_url port_number`

### The HTTP Server

_Execution_: Your server should start first (i.e., before the client) and take a command line
argument specifying a port number. For example,
  `./http_server 7890`
  
# 
  
