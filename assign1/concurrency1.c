#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include "mt19937ar.c"

void setRegisters();
int rdrand(int *val);
int RNGcheck(int min, int max);

int eax;
int ebx;
int ecx; 
int edx;

pthread_t produce;
pthread_t consume;
pthread_cond_t producerCondition;
pthread_cond_t consumerCondition;
pthread_mutex_t lock;

struct Data {
        int number;
        int sleepTime;
};

struct Data buffer[32];
int bufferIndex = 0;

void *producer(void *arg) {
    struct Data tmp;
    // Generate random sleep time for initial sleep
    int sleepData = RNGcheck(2, 9);
    while(1){
        sleep(sleepData);
        pthread_mutex_lock(&lock); //lock mutex
        
        //Wait until buffer is not full
        while (bufferIndex == 32){
            printf("Pending Consumer...\n");
            pthread_cond_wait(&producerCondition, &lock);
        }
        
        //Generate random producer values to add into buffer
        tmp.number = RNGcheck(1, 99);
        tmp.sleepTime = RNGcheck(2, 9);
        
        //Insert data into buffer
        buffer[bufferIndex] = tmp; 
        printf("Producer: index #%d, Buffer number:%d, sleep time:%d\n", bufferIndex, tmp.number, tmp.sleepTime);
        bufferIndex++;
        
        //Wake consumer thread & unlock mutex
        pthread_cond_signal(&consumerCondition);
        pthread_mutex_unlock(&lock);
    }
}

void *consumer(void *arg) {
    while(1){
        //Lock mutex before checking buffer
        pthread_mutex_lock(&lock);
        
        //Wait until there is space in the buffer and fill with data from producer
        while (bufferIndex == 0){
            printf("Pending Producer...\n");
            pthread_cond_wait(&consumerCondition, &lock);
        }
        
        //Unlock mutex
        pthread_mutex_unlock(&lock);

        //Get values for consumption
        sleep(buffer[bufferIndex-1].sleepTime);

        //Lock mutex before consumption
        pthread_mutex_lock(&lock);
        
        //Print consumption values
        printf("Consumer: consumed index #%d, Buffer number:%d\n", bufferIndex-1, buffer[bufferIndex-1].number);
        bufferIndex--;
        
        //Wake producer threader and unlock mutex
        pthread_cond_signal(&producerCondition);
        pthread_mutex_unlock(&lock);
    }
}

//Rdrand Implementation from: 
//https://github.com/vincenthz/hs-crypto-random/blob/master/cbits/rdrand.c
int rdrand(int *val) {
    unsigned char err;
    __asm__ __volatile__("rdrand %0; setc %1" : "=r" (*val), "=qm" (err));
    return (int) err;
}

void setRegisters() {
    //Set EAX register
    eax = 0x01;
    __asm__ __volatile__("cpuid;" : "=a"(eax), "=b"(ebx), "=c"(ecx), 
                            "=d"(edx) : "a"(eax));
}

int RNGcheck(int min, int max) {
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

void signalHandler(int sig)
{
	if(sig == SIGINT){
		pthread_detach(produce);
                pthread_detach(consume);
		exit(EXIT_SUCCESS);
	}
}

//Threads Implementation from:
//https://computing.llnl.gov/tutorials/pthreads/
int main() {
    signal(SIGINT, signalHandler);
    memset(buffer, 0, sizeof(buffer));

    //initialize thread variables
    pthread_cond_init(&producerCondition, NULL);
    pthread_cond_init(&consumerCondition, NULL);
    pthread_mutex_init(&lock, NULL);
    //create the threads
    pthread_create(&consume, NULL, consumer, NULL);
    pthread_create(&produce, NULL, producer, NULL);
    //join the threads
    pthread_join(produce, NULL);
    pthread_join(consume, NULL);
    
    return 0;

}