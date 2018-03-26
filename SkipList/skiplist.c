#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/linkage.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include 


//Initializes the mailbox system, setting up the initial state of the skip list. The ptrs parameter specifies the maximum number of pointers any node in the list will be allowed to have.
asmlinkage long slmbx_init(unsigned int ptrs, unsigned int prob)
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
asmlinkage long slmbx_shutdown(void)
{

}

//Creates a new mailbox with the given id if it does not already exist (no duplicates are allowed).
asmlinkage long slmbx_create(unsigned int id, int protected)
{

}

//Deletes the mailbox identified by id if it exists and the user has permission to do so. If the mailbox has any messages stored in it, these messages should be deleted. Returns 0 on success or an appropriate error code on failure.
asmlinkage long slmbx_destroy(unsigned int id)
{

}

//Returns the number of messages in the mailbox identified by id if it exists and the user has permission to access it. Returns an appropriate error code on failure.
asmlinkage long slmbx_count(unsigned int id)
{

}

//Sends a new message to the mailbox identified by id if it exists and the user has access to it. The message shall be read from the user-space pointer msg and shall be len bytes long. Returns 0 on success or an appropriate error code on failure.
asmlinkage long slmbx_send(unsigned int id, const unsigned char *msg, unsigned int len)
{

}


//Reads the first message that is in the mailbox identified by id if it exists and the user has access to it, storing either the entire length of the message or len bytes to the user-space pointer msg, whichever is less.
asmlinkage long slmbx_recv(unsigned int id, unsigned char *msg, unsigned int len)
{

}

//Retrieves the length (in bytes) of the first message pending in the mailbox identified by id, if it exists and the user has access to it. Returns the number of bytes in the first pending message in the mailbox on success, or an appropriate error code on failure.
asmlinkage long slmbx_length(unsigned int id)
{

}
