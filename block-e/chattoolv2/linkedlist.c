#include "linkedlist.h"
#include "main.h"
#include <stdlib.h>

int isEmpty(packet* head) {
   return head == NULL;
}

//find a link with given id
struct packet* find(unsigned short id, packet* head) {

   //start from the first link
   struct packet* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->id != id) {
	
      //if it is last packet
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if data found, return the current Link
   return current;
}


//delete first item
struct packet* deleteFirst(packet* head) {

   //save reference to first link
   struct packet *tempLink = head;
	
   //mark next to first link as first 
   head = head->next;
	
   //return the deleted link
   return tempLink;
}

//insert link at the first location
void insertFirst(unsigned short id, unsigned short seq, packet* head) {
   //create a link
   struct packet *link = (struct packet*) malloc(sizeof(struct packet));
	
   link->id = id;
   link->seq = seq;
	
   //point it to old first packet
   link->next = head;
	
   //point first to new first packet
   head = link;
}

