/*
 create by nicexw at www.baofeng.com
 Email: nicexw@gmail.com xiongwei@baofeng.com
*/
#include "queue.h"

struct queue* new_queue(char *server_filepath,char *name){
  struct queue* q;
	
  char *qu_name=(char*)malloc(sizeof(char)*50);
  sprintf(qu_name,"%s%s/%s",server_filepath,name,name);
  q = (struct queue*)malloc(sizeof(struct queue));
  q->queue_name=qu_name;
  q -> tail_cache = new_cache_queue();
  q -> head_cache = new_cache_queue();
  q -> file_queue = new_file_queue();
  q -> file_counter = 0;
  return q;
}


struct cache_queue* new_cache_queue(){
  struct cache_queue* cq;
  cq = (struct cache_queue*)malloc(sizeof(struct cache_queue));
  cq -> head = cq -> tail = NULL;
  cq -> node_num = 0;
  return cq;
}

void cache_push(struct cache_queue* cq, char* dt){
  struct node* tmp;
  struct node* p;
  char *res=(char *)malloc(sizeof(char)*NODE_LIMIT);
  sprintf(res,"%s",dt);
  tmp = (struct node*)malloc(sizeof(struct node));
  tmp -> data = res;
  tmp -> next = NULL;

  if((cq -> head == cq -> tail) && (cq -> head == NULL)){
    cq -> head = cq -> tail = tmp;
    cq -> node_num = 1;
  }
  else{
    p = cq -> tail;
    p -> next = tmp;
    cq -> tail = tmp;
    cq -> node_num ++;
  }
}
void pushhead(struct queue* q, char* dt)
{
	cache_push(q ->head_cache, dt);
}

void pushtail(struct queue* q, char* dt)
{
	cache_push(q ->tail_cache, dt);
}

void file_push(struct file_queue* cq,char *dt)
{
  struct file_node* tmp;
  struct file_node* p;
  
  char *res=(char *)malloc(sizeof(char)*50);
  sprintf(res,"%s",dt);
  tmp = (struct file_node*)malloc(sizeof(struct file_node));
  tmp ->name = res;
  tmp -> next = NULL;

  if((cq -> head == cq -> tail) && (cq -> head == NULL)){
    cq -> head = cq -> tail = tmp;
    cq ->file_num = 1;
  }
  else{
    p = cq -> tail;
    p -> next = tmp;
    cq -> tail = tmp;
    cq ->file_num ++;
  }
}
void pushfilequeue(struct queue* q,char* dt)
{
	file_push(q->file_queue,dt);
}

void push(struct queue* q, char* res,char *file_path, char *queuename){
  if(q -> file_counter == 0){
    if(q -> tail_cache -> node_num == CACHE_LIMIT) // we need to save
      {
		char* name = get_a_new_name(q,file_path,queuename);
		add_file(q -> file_queue, name);
		q -> file_counter ++;
		save_cache_queue_to_file(name,q->tail_cache);
		q -> tail_cache = new_cache_queue();
		cache_push(q -> tail_cache, res);
      }
    else cache_push(q -> tail_cache, res);
  }
  else{
    if(q -> tail_cache -> node_num == CACHE_LIMIT) // we need to save
      {
		char* name = get_a_new_name(q,file_path,queuename);
		add_file(q -> file_queue, name);
		q -> file_counter ++;
		save_cache_queue_to_file(name,q->tail_cache);
		q -> tail_cache = new_cache_queue();
		cache_push(q -> tail_cache, res);
      }
    else cache_push(q -> tail_cache, res);
  }    
}

char* cache_pop(struct cache_queue* cq){
  struct node* p;
  char *res = NULL;

  	if(cq -> head == cq -> tail && cq -> head==NULL)     // only one node
    {
    }
    else if(cq -> head == cq -> tail)
    {
      p = cq -> head;
      res = p -> data;
      cq -> head = cq -> tail = NULL;
      cq -> node_num = 0;
      free(p);
      p=NULL;
    }
  	else{
    	p = cq -> head;
    	cq -> head = p -> next;
    	cq -> node_num --;
    	res = p -> data;
    	free(p);
    	p=NULL;
  	}
  return res;
}

char* pop(struct queue* q){
  char *res = NULL;
  if(q ->file_queue->file_num == 0){
    if(q -> head_cache -> node_num > 0)
    {
      res = cache_pop(q -> head_cache);
    }
    else if(q ->tail_cache->node_num > 0)
    {
       res = cache_pop(q ->tail_cache);
    }  
    else res = NULL;
  }
  else{
    if(q -> head_cache -> node_num > 0)
      res = cache_pop(q -> head_cache);
    else{
    	while(q -> file_queue->head !=NULL)
    	{
      		char* name = get_file_name(q -> file_queue);
      		q ->file_counter--;
      		
      		if(name !=NULL)
      		{
      			FILE *fptest;
      			fptest=fopen(name,"r");
      			if(fptest!=NULL)
      			{
      				fclose(fptest);
      				read_cache_queue_from_file(name,q -> head_cache);
      				remove(name); // remove the file from disk
      				free(name);
      				name=NULL;
      				break;
      			}
      			else
      			{
      				free(name);
      				name=NULL;
      			}

      		}
      		else
      		{
      			//break;
      		}

    	}
      	res = cache_pop(q -> head_cache);
      	if(res==NULL)
      	{
      		res=cache_pop(q->tail_cache);
      	}
    }
  }
  return res;
}


