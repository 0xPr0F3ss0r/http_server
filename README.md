#@Project Overview

This project is a simple HTTP server written from scratch that handles requests coming from clients such as web browsers (e.g., Google Chrome, Firefox) or command-line tools like curl.

The server hosts a small portfolio-style website (content partially generated with AI). When the server program is started, it creates a dedicated thread and begins listening for incoming connections on a specified port.

When a client accesses the server—by visiting http://localhost:8080 in a browser or sending an HTTP request via curl—the server:

1. Accepts the incoming connection using the socket accept() function.

2. Reads and parses the HTTP request sent by the client.

3. Determines the requested resource (e.g., an HTML file).

4. Generates the appropriate HTTP response, including headers and content.

5. Sends the response back to the client.

If the client requests additional resources, the same process is repeated. Once all requests from a client are handled, the server closes the socket associated with that client while continuing to listen for new incoming connections.

This project demonstrates the fundamentals of networking, multithreading, and HTTP protocol handling at a low level.this project is an http server that handle request comming from the client which can be any brwoser , I made website like small portfolio and (it's generally with ai) , when server start by run the 
server program the server make thread and start listing for any incomming connection , and when the browser like google chrome for example , when the clientmake curl or type http://localhost in search ,
the server accept that connection with socket accept function and start read any request from the client like the client request for the html file and then the server generate the file with header 
and send it ,and then if the client want other file , the server follow the same process, and when all requests end ,the server close the socket of this client ,but in the same will keep listining to 
any incommig connection
