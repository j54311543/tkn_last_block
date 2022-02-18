//includes
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>

struct header{
        unsigned char Version:2; 
        unsigned char type:5; 
        unsigned char options:1;
        unsigned char SeqNo;
        //unsigned char DataLen [2];
        //unsigned int DataLen [2];
        uint16_t DataLen;
};

struct data{
        unsigned char *payload;
};

struct Packet{
        struct header head;
        struct data dat;
};


//functions
//int encode ((packet*)&packet, char buffer[]);
int encode(struct Packet packet, char buffer[]);
struct Packet packet_creator(char *buffer_read, unsigned int Version, unsigned int type, unsigned int options, unsigned int SeqNo, size_t characters);