char* get_a_new_name(struct queue* q,char *file_path,char *queuename){
  char *name=malloc(sizeof(char)*50);
  int dir_num=q->file_queue->file_num/FILE_LIMIT;
  sprintf(name,"%s%s/",file_path,queuename);
  if(mkdir(name, 0777) == -1){
    	//perror("Couldn't create the directory.\n");
  }
  
  sprintf(name,"%s%s/q_%i/",file_path,queuename,dir_num);
  if(q->file_queue->file_num%FILE_LIMIT == 0)
  {
  	 if(mkdir(name, 0777) == -1){
    	//perror("Couldn't create the directory.\n");
  	}
  }
  sprintf(name,"%s%s/q_%i/queue_%i",file_path,queuename,dir_num,q->file_counter);

  return name;
}

void save_cache_queue_to_file(char* filename, struct cache_queue* cq){
  struct node* p;
  FILE* outfile = fopen(filename, "w");

  for(p = cq -> head; p; p = p -> next){
    fprintf(outfile,"%s\n", p -> data);
    free(p -> data);
    free(p);
  }
  free(cq);
  cq = NULL;
  
  fflush(outfile);
  fclose(outfile);
}

void read_cache_queue_from_file(char* filename,struct cache_queue* res){
  FILE* infile = fopen(filename, "r");
  char* dt=(char*)malloc(sizeof(char)*NODE_LIMIT);
  while(!feof(infile)){
    fscanf(infile,"%s\n",dt);
    cache_push(res,dt);
  }
  free(dt);
  fclose(infile);
}

struct file_queue* new_file_queue(){
  struct file_queue* q;
  
  q = (struct file_queue*)malloc(sizeof(struct file_queue));
  q -> file_num = 0;
  q -> head = q -> tail = NULL;
  return q;
}

void add_file(struct file_queue* q, char* filename){
  struct file_node* tmp;
  struct file_node* p;
  
  tmp = (struct file_node*)malloc(sizeof(struct file_node));
  tmp -> name = filename;
  tmp -> next = NULL;
  
  if((q -> head == q -> tail) && (q -> head == NULL))
  {
    q -> head = q -> tail = tmp;
    q->file_num=1;
  }
  else{
    p = q -> tail;
    p -> next = tmp;
    q -> tail = tmp;
    q->file_num++;
  }

}

char* get_file_name(struct file_queue* q){
  struct file_node* p;
  char* res = NULL;
  
  if((q -> head == q -> tail) && (q -> head == NULL))
    res = NULL;
  else if (q -> head == q -> tail)
    {
      p = q -> head;
      res = p -> name;
      q -> head = q -> tail = NULL;
      q->file_num--;
      free(p);
      p=NULL;
    }
  else{
    p = q -> head;
    q -> head = p -> next;
    q->file_num--;
    res = p -> name;
    free(p);
    p=NULL;
  }
  return res;
}

void freeQueue(struct queue* list)
{
	freeQueueGeneric(list,0);
}

void freeQueueGeneric(struct queue* list,int data)
{
	struct node *q=NULL;
	struct file_node *p=NULL;
    while(list->head_cache->head!=NULL)
    {
       q=list->head_cache->head;
       list->head_cache->head=q->next;
       q->next=NULL;
       free(q->data);
       q->data=NULL;
       free(q);
       q=NULL; 		
    }
    while(list->tail_cache->head!=NULL)
    {
    	q=list->tail_cache->head;
    	list->tail_cache->head=q->next;
    	q->next=NULL;
    	free(q->data);
    	q->data=NULL;
    	free(q);
    	q=NULL;
    }
    while(list->file_queue->head!=NULL)
    {
    	p=list->file_queue->head;
    	list->file_queue->head=p->next;
    	p->next=NULL;
    	if(data)
    	{
    		remove(p->name);
    	}
    	free(p->name);
    	p->name=NULL;
    	free(p);
    	p=NULL;
    }
    if(data)
    {
    	remove(list->queue_name);
    }
    free(list->queue_name);
    list->queue_name=NULL;
	free(list->tail_cache);
	list->tail_cache=NULL;
  	free(list->head_cache);
  	list->head_cache=NULL;
  	free(list->file_queue);
  	list->file_queue=NULL;
  	free(list);
  	list=NULL;
}
