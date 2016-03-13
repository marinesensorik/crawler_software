/*
    C Client Test for Crawler
*/

// Includes
#include<stdio.h> 
#include<string.h>    
#include<sys/socket.h>    
#include<arpa/inet.h> 
#include<stdlib.h>
#include<unistd.h>

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("192.168.1.2");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    int i = 1;
    

    for (int u = 1;u<3;u++)
    {
        message[0] = 0xF0; // Right Startbyte, 0xF0->240
        message[1] = 1; // Movement of crawler -> 1:straight on
        message[2] = 250; // velocity of crawler -> 255: max.velocity
        message[3] = 101;   // Checksum




        //Send the data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }
		// Answer of server
        puts("Server reply :");
        puts(server_reply);
        
    }
	   
    double timer = 5000000000; // time, after the client will send again

    while(timer > 0)
    {
        timer--;
    }

        for (int t = 1;t<3;t++)
        {
        message[0] = 0xF0; // Startbyte, 0xF0->240
        message[1] = 16; // Movement
        message[2] = 50; // Velocity
        message[3] = 101;   // Checksum




        //Send the data again
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

    }

    close(sock);

    return 0;
}
