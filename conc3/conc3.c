#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syscall.h>

#include <sys/syscall.h>

#include "mt19937ar.c"

typedef struct List{
        int value;
        struct List *next;
}list_t;

list_t *llist;

pthread_t search_thread[3];
pthread_t insert_thread[3];
pthread_t delete_thread[3];

sem_t noInserter;
sem_t noSearcher;

pthread_mutex_t insert_lock;
pthread_mutex_t delete_lock;

int eax;
int ebx;
int ecx; 
int edx;

void *searcher(void *arg);
void *inserter(void *arg);
void *deleter(void *arg);

void setRegisters();
int rdrand(int *val);
int createRNG(int min, int max);
void getLinkedList(list_t *llist);

void *searcher(void *arg)
{
	while(1){
		//wait until delete finishes
		sem_wait(&noSearcher);
		
		printf("Searcher[%d] running...\n", syscall(SYS_gettid));
		printf("linkedlist: ");
		
		//search through linked list
		getLinkedList(llist);
		printf("\n");
		
		sleep(3);
	}
}

void getLinkedList(list_t *llist)
{
	if(llist == NULL){
		printf("LINKED LIST EMPTY...\n");
	}
	else{
		while(llist != NULL){
			printf("%d ", llist->value);
			llist = llist -> next;
	}
	printf("\n");
	}
}

void *inserter(void *arg)
{
	int rng_val;
	int sigID;
	
	//get pid of thread
	sigID = syscall(SYS_gettid);
	
	list_t *head;
	list_t *tmp;
	
	head = llist;
	
	while(1){
		rng_val = genrand_int32() % 10;

		//wait for delete to finish
		sem_wait(&noInserter);
		
		//lock insert mutex
		pthread_mutex_lock(&insert_lock);
		printf("Inserter Mutex Locked\n");
		
		if(head == NULL){
			//insert head
			head = (list_t *)malloc(sizeof(list_t));
			head -> next =  NULL;
			head -> value = rng_val;
			llist = head;
			printf("Inserter[%lu]: Inserting to head, %d\n", sigID, rng_val);
		}
		else{
			//insert tail
			tmp = (list_t *)malloc(sizeof(list_t));
			tmp -> value = rng_val;
			tmp -> next = NULL;
			
			//traverse to tail
			while(head -> next != NULL){
				head = head -> next;
			}

			head -> next = tmp;
			printf("Inserter[%lu]: Inserting to tail, %d\n", sigID, rng_val);
		}
		sleep(3);
		
		//unlock insert mutex
		pthread_mutex_unlock(&insert_lock);
		printf("Inserter Mutex Unlocked\n");
	}
}

void *deleter(void *arg)
{
	int rng_val;
	int sigID;
	
	//get pid of thread
	sigID = syscall(SYS_gettid);
	
	list_t *tmp;
	list_t *prev;
	
	while(1){
		tmp = llist;
		
		rng_val = genrand_int32() % 10;
		
		//lock delete mutex
		pthread_mutex_lock(&delete_lock);
		printf("Deleter Mutex Locked\n");
		
		while(tmp != NULL && llist != NULL){
			if(tmp -> value == rng_val){
				if(tmp == llist){
					printf("Searcher&Inserter Locked\n");
					llist = tmp -> next;
					free(tmp);
					printf("Deleter[%d]: Deleted head, %d\n", sigID, rng_val);
					break;
				}
				else{
					printf("Searcher&Inserter Locked\n");
					prev->next = tmp->next;
					free(tmp);
					printf("Deleter[%d]: Deleted node, %d\n", sigID, rng_val);
					break;
				}
			}
			else{
				prev = tmp;
				tmp = tmp->next;
			}
			break;
		}
		printf("Deleter Mutex Unlocked\n");
		pthread_mutex_unlock(&delete_lock);
		
		//release lock on inserter and searcher		
		printf("Searcher and/or Inserter Unlocked\n");
		sem_post(&noInserter);
		sem_post(&noSearcher);
		
		sleep(3);
	}
}

//Rdrand Implementation from: 
//https://github.com/vincenthz/hs-crypto-random/blob/master/cbits/rdrand.c
int rdrand(int *val)
{
    unsigned char err;
    __asm__ __volatile__("rdrand %0; setc %1" : "=r" (*val), "=qm" (err));
    return (int) err;
}

void setRegisters()
{
    //Set EAX register
    eax = 0x01;
    __asm__ __volatile__("cpuid;" : "=a"(eax), "=b"(ebx), "=c"(ecx), 
                            "=d"(edx) : "a"(eax));
}

int createRNG(int min, int max)
{
    int val = 0;
    
    init_genrand(time(NULL)); //Initialize Mersenne Twister
    setRegisters();
    
    //Check bit 30 of the ECX register for rdrand system compatibility.
    if (ecx & 1<<30) {
        rdrand(&val);
    }
    //Use Mersenne Twister if incompatible
    else {
        val = (int)genrand_int32();
    }
    
    val = abs(val);
    val %= (max - min);
    if (val < min) {
        val = min;
    }
    return val;
}

int main()
{
	//semaphore for excluding inserter & searcher when deleting 
	sem_init(&noInserter, 0, 1);
	sem_init(&noSearcher, 0, 1);
	
	//setup mutexes for insert & delete
	pthread_mutex_init(&insert_lock, NULL);
	pthread_mutex_init(&delete_lock, NULL);

	//initiate threads
	for(int i = 0; i < 3; i++){
		pthread_create(&search_thread[i], NULL, searcher, NULL);
		pthread_create(&insert_thread[i], NULL, inserter, NULL);
		pthread_create(&delete_thread[i], NULL, deleter, NULL);
	}

	for(int i = 0; i < 3; i++){
		pthread_join(insert_thread[i], NULL);
		pthread_join(search_thread[i], NULL);
		pthread_join(delete_thread[i], NULL);
	}
}