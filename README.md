# Web Server and Client in C/C++

## The HTTP Client  

_Execution_: Client takes command line arguments and the GET method to
download a web-page. The arguments are as follows:
    • The **server URL** (http://www.server.com/path/to/file) or an IP address
    • The **port number** on which to contact the server
    • The **options** that which add functionality to the client
        - `–p`: prints the RTT for accessing the URL on the terminal before server’s response (i.e., the webpage)
For example
`./http_client [-options] server_url port_number`

## The HTTP Server

_Execution_: Your server should start first (i.e., before the client) and take a command line
argument specifying a port number. For example,
  `./http_server 7890`
  
