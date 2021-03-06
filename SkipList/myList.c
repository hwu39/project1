#include <linux/kernel.h>
#include <stdlib.h>
#include <linux/string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <linux/string.h>
#include <linux/errno.h>

void dump();
long slmbx_init(unsigned int ptrs, unsigned int prob);
static int rand_level();
long slmbx_shutdown(void);
long slmbx_create(unsigned int id, int protected);
long slmbx_destroy(unsigned int id);
long slmbx_count(unsigned int id);
long slmbx_send(unsigned int id, const unsigned char *msg, unsigned int len);
long slmbx_recv(unsigned int id, unsigned char *msg, unsigned int len);
long slmbx_length(unsigned int id);

unsigned int MaxLevel;

unsigned int MaxPtr = 4294967295; //max number of pointers 2 to the 32 minus 1
unsigned int MaxID = 4294967295; //max id 2 to the 32 minus 1
  
static unsigned int generate_random_int(void);

static unsigned int next_random = 9001;

static unsigned int max_rand = 32768;

static unsigned int generate_random_int(void) {
    next_random = next_random * 1103515245 + 12345;
    return (next_random / 65536) % 32768;
}

static void seed_random(unsigned int seed) {
    next_random = seed;
}

//queue node
typedef struct Qnode{
  unsigned char* data;
  unsigned int length;
  struct Qnode* next;
}Qnode;

//queue list
typedef struct queue{
  struct Qnode* front;
  struct Qnode* back;
}queue;

//skiplist node
typedef struct node {
  unsigned long size; //size of message
  unsigned int id;
  int protected;
  struct queue *msg;
  struct node **next;
  //struct node *prev;
}node;

typedef struct skiplist{
  unsigned int level;
  unsigned int size;
  struct node* shead;
  struct node* stail;
}skiplist;
//global list
skiplist* list;
//node *create(char *data, node* next);
//node *prepend(node* head, char *data);

node* insert(node* head, unsigned int id)
{
  
  //check if head can be found
  if (head == NULL) {
    head = malloc(sizeof(node));
    head->id = id;
    head->next = NULL;
    return head;
  }
  else {
    
    //create new node
    node *newNode = (node*)malloc(sizeof(node));
    newNode->id = id;
    newNode->next = NULL;
    
    //check order
    if (id < head->id) {
      newNode->next = head;    
      head = newNode;
      return head;
    }
    else {
      //iterate through linked list to insert at correct spot
      node *curr, *prev; 
      curr = head;
      while (curr != NULL && curr->id <= id) {
	prev = curr;
	curr = curr->next;
      }
      newNode->next = curr;
      prev->next = newNode;
      return head;  
    }
  }
}

void dump() {
  node *temp = list->shead;
  while (temp && temp->next[1] != list->shead) {
    printf("%d %s  ",temp->next[1]->id, temp->next[1]->msg->front->data);
    temp = temp->next[1];
  }
  printf("\n");
}

//Initializes the mailbox system, setting up the initial state of the skip list. The ptrs parameter specifies the maximum number of pointers any node in the list will be allowed to have.
long slmbx_init(unsigned int ptrs, unsigned int prob)
{
  if (prob == 2 || prob == 4 ||prob == 8 || prob == 16) {
    MaxLevel = ptrs;
  }
  else {
    return -EINVAL;
  }
  if (ptrs == 0 || ptrs >= MaxPtr) {
    return -EINVAL;
  }
  list = malloc(sizeof(skiplist));
  node *header = (node*)malloc(sizeof(struct node));
  list->shead = malloc(sizeof(node));
  list->shead = header;
  header->id = MaxPtr;
  header->next = (node**)malloc(sizeof(node*) * (MaxLevel + 1));
  int i;
  for (i=0; i <= MaxLevel; i++) {
    header->next[i] = list->shead;
  }

  list->level = MaxLevel;
  list->size++;

  return 0;
}

static int rand_level()
{
  int level = 1;
  while (generate_random_int() < max_rand/2 && level < MaxLevel) {
    level++;
  }
  return level;
}

//Shuts down the mailbox system, deleting all existing mailboxes and any messages contained therein. Returns 0 on success. Only the root user should be allowed to call this function.
long slmbx_shutdown(void)
{
  node *curr = list->shead->next[1];
  while(curr != list->shead) {
    node *temp = curr->next[1];
    free(curr->next);
    free(curr);
    curr = temp;
  }
  free(curr->next);
  free(curr);
  free(list);
  
  return 0;
}

