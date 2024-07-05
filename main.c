#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    char buffer[BUFFER_SIZE];
    char resp[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hello, world</html>\r\n";

    // (int domain, int type, int protocol)
    int s = socket(AF_INET, SOCK_STREAM, 0); 
    if (s == -1) {
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully");

    //Create the address to bind the socket to
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    // The htons() function makes sure that numbers are stored in memory in network byte order, which is with the most significant byte first.
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Create client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    if(bind(s, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
    }
    printf("socket sucessfully bound to address\n");

    if(listen(s, SOMAXCONN) != 0) {
        perror("webserver (listen)");
    }
    printf("server listening for connections\n");

    for(;;) {
        //Accept incoming connections
        int new_s = accept(s, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);

        if(new_s < 0){
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        //Get client address
        int sockn = getsockname(new_s, (struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);

        if(sockn < 0) {
            perror("webserver (getsocketname)");
            continue;
        }
        // Read the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);

        //Print the request
        printf("[%s:%u]\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        //Read from the socket
        int valread = read(new_s, buffer, BUFFER_SIZE);
        if(valread < 0) {
            perror("webserver (read)");
            continue;
        }

        int valwrite = write(new_s, resp, strlen(resp));
        if(valwrite < 0) {
            perror("webserver (write)");
        }

        close(new_s);
    }


    return 0;
}

