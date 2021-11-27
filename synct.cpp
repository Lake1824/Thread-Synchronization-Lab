/*
 * Matthew Lake 
 * 11/1/2021
 * CS 3080 Lab Assignment 4
 * This lab is a extention of lab 3 but we are focusing on implementing thread synchonization.
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PRODUCERS 6
#define NUM_CONSUMERS 6


#define BSIZE 10		// Buffer size is 10

int buf[BSIZE];
int NextIn=0, NextOut=0;


void * producer(void *);	// function for producer thread
void * consumer(void *);	// function for consumer thread

pthread_t ptid[NUM_PRODUCERS];      // array of producer thread IDs
pthread_t ctid[NUM_CONSUMERS];      // array of producer thread IDs

//pthread conditional varibles
pthread_cond_t empty;
pthread_cond_t full;

//Critical section lock
pthread_mutex_t lock;

//Declare varibles
int totMin;
int totMax = 0;
int totSum = 0;
int range;
int NUM_ITEMS;		// Num Items to produce or consume
int counter = 0; //Global counter

int main(int argc, char *argv[]){
	printf("\n **** Main Program creating threads ++++ \n");

	//Assigning the arguments
	NUM_ITEMS = atoi(argv[1]);
	range = atoi(argv[2]);
	totMin = range + 1;

	pthread_attr_t attr;

	//Initalizing conditions
	pthread_attr_init(&attr);
	pthread_cond_init(&full, NULL);
	pthread_cond_init(&empty, NULL);
	pthread_mutex_init(&lock, NULL);
	pthread_cond_signal(&empty);

	int i;
	

	//Create threads 
	for (i = 0; i < NUM_PRODUCERS; i++) {
		pthread_create(&ptid[i], &attr, producer, NULL);
	}
	for (i = 0; i < NUM_CONSUMERS; i++) {
		pthread_create(&ctid[i], NULL, consumer, NULL);
	}

	// join threads;
	for (i = 0; i < NUM_PRODUCERS; i++) {    //terminating threads
		pthread_join(ptid[i], NULL);
	}
	for (i = 0; i < NUM_CONSUMERS; i++) {    //terminating threads
		pthread_join(ctid[i], NULL);
	}

	printf("\n The overall minimum integer was: %d\n" , totMin);
	printf("\n The overall maximum integer was: %d\n" , totMax);

	int totAvg = totSum / (NUM_ITEMS*NUM_CONSUMERS);
	printf("\n The overall average is: %d\n", totAvg);
	pthread_exit(0);
	return 0; 
}  

  

void * producer(void * parm){
	int producedNum;

	printf("\n ++++ Producer started ++++ \n");

	//Get the pthread ID
	pthread_t myID = pthread_self();
	srand(myID);

	for (int i = 0; i < NUM_ITEMS; i++) {
		//Produce item 
		producedNum = rand() % range;

		//Lock
		pthread_mutex_lock(&lock);

		//Wait if needed
		while (counter >= BSIZE) {
			pthread_cond_wait(&empty, &lock);
		}
		counter++;
	
		// place item in the buffer
		buf[NextIn] = producedNum;
		NextIn = (NextIn + 1) % BSIZE;
		
		//unlock and signal
		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&full);

		printf(" Produced item: %d\n", producedNum);
	}
	printf("\n +++ Producer exiting +++ \n");
	pthread_exit(0);
}    

void * consumer(void * parm){
	int fetchNum;
	int cCounter = 0;

	//Initalize local variables
	int myMin = range + 1;
	int myMax = 0;
	int mySum = 0;

	printf("\n --- Consumer started --- \n");

        while (cCounter < NUM_ITEMS){
			//Fetch an item from the buffer
			fetchNum = buf[NextOut];
			printf(" Consuming item: %d\n", fetchNum);

			//Lock
			pthread_mutex_lock(&lock);

			//Wait if needed
			while (counter == 0) {
				pthread_cond_wait(&full, &lock);
			}
			counter--;

           //Changing nextOut
			NextOut = (NextOut+ 1) % BSIZE;

			//Update final varibles
			if (fetchNum > totMax) {
				totMax = fetchNum;
			}

			if (fetchNum < totMin){
				totMin = fetchNum;
			}
			totSum = totSum + fetchNum;
			

			//Update local variables
			if (fetchNum > myMax) {
				myMax = fetchNum;
			}

			if (fetchNum < myMin) {
				myMin = fetchNum;
			}
			mySum = mySum + fetchNum;

			//unlock and signal
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&empty);
		
			cCounter++;
			
		}
		//Print values
	    printf("\n The minimum integer was: %d\n", myMin);
	    printf("\n The maximum integer was: %d\n", myMax);
		int myAvg = mySum / NUM_ITEMS;
		printf("\n The average is: %d\n", myAvg);

	printf("\n --- Consumer exiting ----\n");
	pthread_exit(0);
}


