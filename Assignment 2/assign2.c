/*
 * assign2.c
 *
 * Name: Kutay Cinar
 * Student Number: V00******
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "train.h"

/*
 * If you uncomment the following line, some debugging
 * output will be produced.
 *
 * Be sure to comment this line out again before you submit 
 */

/* #define DEBUG	1 */

void ArriveBridge (TrainInfo *train);
void CrossBridge (TrainInfo *train);
void LeaveBridge (TrainInfo *train);

// Queue structure with head and size
struct queue
{
	TrainInfo** head;
	int size;
};

// Queue variables for east and west
struct queue eastQueue;
struct queue westQueue;

// Mutex variables
pthread_mutex_t bridge;
pthread_mutex_t queue;

// Condition variable
pthread_cond_t nextTrain;

// Consecative east train count variable
volatile int conEastCount;

/*
 * This function is started for each thread created by the
 * main thread.  Each thread is given a TrainInfo structure
 * that specifies information about the train the individual 
 * thread is supposed to simulate.
 */
void * Train ( void *arguments )
{
	TrainInfo	*train = (TrainInfo *)arguments;

	/* Sleep to simulate different arrival times */
	usleep (train->length*SLEEP_MULTIPLE);

	ArriveBridge (train);
	CrossBridge  (train);
	LeaveBridge  (train); 

	/* I decided that the paramter structure would be malloc'd 
	 * in the main thread, but the individual threads are responsible
	 * for freeing the memory.
	 *
	 * This way I didn't have to keep an array of parameter pointers
	 * in the main thread.
	 */
	free (train);
	return NULL;
}

/*
 * You will need to add code to this function to ensure that
 * the trains cross the bridge in the correct order.
 */