//Creates a new mailbox with the given id if it does not already exist (no duplicates are allowed).
long slmbx_create(unsigned int id, int protected)
{
  if (id == 0 || id >= MaxID) {
    return -EINVAL;
  }
  if (list == NULL) {
    return -ENODEV;
  }

  int i, level;
  //skiplist node as a dynamically allocated array

  node *new[MaxLevel + 1];
  node *temp = list->shead;
  //iterate through skiplist from top to bottom
  for (i=MaxLevel; i>0; i--) {
    while (temp->next[i]->id < id) {
      temp = temp->next[i];
    }
    new[i] = temp;
  }

  temp = temp->next[1];
  //return error if id already exists
  if (temp->id == id) {
    return -EEXIST;
  }
  else {
    level = rand_level();
    if (list->level < level) {
      for(i=(list->level + 1); i<(level + 1); i++){
	new[i] = list->shead;
      }
      list->level = level;
    }
    //insert node into skiplist
    temp = malloc(sizeof(node));
    temp->protected = protected;
    temp = insert(temp, id);
        
    //insert a number of nodes for that column determined by level
    for (i = 1; i<level; i++) {
      temp->next[i] = new[i]->next[i];
      new[i]->next[i] = temp;
    }

    //create mailbox queue
    queue *message = (queue*)malloc(sizeof(queue));
    temp->msg = message;
    temp->msg->front = NULL;
    temp->msg->back = NULL;
    
  }
  return 0;
}

//Deletes the mailbox identified by id if it exists and the user has permission to do so. If the mailbox has any messages stored in it, these messages should be deleted. Returns 0 on success or an appropriate error code on failure.
long slmbx_destroy(unsigned int id)
{
  int i;
  node *new[MaxLevel+1], *temp = list->shead;
  for (i=list->level; i>0; i--){
    while (temp->next[i] && temp->next[i]->id < id) {
      temp = temp->next[i];
    }
    new[i] = temp;
  }
  temp = temp->next[1];
  if (temp->id == id && temp) {
    for (i=1; i<list->level+1; i++) {
      if(new[i]->next[i] != temp) {
	break;
      }
      new[i]->next[i] = temp->next[i];
    }
    if(temp) {
      free(temp->next);
      free(temp);
    }
    while (list->level > 1 && list->shead->next[list->level] == list->shead) {
      list->level--;
      //success
      return 0;
    }
  }
  //mailbox not found
  return -ENOENT;
}

//Returns the number of messages in the mailbox identified by id if it exists and the user has permission to access it. Returns an appropriate error code on failure.
long slmbx_count(unsigned int id)
{
  int i;
  node *temp = list->shead;
  //iterate to find the node before the greater id
  for (i=list->level; i>0; i--){
    while (temp->next[i] && temp->next[i]->id < id) {
      temp = temp->next[i];
    }
  }
  //if id can't be found
  if(temp) {
    return -ENOENT;
  }
  //if message queue is still empty
  else if (temp->msg->front == NULL && temp->msg->back == NULL) {
    return -ESRCH;
  }
  else {
    //if id found and queue not empty
    return temp->msg->front->length;
  }
}

//Sends a new message to the mailbox identified by id if it exists and the user has access to it. The message shall be read from the user-space pointer msg and shall be len bytes long. Returns 0 on success or an appropriate error code on failure.
long slmbx_send(unsigned int id, const unsigned char *msg, unsigned int len)
{
  int i;
  node *temp = list->shead;
  for(i=list->level; i>0; i--) {
    while (temp->next[i]->id < id) {
      temp = temp->next[i];
    }
  }

  if(temp->next[i]->id == id) {
    Qnode *q = malloc(sizeof(Qnode));
    if (q == NULL) {
      return -ENOMEM;
    }
    //send data
    if (len <= sizeof(msg)) {
      q->data = malloc(sizeof(Qnode));
      q->length = len;
      //assign each character at a time
      unsigned int j;
      for (j=0; j<q->length; j++) {
	q->data[i] = msg[i];
      }
    }
    else {
      return -EINVAL;
    }

    q->next = NULL;
    //error if mailbox message not found
    if (temp->next[i]->msg == NULL) {
      free(q);
      return -ENOENT;
    }
    //if message queue is empty, assign message
    if(temp->next[i]->msg->front == NULL && temp->next[i]->msg->back == NULL) {
      temp->next[i]->msg->front = q;
      temp->next[i]->msg->back = q;
      return 0;
    }
    else if(temp->next[i]->msg->front == NULL || temp->next[i]->msg->back == NULL) {
      free(q);
      return -ENOENT;
    }
    else {
      temp->next[1]->msg->back->next = q;
      temp->next[1]->msg->back = q;
      return 0;
    }
  }
  return -ENOENT;
}


