
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "mymalloc.h"
//perform workload 50 times and report average time 
//use get time of day function to test runtime
//ONE: MALLOC AND FREE AN OBJECT 50 TIMES
#define RUNS 50
static int firstTest()
{
    for(int i = 0; i < 50; i++)
    {
        void* ptr = malloc(1);
        //allocate one byte of data to void pointer 
        if(ptr == NULL)
        {
            //something went wrong
            return -1;
        }
        free(ptr);
    }

    return 0;
    //returns 0 on success
}

//TWO: STORE 120 1 BYTE POINTERS IN ARRAY AND FREE
static int secondTest()
{
    //define ptr with size 120
    char* arr[120] = {NULL};
    //had to fill array with NULL because while condition wont work
    for(int i = 0; i < 120; i++)
    {
        //call to malloc before checking if NULL
        arr[i] = malloc(1);
        //malloc 1 byte to arr loc
        //arr[i] becomes a void pointer 
        if(arr[i] == NULL)
        {
            //this case should not run 
            //if somehow this fails for some value in arr then prev values should be freed
            for(int j = 0; j < i; j++)
            {
                free(arr[j]);
            }
            return -1;
        }
    }
    //if all successes then free everything

    for(int k = 0; k < 120; k++)
    {
        free(arr[k]);
    }

    return 0; //on success
    //int iterate = 0;
    //while(arr[iterate] == NULL)
    //{

    //}

}
//THREE: 120 TIMES CHOOSE BETWEEN MALLOC OR FREE
static int thirdTest()
{
    char *blocks[120] = {NULL};
    int allocated = 0; //Number of currently allocated blocks
    int remainingMallocs = 120;
    int totalOperations = 240;

    for (int i = 0; i < totalOperations; i++)
    {
        int doMalloc;

        if (allocated == 0)
        {
            doMalloc = 1; //Must allocate since nothing to free
        }
        else if (remainingMallocs == 0)
        {
            doMalloc = 0; //Must free since no remaining mallocs
        }
        else
        {
            doMalloc = rand() % 2; //Randomly choose between malloc and free
        }

        if (doMalloc)
        {
            //Allocate memory
            //Find the first available slot
            int index = 0;
            while (index < 120 && blocks[index] != NULL)
                index++;
            if (index == 120)
            {
                fprintf(stderr, "Allocation error: No available slots.\n");
                return -1;
            }
            blocks[index] = malloc(1);
            if (blocks[index] == NULL)
            {
                //Handle allocation failure
                //Free any allocated blocks before returning
                for (int j = 0; j < 120; j++)
                {
                    if (blocks[j] != NULL)
                        free(blocks[j]);
                }
                return -1;
            }
            allocated++;
            remainingMallocs--;
        }
        else
        {
            //Free memory
            //Find a random allocated block to free
            int index = rand() % 120;
            while (blocks[index] == NULL)
            {
                index = rand() % 120;
            }
            free(blocks[index]);
            blocks[index] = NULL;
            allocated--;
        }
    }

    //Free any remaining allocated blocks
    for (int j = 0; j < 120; j++)
    {
        if (blocks[j] != NULL)
            free(blocks[j]);
    }

    return 0;
}

//allocate a random sized data request between 1 - 10 bytes, store it in the array and deallocate it immideatly
//repeat 120 times
//FOUR: 120 TIMES RANDOMLY ALLOCATE AND DEALLOCATE RANDOMIZED SIZES ( 1 - 10 )
static int fourthTest() 
{
    char* ptr = NULL;

    for (int i = 0; i < 240; i++) 
    {
        int allocSize = (rand() % 10) + 1; 
        //ranodmly allocate between 1 - 10
        ptr = malloc(allocSize);
        if (ptr == NULL) 
        {   
            //check if malloc fails
            return -1;
        }
        //free allocated immediatly 
        free(ptr);                         
    }

    return 0; 
}

//allocate increasing size of memory from 1 - 40, and then free the entire array
//FIVE: ALLOCATE INCREASING SIZE OF MEMORY, THEN FREE

static int fifthTest()
{
    char* arr[40] = {NULL};

    //Allocate increasing sizes from 1 to 40 bytes
    for (int i = 0; i < 40; i++)
    {
        arr[i] = malloc(i + 1);
        if (arr[i] == NULL)
        {
            //Free any previously allocated blocks if malloc fails
            for (int j = 0; j < i; j++)
            {
                free(arr[j]);
            }
            return -1;
        }
    }

    //Free all allocated memory
    for (int i = 0; i < 40; i++)
    {
        free(arr[i]);
    }

    return 0;
}

#define REPEAT_COUNT 50

int main(void) 
{
    initialize_heap(); //Call custom memory initializer
    struct timeval start_time, end_time;
    srand(getpid()); //Seed for random functions

    //Run each test case REPEAT_COUNT times
    gettimeofday(&start_time, NULL);
    for (int run = 0; run < REPEAT_COUNT; run++) {
        for (int i = 0; i < 50; i++) {
        firstTest();
        secondTest();
        thirdTest();
        fourthTest();
        fifthTest();
    }

    gettimeofday(&end_time, NULL);
    double elapsed = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed += (end_time.tv_usec - start_time.tv_usec) / 1000.0;
    printf("Average time: %f ms\n", elapsed / 50);


    return 0;
}
}


