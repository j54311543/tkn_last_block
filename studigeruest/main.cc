//----------------------------------------------------------------------
//  Technische Universit�t Berlin
//  Fachgebiet Telekommunikationsnetze
//
//  Block E
//
//  Andreas K�psel
//
//----------------------------------------------------------------------
//  CHAT-Programm Grundger�st
//  


#include "main.h"  
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h> #include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h> #include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 


int             option_index          = 0;
int             verbose               = 0;

fd_set          rfds;

int             appl_state;
struct timeval  globalTimer;

char            buf[MAX_MSG_LEN];
int             buflen;

void            setGlobalTimer(int, int);
void            clearGlobalTimer(void);
void            setNewState(int);
int             getApplState(void);




int main(int argc, char** argv) {

  int retval;
  int fd_val;
  int rc;
  int maxsockfd = 0;
  int state_count = 0;

  setNewState(STATE_INIT);

  parse_options(argc, argv);

  signal(SIGINT,  abort_chat);
  signal(SIGTERM, abort_chat);
  signal(SIGUSR1, db_dump);

  globalTimer.tv_sec  = 1;
  globalTimer.tv_usec = 0;
/*
  //Multicast socket init
  int socket_multi;
  socket_multi = socket(PF_INET,SOCK_DGRAM,0);  //returns fd of socket
  #define MC_GROUP_ADDR "224.45.2.1"
  //join multicast group
  mreq.imr_multiaddr.s_addr = inet_addr(MC_GROUP_ADDR); 
  mreq.imr_interface.s_addr = INADDR_ANY;
  setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))

    if(socket_multi < 0)
  {
	  perror(“open stream socket”);
	  exit(1);
  }

*/
//unicast UDP socket server
sockaddr_in socket_adresse;
socket_adresse.sin_family = AF_INET;
socket_adresse.sin_port = htons (DEFAULT_DATA_PORT);
socket_adresse.sin_addr.s_addr =  inet_addr("127.0.0.1");
char buffer[1024] = {0};
int udp = socket(AF_INET,SOCK_DGRAM,0);
//bind socket 
if ( bind(udp, (const struct sockaddr *)&socket_adresse, sizeof(socket_adresse)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
if (listen(udp, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
if (new_socket = accept(udp, (struct sockaddr *)&socket_adresse, sizeof(socket_adresse))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
valread = read( new_socket , buffer, 1024);
printf("%s\n",buffer );


//Unicast UDP socket client 
  char buffer[1024] = {0};
  sockaddr_in socket_adresse;
  socket_adresse.sin_family = AF_INET;
  socket_adresse.sin_port = htons (DEFAULT_DATA_PORT);
  socket_adresse.sin_addr.s_addr =  inet_addr("127.0.0.1");
int udp = socket(AF_INET,SOCK_DGRAM,0);
//connect socket 
if (connect(udp, (struct sockaddr *)&socket_adresse, sizeof(socket_adresse)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
//test connection 
char *hello = "Hello from client";
send(sock , hello , strlen(hello) , 0 );
printf("Hello message sent\n");
valread = read( sock , buffer, 1024);
printf("%s\n",buffer );


  

  init_fdSet(&rfds);

  FD_SET(,&rfds);
  
  maxsockfd = find_highest_fd() + 1;
  
  
  for (;;)
    {

	  // if globalTimer set, select with upper bound, else indefinitely long waiting
	  if ((globalTimer.tv_sec != 0) || (globalTimer.tv_usec != 0)) 
	    {
	      retval = select(maxsockfd, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)&globalTimer);
	    }
	  else
	    {
	      retval = select(maxsockfd, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);
	    }




     	  assert(retval >= 0);

	  
	  if (retval == 0)
	    {
	      
	      // Zustandsmaschine ChatTool
	      // switch (appl_state)
	      // case STATE_...:
	      //           do whatever is necessary
	      //           break;


	    }
	  else 
	    {
	      
	      for (fd_val = 0; retval--; fd_val++) {  
		
		while (!FD_ISSET(fd_val, &rfds))
		  fd_val++;  
		
		// console stdin
		if (fd_val == 0)
		  {
		    
		    // read line from stdin

		  }
		    
	      }


	    }

	  // restore rfds
	  init_fdSet(&rfds);
	  maxsockfd = find_highest_fd() + 1;

    }



  // just to make gcc happy
  return 0;

}

void error_handler (long error) {

   abort_chat(0);

}

void abort_chat(int status)
{


}

void db_dump(int status)
{



}

void version(char* progname)
{

  fprintf(stderr,"%s version 0.01\n",progname);

}

void usage(char* progname)
{

  fprintf(stderr,"usage: %s\n\t[-h|--help]\n\t[-V|--version]\n",progname);

}

void parse_options(int argc, char** argv) {
  
  int ret;
  
  while ((ret = getopt_long(argc,argv,"p:vVhs:l:",
			    long_options, &option_index)) != EOF)
    switch (ret) {
    case 'h':
      usage(argv[0]);
      break;               
    case 'V':
      version(argv[0]);
      break;               
    }

}








void
setGlobalTimer(int sec, int usec)
{

  static int i_sec;
  static int i_usec;

  if ((globalTimer.tv_sec == -1) && (globalTimer.tv_usec == -1))
    {
      //fprintf(stderr,"setting timer (2)\n");
      globalTimer.tv_sec  = i_sec;
      globalTimer.tv_usec = i_usec;
      return;
    }

  i_sec  = sec;
  i_usec = usec;

  if (sec <= globalTimer.tv_sec)
    if (usec <= globalTimer.tv_usec)
      {
	//fprintf(stderr,"setting timer (1)\n");
	globalTimer.tv_sec  = sec;
	globalTimer.tv_usec = usec;
      }

  if ((globalTimer.tv_sec == 0) && (globalTimer.tv_usec == 0))
    {
      //fprintf(stderr,"setting timer (2)\n");
      globalTimer.tv_sec  = sec;
      globalTimer.tv_usec = usec;
    }


}











void
clearGlobalTimer()
{
  globalTimer.tv_sec   = 0;
  globalTimer.tv_usec  = 0;
}










void
setNewState(int newState)
{

  appl_state = newState;

}






int
getApplState()
{

  return appl_state;

}







int
find_highest_fd()
{
  
  int fd = 0;

  
  // add additional file descriptors for data and net sockets


  return fd;

}








int
init_fdSet(fd_set* fds)
{

  // initialize fds structure
  FD_ZERO(fds);

  // add console filedescriptor
  FD_SET(0, fds);

}
