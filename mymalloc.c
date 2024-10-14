#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "mymalloc.h"

#define MEMSIZE 4096

static union{
    char bytes[MEMSIZE];
    double not_used;
}heap;

static bool initialized; //HEAP

typedef struct metaNode {
    int valid; //valid bit
    int payload; //size of payload
    //size of payload + 4(valid bit) +4(size) = size of chunk
}metaNode; //metadata
//address of struct is the same as address of allocated (first variable)

metaNode* first_meta;
void* last;

//print stderr for file prints to represent errors 
//typedef struct chunks{

//metaNode* meta_data;
  //  metaNode* next;

//}chunks;

//chunks* first_chunk;

static int convertTo8(int num)
{
    if(num % 8 == 0)
    {
        return num;
    }
    int temp = num/8;
    return (temp + 1) * 8;
}

void coaleceChunks()
{
    char* memory = heap.bytes;
    metaNode* curr = first_meta;

    while((void*) memory < last)
    {
        if(curr->valid == 0)
        {
            char* next_memory = memory + sizeof(metaNode) + curr->payload;
            if((void*) next_memory >= last)
                break;

            metaNode* next = (metaNode*) next_memory;

            while( (void*) next_memory < last && next->valid == 0)
            {
                curr->payload += sizeof(metaNode) + next->payload;
                next_memory = next_memory + sizeof(metaNode) + next->payload;
                if((void*) next_memory >= last)
                    break;
                next = (metaNode*) next_memory;
            }
        }
        memory += sizeof(metaNode) + curr->payload;
        curr = (metaNode*) memory;
    }
}

static void leak_checker()
{
    char* memory = heap.bytes;
    //iterate to the end to figure out if there r any nodes that are filled left in the heap
    metaNode* curr = first_meta;
    int counter = 0;
    int numChunks = 0;
    //counts the number of chunks that are still allocated 
    void* addy = (void*) memory;
    while(addy < last)
    {
        if(curr->valid == 1)
        {
            counter += curr->payload;
            numChunks++;
        }
        memory+=sizeof(metaNode) + curr->payload;
        curr = (metaNode*) memory;
        addy = (void*) memory;
        
    }

    if(counter != 0)
    {
        fprintf(stderr, "leak_checker: %d bytes leaked in %d objects\n" ,counter, numChunks);
        return;
    }
}

void initialize_heap()
{

    first_meta = (metaNode*) heap.bytes;
    first_meta->payload = MEMSIZE - sizeof(metaNode);
    first_meta->valid = 0;
    last = (void*) heap.bytes + MEMSIZE;
    initialized = true;


    //first_chunk->meta_data = first_meta;
    //first_chunk->next = NULL;

    atexit(leak_checker);
    /*initialized = true;
    first_meta = (metaNode*) heap.bytes;
    //create a pointer that stores the address of the start of the heap but typecasts into a node pointer
    //because we typecast into a metaNode... the first 8 bytes have been reserved for the metadata
    first_meta->payload = MEMSIZE - sizeof(metaNode);
    int offset = first_meta->payload;
    first_meta->valid = 0;
    //essentially setting the offset to the rest of the size of 
    last_meta = (metaNode*) (heap.bytes + offset);
    last_meta->valid = 0;
    last_meta->payload = 0;
    //atexit(leak_checker)
    chunks *first;
    first->meta_data = first_meta;
    chunks *second;
    second->meta_data = last_meta;
    second->next = NULL;
    first->next = second;
    //created a linkedlist that allows for easy access to metadata, initalized first andf last as start and end */
}


int getAllocatedMemory(metaNode* target)
{

    metaNode* ptr = first_meta;
    //int counter = 0;
    char* memory = heap.bytes;
    while(ptr!=target)
    {
        //counter += sizeof(metaNode) + ptr->payload;
        memory += sizeof(metaNode) + ptr->payload;
        ptr = (metaNode*) memory;
    }

    return memory - heap.bytes;
    //returns the difference between the start of target and the begginging of the heap

    //get all the memory allocated before the newly created chunk
    /*chunks* ptr = first_chunk;
    int counter = 0;
    while(ptr!=chunky)
    {
        counter += ptr->meta_data->payload + sizeof(metaNode);
        //add the size of the chunk to the counter of total allocated memory 
        ptr = ptr->next;
    }

    return counter;*/
}

