#include <stdlib.h>
#include <string.h>
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

//message node
typedef struct node {
  unsigned long size; //size of message
  char *message;
  struct node *next;
  struct node *prev;
}node;

//node *create(char *data, node* next);
//node *prepend(node* head, char *data);

//create new node to carry message
node *create(char *data, node* next) {
  node *temp = (node*)malloc(sizeof(node));
  if (temp == NULL) {
    printf("Error! Can't create new node!\n");
    exit(0);
  }
  temp->message = data;
  temp->next = next;

  return temp;
}

void dump(node *head) {
  for(head;head != NULL; head=head->next) {
    printf("%s ",head->message);
  }
  printf("\n");
}

//point next pointer of new node to the head
node *prepend(node *head, char *data){
  node *temp = create(data,head);
  head = temp;
  return head;
}

int main() {
  seed_random(time(NULL));
  unsigned int random_num  = generate_random_int();
  //examine random functionality
  printf("%u\n", random_num);
  //simple skiplist implementation
  struct node *head = prepend(head, "Hi");
  dump(head);
  return 0;
}
