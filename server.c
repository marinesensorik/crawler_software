/*
    C Crawler Server Example 
	Version 2016_01_15
*/


// Includes
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <stdio.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyS1" // Serieller Port -> COM
#define _POSIX_SOURCE 1         //POSIX compliant source
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

void signal_handler_IO (int status);    //definition of signal handler
int wait_flag=TRUE;                     //TRUE while no signal received
char devicename[80];
//long Baud_Rate = 115200;         // default Baud Rate (110 through 38400)
//long BAUD;                      // derived baud rate from command line
long DATABITS;
long STOPBITS;
long PARITYON = 0;
long PARITY;
int Data_Bits = 8;              // Number of data bits
int Stop_Bits = 1;              // Number of stop bits
int Parity = 0;                 // Parity as follows:
                  // 00 = NONE, 01 = Odd, 02 = Even, 03 = Mark, 04 = Space
int Format = 4;
FILE *input;
FILE *output;
int status;

//

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#define telemetryHeader 0xF0 // Proof the data, if its complete



int main(int argc , char *argv[])
{

    // Open Serial Port
    int fd, tty, res, i, error;
   char In1, Key;
   struct termios oldtio, newtio;       //place for old and new port settings for serial port
   struct termios oldkey, newkey;       //place tor old and new port settings for keyboard teletype
   struct sigaction saio;               //definition of signal action
   char buf[255];                       //buffer for where data is put
 char*  devicename = "/dev/ttyO0";			// UART0



      //open the device(com port) to be non-blocking (read will return immediately)
      fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);
      if (fd < 0)
      {
         perror(devicename);
         exit(-1);
      }



    // Build Server
    int socket_desc , client_sock  , read_size, c;
    struct sockaddr_in server , client;
    char client_message[2000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(client_sock , client_message , strlen(client_message));

        uint8_t Header = client_message[0];

        printf("Message sent: %i \n", Header);

        switch(Header) // Proof imcoming data
        {
            case telemetryHeader:
            { // Forwarded with right startbyte
                uint8_t crawler_move = client_message[1]; // Movement of the Crawler
                uint8_t crawler_velocity = client_message[2]; // Velocity of the Crawler
                puts("Right Startbyte");

                // Startbyte
                Key = Header;				
                write(fd,&Key,1);          //write 1 byte to the port

                // Movement
                Key = crawler_move;					
                write(fd,&Key,1);          //write 1 byte to the port

                // Velocity
                Key = crawler_velocity;					
                write(fd,&Key,1);          //write 1 byte to the port


                break;

            }
            default:
                puts("Wrong Startbyte");
                break;

           }





    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

     close(fd);        //close the com port
    return 0;
}
