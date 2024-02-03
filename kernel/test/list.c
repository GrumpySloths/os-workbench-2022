#include <list.h>
#include<stdlib.h>
#include<stdio.h>

// static list_t* head = NULL;
static void push(void*pt){
    if(head==NULL){
        head = (list_t*)malloc(sizeof(list_t));
        head->pt = pt;
        head->next = NULL;
        length++;
        return;
    }
    list_t* cur;
    for (cur = head; cur->next != NULL;cur=cur->next){}
    list_t* node = (list_t*)malloc(sizeof(list_t));
    node->pt = pt;
    node->next = NULL;
    cur->next = node;
    length++;
    return;
}
// 产生一个合法的要free的指针
static void* pop(){
   if(!length){
       printf("cur malloc list is empty,remove disabled\n");
       return NULL;
       //    exit(EXIT_SUCCESS);
   }
   int id = rand() % length;
   //id为0时的处理
   if(!id){
       list_t* tmp = head;
       head = head->next;
       void* pt = tmp->pt;
       length--;
       free(tmp);
       return pt;
   }
   list_t* pre=head;
   for (int i = 0; i < id-1;i++){
       pre = pre->next;
   }
   list_t* tmp = pre->next;
   void* pt = tmp->pt;
   pre->next = tmp->next;
   free(tmp);
   length--;
   return pt;
}

MODULE_DEF(malloclist) = {
    .push = push,
    .pop = pop,
}
;