void *mymalloc(size_t bytes, char *filename, int linenum) //return void pointer 
{
    if(!initialized) initialize_heap();

    if(bytes <= 0)
    {
        fprintf(stderr, "%s:%d:malloc: invalid byte request\n", filename, linenum);
        return NULL;
    }
    int scaledBytes = convertTo8(bytes);
    

    if(scaledBytes > MEMSIZE - sizeof(metaNode))
    {
        fprintf(stderr, "%s:%d:malloc: requested too many bytes\n", filename, linenum);
        return NULL;
    }

    metaNode* curr = first_meta;
    char* memory = heap.bytes;
    while((void*) memory < last)
    {
        if(curr->valid == 0 && curr->payload >= scaledBytes)
        {
            //return
            int old = curr->payload;
            if (old - scaledBytes >= sizeof(metaNode) + 8) 
            {
                // Split the block
                curr->payload = scaledBytes;
                curr->valid = 1;

                // Calculate the address of the next metaNode
                char* next_address = (char*) curr + sizeof(metaNode) + scaledBytes;
                metaNode* next = (metaNode*) next_address;
                next->valid = 0;
                next->payload = old - scaledBytes - sizeof(metaNode);
            } 
            else 
            {
                // Allocate the entire block
                curr->valid = 1;
            // No need to adjust curr->payload; it remains as is
            }

            // Return pointer to the payload area
            return (void*) ((char*) curr + sizeof(metaNode));
        }
    //curr node is filled
    memory += curr->payload + sizeof(metaNode);
    //iterate memory to where next address should be 
    curr = (metaNode*) memory;
    }
    //PRINT NOT ENOUGH SPACE
    fprintf(stderr, "%s:%d:malloc: Unable to allocate %d bytes\n", filename, linenum, scaledBytes);
    return NULL;

    //if exits out the loop then there is no freespace

    /*if(!initialized) initialize_heap();
    int scaledBytes = convertTo8(bytes);
    //FInd first instance of block that has enough storage to store size_t bytes
    chunks* ptr = first_chunk;
    while(ptr!=NULL)
    {
        //iterate through all chunks
        metaNode* curr = ptr->meta_data;
        if(curr->payload > scaledBytes && curr->valid == 0)
        {
            //empty and has enough size
            chunks* chunky;
            metaNode* node;
            node->valid = 0;
            chunky->meta_data = node;
            chunky->next=ptr->next;
            ptr->next = chunky;
            curr->payload = scaledBytes;
            curr->valid = 1;
            //create new MetaNode for the next location of freecell
            int memoryTil = getAllocatedMemory(chunky) + sizeof(metaNode);
            
            if(chunky->next!=NULL)
            {
                //post free condition 
                //basically if something was freed then its something was allocated but not to capacity...it doesnt extend til end of heap so must declare accordingly 
                node->payload = curr->payload - scaledBytes - sizeof(metaNode) - sizeof(metaNode);
            }
            else
            {
                //otherwise not a concern assume until end of heap
                node->payload = MEMSIZE - memoryTil;
            }
            //everything updated now return address of right after new metadata in chunky
            return (void*) heap.bytes + memoryTil;
        }
        ptr = ptr->next;
    }
    */
    //after heap is inialized we search through the chunks to find the first one that is empty/ has enough to support byte request
    //create new chunk and metanode that represents the next chunk
    //this metadata needs to contain a payload of the remaining free space
    /*if (!initialized) initialize_heap(); 
    //After heap is initialized, we should search for headers that have a valid bit of 0 (meaning they are empty
    //We can compare the size_t bytes to the size of the free space in the metadata
    //if the freespace is larger, we can set the current header to the header of the memory we want to allocate
    //then we can create a pointer to the data that we want at location of current meta data + size of the payload
    //if freespace in the inital metadata doesnt have enough size then we can check for the next free metadata
    //maybe have to create a linkedlist of nodes that have metadata objects in them
    chunks *currChunk = first_chunk;
    chunks *prevChunk = NULL;
    int freespace = MEMSIZE;
    int scaled_bytes = convertTo8(bytes);
    while(currChunk->meta_data != NULL)
    {
        //loop through the chunks
        //neec to meet 2 conditions, valid = 0 and 
        if(currChunk->meta_data->valid == 0)
        {
            //check for if size_t even fits
            if(currChunk->meta_data->payload - bytes > 0)
            {
                //both conditions have been met 
                currChunk->meta_data->valid = 1;
                currChunk->meta_data->payload = scaled_bytes;
                //set metadata to full and size to requested size
                //curr chunk needs 
                metaNode* shift = (metaNode*) (&(currChunk->meta_data) + (scaled_bytes + sizeof(first_meta)));
                shift->valid = 0;
                shift->payload = MEMSIZE - scaled_bytes + sizeof(currChunk->meta_data); //subtracting total space from space that comes after 
                chunks temp;
                temp.meta_data = shift;
                temp.next = currChunk->next;
                //inserting new chunk between previous and old next chunk
                currChunk->next = &temp;
                return (void*) temp.meta_data + sizeof(temp.meta_data); //return address to the start of the allocated memory typecasted to void 
            }


        }     
    }*/
    
}