void ArriveBridge ( TrainInfo *train )
{
	printf ("Train %2d arrives going %s\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	/* Your code here... */

	// Queue is locked for enqueuing
	pthread_mutex_lock(&queue);

	// Add the arrived train to the corresponding queue on its direction
	if (train->direction == DIRECTION_WEST) {

		// Check if westbound train queue is empty
		if(westQueue.size == 0)
		{
			conEastCount = 0; // reset consecative train count back to 0
		}

		// West bound train added and size incremented
		westQueue.head[westQueue.size++] = train;

	}
	else if (train->direction == DIRECTION_EAST){

		// East bound train added and size incremented
		eastQueue.head[eastQueue.size++] = train;
	}

	// Queue is unlocked
	pthread_mutex_unlock(&queue);

	// Lock bridge mutex
	pthread_mutex_lock(&bridge);

	// If train is west bound
	if(train->direction == DIRECTION_WEST)
	{
		// Queue is locked in order to check conditions
		pthread_mutex_lock(&queue);

		// While loop to wait for next train condition
		while( !( (eastQueue.size == 0 || conEastCount == 2) && (train == westQueue.head[0])) )
		{
			// Queue is unlocked to allow new arrived trains to be added while waiting on next train call condition
			pthread_mutex_unlock(&queue);
			pthread_cond_wait(&nextTrain, &bridge);  // wait for next train call
			pthread_mutex_lock(&queue);
		}

		// Queue is unlocked
		pthread_mutex_unlock(&queue);

	}
	// If train is east bound
	else if (train->direction == DIRECTION_EAST)
	{
		// Queue is locked in order to check conditions
		pthread_mutex_lock(&queue);

		// While loop to wait for next train condition
		while( !( (westQueue.size == 0 || conEastCount < 2) && (train == eastQueue.head[0])) )
		{
			// Queue is unlocked to allow new arrived trains to be added while waiting on next train call condition
			pthread_mutex_unlock(&queue);
			pthread_cond_wait(&nextTrain, &bridge); // wait for next train call
			pthread_mutex_lock(&queue);
		}

		// Queue is unlocked
		pthread_mutex_unlock(&queue);

	}
}


/*
 * Simulate crossing the bridge.  You shouldn't have to change this
 * function.
 */
void CrossBridge ( TrainInfo *train )
{
	printf ("Train %2d is ON the bridge (%s)\n", train->trainId,
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
	
	/* 
	 * This sleep statement simulates the time it takes to 
	 * cross the bridge.  Longer trains take more time.
	 */
	usleep (train->length*SLEEP_MULTIPLE);

	printf ("Train %2d is OFF the bridge(%s)\n", train->trainId, 
			(train->direction == DIRECTION_WEST ? "West" : "East"));
	fflush(stdout);
}

/*
 * Add code here to make the bridge available to waiting
 * trains...
 */
void LeaveBridge ( TrainInfo *train )
{

	// Queue is locked for dequeue operation
	pthread_mutex_lock(&queue);

	// If west bound train arrives
	if (train->direction == DIRECTION_WEST)
	{
		// Dequeue the train from west queue
		for (int i = 0; i < westQueue.size-1; i++)
		{
			westQueue.head[i] = westQueue.head[i+1];
		}

		// Decerement west queue size
		westQueue.size--;

		// Set consecutive east bound trains to 0
		conEastCount = 0;
	}
	// If east bound train arrives
	else if (train->direction == DIRECTION_EAST)
	{
		// Dequeue the train from east queue
		for (int i = 0; i < eastQueue.size-1; i++)
		{
			eastQueue.head[i] = eastQueue.head[i+1];
		}

		// Decerement east queue size
		eastQueue.size--;

		// Increment consecutive east bound trains
		conEastCount++;
	}

	// Queue is unlocked
	pthread_mutex_unlock(&queue);

	// Signal next train to cross bridge
	pthread_cond_broadcast(&nextTrain);

	// Unlock bridge mutex
	pthread_mutex_unlock(&bridge);

}

int main ( int argc, char *argv[] )
{
	int		trainCount = 0;
	char 		*filename = NULL;
	pthread_t	*tids;
	int		i;

		
	/* Parse the arguments */
	if ( argc < 2 )
	{
		printf ("Usage: part1 n {filename}\n\t\tn is number of trains\n");
		printf ("\t\tfilename is input file to use (optional)\n");
		exit(0);
	}
	
	if ( argc >= 2 )
	{
		trainCount = atoi(argv[1]);
	}
	if ( argc == 3 )
	{
		filename = argv[2];
	}	
	
	initTrain(filename);

	// Initilize west queue
	westQueue.head = (TrainInfo **)malloc(sizeof(TrainInfo *)*trainCount);
	westQueue.size = 0;

	// Initilize east queue
	eastQueue.head = (TrainInfo **)malloc(sizeof(TrainInfo *)*trainCount);
	eastQueue.size = 0;

	// Initilize consecutive eastbound trains
	conEastCount = 0;

	if (pthread_mutex_init(&bridge, NULL) != 0){ //mutex initialization check
        printf("\n mutex init failed\n");
        return 1;
    }

	if (pthread_mutex_init(&queue, NULL) != 0){ //mutex initialization check
        printf("\n mutex init failed\n");
        return 1;
    }

    if (pthread_cond_init(&nextTrain, NULL) != 0) //cond initialization check
    {
        printf("\n condition nextTrain failed\n");
        return 1;
    }

	/*
	 * Since the number of trains to simulate is specified on the command
	 * line, we need to malloc space to store the thread ids of each train
	 * thread.
	 */
	tids = (pthread_t *) malloc(sizeof(pthread_t)*trainCount);
	
	/*
	 * Create all the train threads pass them the information about
	 * length and direction as a TrainInfo structure
	 */
	for (i=0;i<trainCount;i++)
	{
		TrainInfo *info = createTrain();
		
		printf ("Train %2d headed %s length is %d\n", info->trainId,
			(info->direction == DIRECTION_WEST ? "West" : "East"),
			info->length );

		if ( pthread_create (&tids[i],0, Train, (void *)info) != 0 )
		{
			printf ("Failed creation of Train.\n");
			exit(0);
		}
	}

	/*
	 * This code waits for all train threads to terminate
	 */
	for (i=0;i<trainCount;i++)
	{
		pthread_join (tids[i], NULL);
	}

	pthread_mutex_destroy(&bridge);
	pthread_mutex_destroy(&queue);
	pthread_cond_destroy(&nextTrain);
	
	free(tids);
	return 0;
}

