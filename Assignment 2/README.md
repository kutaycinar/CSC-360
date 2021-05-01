# Trains and Bridges (threads and semaphores)

This project assignment helps to learn to use three programming constructs provided by the POSIX pthread library: threads, mutexes and condition variables.

The goal is to construct a simulation of an automated control system for a single lane bridge. Threads are used to simulate trains approaching the bridge from two different 
directions. The software ensures that there is never more than one train on the bridge at any given time by the use of semaphores and mutexes.

## Trains

Each train will be simulated by a thread and the operation of each thread is defined by  
the function shown below.

```
void * Train ( void *arguments )
{
    TrainInfo *train = (TrainInfo *)arguments;
    
    /* Sleep for awhile to simulate different arrival times */
    usleep (train­>length*SLEEP_MULTIPLE);
    
    ArriveBridge (train);
    CrossBridge  (train);
    LeaveBridge  (train); 
    
    free (train);
    return NULL;
}
```
This program implements the functions ArriveBridge and LeaveBridge. The code for CrossBridge has been supplied.

## Program

The program accepts two parameters on the command line.  The first one is required, the second one is optional.

The first parameter is an integer, greater than 0, which is the number of trains to simulate.

The second parameter is optional: a filename to use as input data for the simulation. The format of the file is shown below.

## Input File Format

The input files have a simple format. Each line contains information about a single train. The files end with a blank line.

The first character on each line is one of the following four characters: 'e', 'E', 'w', or 'W'

The first two letters specify a train that is going East, the second two letters specify a train headed West.

Immediately following the letter specifying the direction is an integer that indicates the length of the train. There is no space between the direction character and the length.

The following file specifies three trains, two headed East and one headed West.
```
E10
w6
E3
```

Run program by providing one of the tests:

```./assign2 3 tests/t0.txt | grep OFF > o3.txt```