bool is_valid_pointer(void* ptr)
{
    if (ptr == NULL)
        return false;

    // Assuming heap.bytes is your heap start and last is the end
    char* heap_start = heap.bytes;
    char* heap_end = heap.bytes + MEMSIZE; // MEMSIZE is the total heap size

    if ((char*)ptr < heap_start || (char*)ptr >= heap_end)
        return false;

    // Traverse the heap to find the block
    char* current = heap_start;

    while (current < heap_end)
    {
        metaNode* meta = (metaNode*)current;
        char* payload = current + sizeof(metaNode);

        if (payload == ptr)
        {
            return meta->valid == 1;
        }

        current += sizeof(metaNode) + meta->payload;
    }

    return false;
}

void myfree(void *ptr, char *filename, int linenum) 
{
    //check if it has been malloced before 
    if(!is_valid_pointer(ptr))
    {
        fprintf(stderr, "%s:%d:free: pointer not allocated by mymalloc\n", filename, linenum);
        return;
    }
    //we have a void pointer that points to the start of the payload that we wantto free
    //we need to somehow get back to the start of the meta_data of that pointer
     void* addy = (void*) heap.bytes;
     if(ptr >= last || ptr < addy)
    {
        fprintf(stderr, "%s:%d:free: attempting to free outside the heap\n", filename, linenum);
        return;
    }

    if(ptr == NULL)
    {
        fprintf(stderr, "%s:%d:free: pointer is NULL\n", filename, linenum);
        return;
    }

    char* memory = ptr - sizeof(metaNode); //goes back to the start of the metaNode that has the data for the current block 
    metaNode* curr = (metaNode*) memory;
    
    if(curr->valid == 0)
    {
        fprintf(stderr, "%s:%d:free: chunk is already empty\n", filename, linenum);
        return;
    }
   
    //gets the metaNode at that address
    //payload stays the same but the valid bit becomes 0
    curr->valid = 0;
    //block has been freed
    //now time to coalece 
    coaleceChunks();

}


//large chunks can split into small chunks depending on the size called
//smaller consecutive freed chunks can combine into a larger chunk

//header of the chunk will contain size & valid bit (metadata)
//the payload contains the data
//[(in use true/false) -> (size of chunk) -> (value in chunk)] all data points to each other but techinally same node "flattened linked list"

//on success the malloc method will return a pointer to the usable portion of a chunk with at least the size of the requested bits 

//A chunk that has already been allocated will not be overided, changed, or moved until it has been freed

//client code cannot access anymore than what its given

//When malloc finds a chunk that meets its requirement, it splits it into the size of the chunk that it needs, and leaves rest of the chunk 
//Situation where split doesnt happen - if taking the position of a previously split or freed chunk... there is a chance there is no space to split
//Second chunk will always remain free

//When splitting chunks by allocation, test to see if the chunk to the right of the second half of the split is free and merge... This should be done in free but can be done in malloc as well... lowkey make a new function to merge 
//Assume largest data type has 8-byte allignment 
//Smallest possible chunk size is 16 bits

//The pointer that malloc returns must point to the payload... if I am flattenting the linked list I should return either the valid bit Node or the Size of payload Node (probably second)
//malloc and free should have an initial check to see if heap is initialized ... permitted to create a single static variable that tells us if the heap is intialized 
//malloc should return NUll if there is no large enough chunk to fulfil the request 

//ERRORS
// - FREE()
// if try to free on address no from malloc then error (probably need test case to determine we are at chunksize Node and that next is payload)
// if free is not at an adress of a start of a chunk
//if free is called on the same thing twice 
//Should return exit(2) error
//to note... exit after free method will be something to look out for 

//Should be an at exit leak detection code (dont call exit())

//At the end need performance testing -- do this AFTER coding

