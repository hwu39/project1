#include <errno.h>
#include <linux/kernel.h>
#include <stdlib.h>
#include <linux/string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
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
  struct Qnode* head;
  struct Qnode* tail;
}queue;

//skiplist node
typedef struct node {
  unsigned long size; //size of message
  unsigned int id;
  struct queue *msg;
  struct node *next;
  //struct node *prev;
}node;

typedef struct skiplist{
  unsigned int level;
  unsigned int size;
  struct node* shead;
  struct ndoe* stail;
}skiplist;

//node *create(char *data, node* next);
//node *prepend(node* head, char *data);

//create new node to carry message
node *create(unsigned int id, node* next) {
  node *temp = (node*)malloc(sizeof(node));
  if (temp == NULL) {
    printf("Error! Can't create new node!\n");
    exit(0);
  }
  temp->id = id;
  temp->next = next;

  return temp;
}

void dump(struct node *head) {
  struct node *temp = (node*)malloc(sizeof(node));
  temp = head;
  /*
  while (temp != NULL) {
    printf("%d ",temp->id);
    temp = temp->next;
  }
  */
  printf("\n");
}

//point next pointer of new node to the head
node *prepend(node *head, unsigned int id){
  node *temp = create(id,head);
  head = temp;
  return head;
}

node* insert(node* head, unsigned int id)
{
  //check if head can be found
  if (head == NULL) {
    head = malloc(sizeof(node));
    if (head == NULL) {
      printf("Cannot create head node\n");
      return head;
    }
    head->id = id;
    head->next = NULL;
    return head;
  }

  //create new node
  node *newNode;
  newNode = malloc(sizeof(node));
  if(newNode == NULL) {
    printf("Cannot create node\n");
    return newNode;
  }

  //check order
  if (id < head->id) {
    newNode->next = head;
    return newNode;
  }

  //iterate through linked list to insert at correct spot
  node* temp, *prev;
  temp = head->next;
  prev = head;
  while (temp != NULL && temp->id < id) {
    prev = temp;
    temp = temp->next;
  }
  newNode->next = temp;
  prev->next = newNode;

  return head;
}

/*
//Initializes the mailbox system, setting up the initial state of the skip list. The ptrs parameter specifies the maximum number of pointers any node in the list will be allowed to have.
long slmbx_init(unsigned int ptrs, unsigned int prob)
{
  if (prob % 2 != 0 && (prob != 2 || prob != 4 || prob != 8 || prob != 16)) {
    return -EINVAL;
  }
  if (ptrs == 0) {
    return -EINVAL;
  }
  struct skiplist *slist = (skiplist*)malloc(sizeof(skiplist));
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
  unsigned int a = 10;
  struct node *head = prepend(head, a);
  dump(head);
  return 0;
}
*/