//Reads the first message that is in the mailbox identified by id if it exists and the user has access to it, storing either the entire length of the message or len bytes to the user-space pointer msg, whichever is less.
long slmbx_recv(unsigned int id, unsigned char *msg, unsigned int len)
{
  int i;
  node *temp = list->shead;
  for(i=list->level; i>0; i--) {
    while (temp->next[i]->id < id) {
      temp = temp->next[i];
    }
  }
  temp = temp->next[1];

  if (temp == NULL) {
    return -EINVAL;
  }
  
  if(temp->id == id) {
    //check whether message is empty
    if (temp->msg == NULL) {
      return -ENOENT;
    }
    Qnode *q = NULL;
    Qnode *head = NULL;
    if(temp->msg->front == NULL && temp->msg->back == NULL) {
      return -ESRCH;
    }
    else if(temp->msg->front == NULL || temp->msg->back == NULL) {
      free(q);
      return -ENOMEM;
    }
    else {
      //transfer message to msg
      if (temp->msg->front->length <= len) {
	for(i=0; i<temp->msg->front->length; i++) {
	  msg[i] = temp->msg->front->data[i];
	}
      }
      else {
	for (i=0; i<len; i++) {
	  msg[i] = temp->msg->front->data[i];
	}
	//update message length
	temp->msg->front->length = len;
      }
      //if len is less than the message length, remove entire message
      if (len < temp->msg->front->length) {
	temp->msg->front == NULL;
	temp->msg->back == NULL;
	temp->msg->front->length = 0;
      }
    }    
  }
  return sizeof(temp->msg);
}

//Retrieves the length (in bytes) of the first message pending in the mailbox identified by id, if it exists and the user has access to it. Returns the number of bytes in the first pending message in the mailbox on success, or an appropriate error code on failure.
long slmbx_length(unsigned int id)
{
  int i;
  node *temp = list->shead;
  for(i=list->level; i>0; i--) {
    while (temp->next[i]->id < id) {
      temp = temp->next[i];
    }
  }
  temp = temp->next[1];

  if (temp == NULL || temp->id != id) {
    return -EINVAL;
  }
  else {
    return sizeof(temp->msg);
  }
}


int main() {
  seed_random(time(NULL));
  unsigned int random_num  = generate_random_int();
  //examine random functionality
  printf("%u\n", random_num);
  //simple skiplist implementation
  node *head = NULL;
  head = insert(head, 100);
  head = insert(head,50);
  head = insert(head,10);
  head = insert(head,30);
  node *temp = head;
  printf("%u ",temp->id);
  temp = temp->next;
  printf("%u ",temp->id);
  temp = temp->next;
  printf("%u ",temp->id);
  temp = temp->next;
  printf("%u ",temp->id);
  printf("\n");
  //test init
  long init;
  init = slmbx_init(0,2);
  printf("%s\n",strerror(-init));
  init = slmbx_init(2,2);
  printf("%s\n",strerror(-init));
  init = slmbx_init(8,18);
  printf("%s\n",strerror(-init));
  init = slmbx_init(6,9);
  printf("%s\n",strerror(-init));

  //test create
  long create;
  create = slmbx_create(2,0);
  printf("%s\n",strerror(-create));
  create = slmbx_create(3,0);
  printf("%s\n",strerror(-create));
  create = slmbx_create(8,10);
  printf("%s\n",strerror(-create));
  create = slmbx_create(5,80);
  printf("%s\n",strerror(-create));
  
  //test destroy
  long delete;
  delete = slmbx_destroy(2);
  printf("%s\n",strerror(-delete));
  delete = slmbx_destroy(3);
  printf("%s\n",strerror(-delete));

  //test send
  long send;
  unsigned char *hi = malloc(sizeof(unsigned char));
  hi = "hello";
  send = slmbx_send(2,hi,50);
  printf("%s\n",strerror(-send));
  unsigned char *h = malloc(sizeof(unsigned char));
  h = "I love it";
  send = slmbx_send(2,h,90);
  printf("%s\n",strerror(-send));

  //test receive
  long recv;
  recv = slmbx_recv(3,"hi",10);
  printf("%s\n",strerror(recv));
  recv = slmbx_recv(3,"UMBC",36);
  printf("%s\n",strerror(recv));
  
  dump();

  //test shutdown
  long shut;
  shut = slmbx_shutdown();
  printf("%s\n",strerror(shut));
  dump();
  return 0;
}

