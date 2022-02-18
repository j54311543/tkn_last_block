#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "tcpserver.h"
//#define PORT 3850;
/*
//decoding the bytestream
int decode (char buffer[], struct Packet packet){
    packet.head.Version = (buffer[0] >> 6) & 0x3;
    packet.head.type = (buffer[0]>>1) & 0x1F;
    int TypeAlt = (buffer[0]<<2)>>3;
    packet.head.options = (buffer[0]) & 0x1;
    packet.head.SeqNo = buffer[1];
    uint16_t u16 = (buffer[2] << 8) | buffer[3];
    packet.head.SeqNo = ntohs(u16);
    return TypeAlt;
}*/
//alternative decoding
struct Packet decode(char buffer[]){
    struct Packet packet_recv;
    packet_recv.head.Version = (buffer[0] >> 6) & 0x3;
    //packet_recv.head.type = (buffer[0]>>1) & 0x1F;
    packet_recv.head.type = (buffer[0]<<2) >>3;
    printf("dont mind us, we are just printing here: %u",packet_recv.head.type);
    //int TypeAlt = (buffer[0]<<2)>>3;
    packet_recv.head.options = (buffer[0]) & 0x1;
    packet_recv.head.SeqNo = buffer[1];
    uint16_t u16 = (buffer[2] << 8) | buffer[3];
    packet_recv.head.DataLen = u16;
    return packet_recv;
}


int main(int argc, char const *argv[])
{
uint16_t port = 3850;
int new_socket;
//unicast UDP socket server
struct sockaddr_in socket_adresse;
int addrlen = sizeof(socket_adresse);
socket_adresse.sin_family = AF_INET;
socket_adresse.sin_port = htons(port);
socket_adresse.sin_addr.s_addr =  inet_addr("127.0.0.1");
char buffer[32] = {0};
int udp = socket(AF_INET,SOCK_STREAM,0);
int valread;
//bind socket 
if ( bind(udp, (struct sockaddr *)&socket_adresse, sizeof(socket_adresse)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
if (listen(udp, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
if ((new_socket = accept(udp, (struct sockaddr *)&socket_adresse, (socklen_t *)&addrlen))<0)
  {
      perror("accept");
      exit(EXIT_FAILURE);
  }

valread = read( new_socket , buffer, 32);

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
/*    
//sendto 
char *hello = "Hello from client";
if (sendto(socket_multi, hello, strlen(hello),0,(struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
    printf("\nsending failed \n");
}*/
//recvfrom
int recvfrom_int;
char recvfrom_buffer [1024];
recvfrom_int=recvfrom(socket_multi,recvfrom_buffer,1024,0,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
printf("received multicast packet: %s",recvfrom_buffer);

//decode the received bytestream into packet
//int temp_decode; // to keep compiler happy
struct Packet packet_recv = decode(buffer); 
printf("Packet received. Version: %u, Type: %u, Option: %u, SeqNo: %u, DataLen: %u \n",packet_recv.head.Version,packet_recv.head.type, packet_recv.head.options, packet_recv.head.SeqNo,packet_recv.head.DataLen);
//bitwise printing for testing
printf("Buffer 1: %u \n",buffer[1]);
/*while (buffer[0]) {
    if (buffer[0] & 1)
        printf("1");
    else
        printf("0");

    buffer[0] >>= 1;
}*/
printf("\n");
//printing all the buffer
/*for(unsigned int i =0; i<32; i++){
    printf("buffer i: %u \n",buffer[i]);
}*/
return 0;
}