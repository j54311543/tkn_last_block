int isEmpty(packet* head);

//find a link with given key
struct packet* find(unsigned short id, packet* head);


//delete first item
struct packet* deleteFirst(packet* head);

//insert link at the first location
void insertFirst(unsigned short id, unsigned short seq, packet* head);
