#include <stdio.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_slmbx_init 333
#define __NR_slmbx_shutdown 334
#define __NR_slmbx_create 335
#define __NR_slmbx_destroy 336
#define __NR_slmbx_count 337
#define __NR_slmbx_send 338
#define __NR_slmbx_recv 339
#define __NR_slmbx_length 340

long slmbx_init(unsigned int ptrs, unsigned int prob){
  return syscall(__NR_slmbx_init, ptrs, prob);
}

long slmbx_shutdown(void){
  return syscall(__NR_slmbx_shutdown);
}

long slmbx_create(unsigned int id, int protected) {
  return syscall(__NR_slmbx_create,id,protected);
}

long slmbx_destroy(unsigned int id) {
  return syscall(__NR_slmbx_destroy,id);
}

long slmbx_count(unsigned int id) {
  return syscall(__NR_slmbx_count,id);
}

long slmbx_send(unsigned int id, const unsigned char *msg, unsigned int len) {
  return syscall( __NR_slmbx_send,id,*msg,len);
}

long slmbx_recv(unsigned int id, unsigned char *msg, unsigned int len) {
  return syscall(__NR_slmbx_recv,id,*msg,len);
}

long slmbx_length(unsigned int id) {
  return syscall(#define __NR_slmbx_length,id);
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

