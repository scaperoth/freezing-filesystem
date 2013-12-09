#include "list.h"

int main(void){

  struct linked_list* ll = ll_create();

  printf("add nodes...");
  ll_add(ll,1);
  ll_add(ll,2);
  ll_add(ll,4);
  ll_add(ll,4);
  ll_add(ll,5);
  ll_add(ll,7);
  ll_add(ll,9);

  ll_display(ll);

  printf("remove node 4\n");
  ll_remove(ll,4);

  ll_display(ll);

  int a = (int)ll_dequeue(ll);
  printf("remove node value %d\n",a);

  a = (int)ll_dequeue(ll);
  printf("remove node value %d\n",a);

  a = (int)ll_dequeue(ll);
  printf("remove node value %d\n",a);

  a = (int)ll_dequeue(ll);
  printf("remove node value %d\n",a);


  ll_display(ll);

  printf("destroy list\n");

  ll_destroy(ll);

  int length2 = ll_length(ll);
  printf("number in list after deletion: %d\n",length2);

  ll_display(ll);

  return 0;
}
