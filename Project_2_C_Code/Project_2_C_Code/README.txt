Notes:
For the Round-trip time (RTT) document. I commented out the printing of the response to the client
so that is would be easier for me to find the RTT. However, I uncommented it back as I know that
is how the workflow should be.

TO COMPILE THE CODE:

There is a Makefile in the directory that compiles the programs for you when you call "make" or "make all". 
There is also a "make clean" command to recompile if any errors should occur.

TO RUN THE CLIENT:

Once you have executed "make" or "make all", executables will be produced. These will be used to run the client and server.
To run the client do: "./client [-options] server_url port_number" and the program will run.
The client will get back the HTTP response once the command has been executed.
The only options available for -options are -p, which prints the Round-trip Time (in milliseconds) of the request that was made.

TO RUN THE SERVER:

Once "make" or "make all" has been executed, then you can run the server program using "./server port_number".
This should put the server to listen for a connection.
Once a connection has been found, then it prints the IP address of the client, followed by the HTTP response.
