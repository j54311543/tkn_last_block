#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include "tcpclient.h"
//#define PORT 3850;

/*
//encode packet 
int encode((struct Packet *)&packet, char buffer[]){  
  buffer[0]= packet->header->Version << 5  | packet->header->type << 1 | packet->header->options;
  buffer[1]= packet->header->SeqNo;
  uint16_t u16 = htons(packet->header->DataLen);
  memcpy(buffer+2, &u16, 2);
  return 0;
}*/

//alternative encode 
int encode(struct Packet packet, char buffer[]){  
  buffer[0]= (((packet.head.Version << 5)  | packet.head.type) << 1) | packet.head.options;
  buffer[1]= packet.head.SeqNo;
  uint16_t u16 = htons(packet.head.DataLen);
  memcpy(buffer+2, &u16, 2);
  return 0;
}

//create packet 
struct Packet packet_creator(char *buffer_read, unsigned int Version, unsigned int type, unsigned int options, unsigned int SeqNo, size_t characters){
  struct Packet packet_send;
  //fill header
  packet_send.head.Version = Version;
  packet_send.head.type = type;
  packet_send.head.options = options;
  packet_send.head.SeqNo = SeqNo;
  //alternatives for DataLen if using arrays instead of 16bit int 
  /*packet_send.head.DataLen[0] = (characters >> 8) & 0xFF;
  packet_send.head.DataLen[1]= characters & 0xFF;*/
  /*memcpy(packet_send.head.DataLen[0],characters,1);
  memcpy(packet_send.head.DataLen[1],characters)*/
  //used option for filling in DataLen
  packet_send.head.DataLen = characters;
  //fill data
  packet_send.dat.payload = (char *)malloc(characters * sizeof(char));
  memcpy(packet_send.dat.payload, buffer_read, characters);
  return packet_send;
}


uint16_t port = 3850;


int main(int argc, char const *argv[])
{
//Unicast TCP socket client 
  char buffer[1024] = {0};
  struct sockaddr_in socket_adresse;
  socket_adresse.sin_addr.s_addr =  inet_addr("127.0.0.1");
int udp = socket(AF_INET,SOCK_STREAM,0);
socket_adresse.sin_family = AF_INET;
socket_adresse.sin_port = htons(port);
//connect socket 
if (connect(udp, (struct sockaddr *)&socket_adresse, sizeof(socket_adresse)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }


//get line from keyboard 
char *buffer_read;
size_t bufsize = 32;
size_t characters;

buffer_read = (char *)malloc(bufsize * sizeof(char));
characters = getline(&buffer_read, &bufsize, stdin);

//make packet 
unsigned int Version = 2;
unsigned int Type = 4;
unsigned int Option = 0;
unsigned int SeqNo = 6;
struct Packet packet_send = packet_creator(buffer_read, Version, Type, Option, SeqNo, characters);
//for testing 
printf("Your Tyoe is: %u \n", packet_send.head.type);

//create bystream to be send 
int temp_bytestream; //just pseudo return 
unsigned buffer_len = characters + 4;
char buffer_bytestream [buffer_len];
temp_bytestream = encode(packet_send,buffer_bytestream);

//printing for testing 
//printf("first byte: %c \n", buffer_bytestream[0]);

//Multicast socket init
  int socket_multi;
  char ttl = 1;
  char loop = 0;
  char reuse = 1;
  struct ip_mreq mreq;
  socket_multi = socket(PF_INET,SOCK_DGRAM,0);  //returns fd of socket
  //fill serv_addr
  struct sockaddr_in serv_addr; 
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(3555);
  #define MC_GROUP_ADDR "224.45.2.1"
  //join multicast group
  mreq.imr_multiaddr.s_addr = inet_addr(MC_GROUP_ADDR); 
  mreq.imr_interface.s_addr = INADDR_ANY;
  setsockopt(socket_multi,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
  
  if(socket_multi < 0){
	  //perror(“open stream socket”);
	  //exit(1);
  }
  
  if (setsockopt(socket_multi, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0){
      perror("setsockopt IP_MULTICAST_TTL");
  }
  if (setsockopt(socket_multi, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0){
      perror("setsockopt IP_MULTICAST_LOOP");
  } 
  if (setsockopt(socket_multi, IPPROTO_IP, IP_MULTICAST_LOOP, &reuse, sizeof(reuse)) < 0){
      perror("setsockopt SO_REUSEADDR");
  } 
//connect 
if (connect(socket_multi, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
//sendto 
char *hello = "Hello from client";
if (sendto(socket_multi, hello, strlen(hello),0,(struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
    printf("\nsending failed \n");
}


//test connection 
//char *hello = "Hello from client";
send(udp , &buffer_bytestream , strlen(buffer_read) , 0 );
printf("Hello message sent\n");
printf("Buffer 1: %u \n",buffer_bytestream[1]);
/*while (buffer_bytestream[0]) {
    if (buffer_bytestream[0] & 1)
        printf("1");
    else
        printf("0");

    buffer_bytestream[0] >>= 1;
}*/
printf("\n");
int valread = read(udp , buffer, 1024);
printf("%s\n",buffer );
return 0;
}