#include <linux/kernel.h>
#include <stdlib.h>
#include <linux/string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <linux/string.h>
#include <linux/errno.h>

unsigned int MaxLevel;

unsigned int MaxPtr = 4294967295; //2 to 32 minus 1
  
static unsigned int generate_random_int(void);

static unsigned int next_random = 9001;

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
  struct queue *msg;
  struct node **next;
  //struct node *prev;
}node;

typedef struct skiplist{
  unsigned int level;
  unsigned int size;
  struct node* shead;
  struct ndoe* stail;
}skiplist;

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

//Initializes the mailbox system, setting up the initial state of the skip list. The ptrs parameter specifies the maximum number of pointers any node in the list will be allowed to have.
long slmbx_init(unsigned int ptrs, unsigned int prob)
{
  if (prob % 2 != 0 && (prob != 2 || prob != 4 || prob != 8 || prob != 16)) {
    return -EINVAL;
  }
  if (ptrs == 0 || ptrs >= MaxPtr) {
    return -EINVAL;
  }
  MaxLevel = ptrs;
  node *header = (node*)malloc(sizeof(struct node));
  list->shead = header;
  header->id = (2 << 29) - 1;
  header->next = (node**)malloc(sizeof(node*) * (MaxLevel + 1));
  int i;
  for (i=0; i <= MaxLevel; i++) {
    header->next[i] = list->shead;
  }

  list->level = 1;
  list->size = 0;

  return 0;
}

//Shuts down the mailbox system, deleting all existing mailboxes and any messages contained therein. Returns 0 on success. Only the root user should be allowed to call this function.
long slmbx_shutdown(void)
{
  
}

//Creates a new mailbox with the given id if it does not already exist (no duplicates are allowed).
long slmbx_create(unsigned int id, int protected)
{
  
}

//Deletes the mailbox identified by id if it exists and the user has permission to do so. If the mailbox has any messages stored in it, these messages should be deleted. Returns 0 on success or an appropriate error code on failure.
long slmbx_destroy(unsigned int id)
{

}

//Returns the number of messages in the mailbox identified by id if it exists and the user has permission to access it. Returns an appropriate error code on failure.
long slmbx_count(unsigned int id)
{

}

//Sends a new message to the mailbox identified by id if it exists and the user has access to it. The message shall be read from the user-space pointer msg and shall be len bytes long. Returns 0 on success or an appropriate error code on failure.
long slmbx_send(unsigned int id, const unsigned char *msg, unsigned int len)
{

}


//Reads the first message that is in the mailbox identified by id if it exists and the user has access to it, storing either the entire length of the message or len bytes to the user-space pointer msg, whichever is less.
long slmbx_recv(unsigned int id, unsigned char *msg, unsigned int len)
{

}

//Retrieves the length (in bytes) of the first message pending in the mailbox identified by id, if it exists and the user has access to it. Returns the number of bytes in the first pending message in the mailbox on success, or an appropriate error code on failure.
long slmbx_length(unsigned int id)
{

}


int main() {
  seed_random(time(NULL));
  unsigned int random_num  = generate_random_int();
  //examine random functionality
  printf("%u\n", random_num);
  //simple skiplist implementation
  node *head = NULL;
  head = insert(head, 100);
  head = insert(head,20);
  head = insert(head,40);
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

  return 0;
}

