/*
create by nicexw at www.baofeng.com
Email: nicexw@gmail.com xiongwei@baofeng.com
*/
#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <string.h>
#define CACHE_LIMIT 3
#define FILE_LIMIT 3
#define NODE_LIMIT 65536


// the data queue and node
typedef struct node{
  char* data;
  struct node* next;
}node;

struct cache_queue{
  int node_num;
  struct node* head;
  struct node* tail;
};

// the queue and node for filenames.
struct file_node{
  char* name;
  struct file_node* next;
};

struct file_queue{
  int file_num;
  struct file_node* head;
  struct file_node* tail;
};

struct queue{
  char * queue_name;
  struct cache_queue* head_cache;
  struct cache_queue* tail_cache;
  struct file_queue* file_queue;
  int file_counter;
};


struct cache_queue* new_cache_queue();
struct queue* new_queue(char *server_filepath,char *name);
char* get_a_new_name(struct queue* q,char *file_path,char *queuename);
void save_cache_queue_to_file(char* filename, struct cache_queue* cq);
void read_cache_queue_from_file(char* filename,struct cache_queue* res);
struct file_queue* new_file_queue();
void add_file(struct file_queue* q, char* filename);
char* get_file_name(struct file_queue* q);
void push(struct queue* q, char* dt,char *file_path,char *queuename);
char* pop(struct queue* q);
void pushhead(struct queue* q, char* dt);
void pushtail(struct queue* q, char* dt);
void pushfilequeue(struct queue* q,char* dt);
void file_push(struct file_queue* cq,char *dt);
void freeQueueGeneric(struct queue* list,int data);
void freeQueue(struct queue* list);

#endif /*QUEUE_H_*/
