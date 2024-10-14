#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif
// Compile with -DLEAK to leak memory
#ifndef LEAK
#define LEAK 0
#endif
#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)
int main(int argc, char **argv)
{

    // Test 1: Check basic malloc
    printf("Test 1: Basic malloc\n");
    char *ptr1 = malloc(100);
    if (ptr1 == NULL) 
    {
        fprintf(stderr, "Test 1 Failed\n");
        return 1;
    }
    strcpy(ptr1, "Passed Test Case 1");
    printf("Allocated and wrote to memory: %s\n", ptr1);
    free(ptr1);
    //WORKS

    // Test 2: Check basic free
    printf("\nTest 2: Basic free\n");
    char *ptr2 = malloc(50);
    if (ptr2 == NULL) 
    {
        fprintf(stderr, "Test 2 Failed\n");
        return 1;
    }
    free(ptr2);
    printf("Memory freed successfully\n");
    //WORKS

    // Test 3: malloc(0)
    printf("\nTest 3: malloc(0)\n");
    char *ptr3 = malloc(0);
    if (ptr3 != NULL)
    {
        fprintf(stderr, "Test 3 Failed\n");
        free(ptr3); // Free if allocator returns a non-NULL pointer
    } else {
        printf("malloc(0) correctly returned NULL\n");
    }
    //WORKS

    // Test 4: Multiple of 8 - Byte alignment
    printf("\nTest 4: Byte alignment\n");
    size_t sizes[] = {1, 7, 8, 9, 15, 16};
    for (int i = 0; i < 6; i++) 
    {
        char *ptr = malloc(sizes[i]);
        if (ptr == NULL) 
        {
            fprintf(stderr, "Test 4 Failed: malloc returned NULL\n");
            return 1;
        }
        // Check if the returned pointer is aligned to 8 bytes
        if ((uintptr_t)ptr % 8 != 0) 
        {
            fprintf(stderr, "Test 4 Failed: Pointer not aligned to 8 bytes\n");
            free(ptr);
            return 1;
        }
        free(ptr);
    }
    printf("All pointers are correctly aligned to 8 bytes\n");
    //WORKS
    // Test 5: Call free for non-malloc address
    printf("\nTest 5: Free non-malloc address\n");
    int non_malloc_var = 42;
    free(&non_malloc_var); // Should produce an error message
    //WORKS

    // Test 6: Call free on address in middle of chunk
    printf("\nTest 6: Free address in middle of chunk\n");
    char *ptr6 = malloc(100);
    if (ptr6 == NULL) 
    {
        fprintf(stderr, "Test 6 Failed\n");
        return 1;
    }
    free(ptr6 + 10); //Should produce an error message
    free(ptr6);      //Correctly free the allocated memory
    //WORKS

    // Test 7: Double free
    printf("\nTest 7: Double free\n");
    char *ptr7 = malloc(50);
    if (ptr7 == NULL) 
    {
        fprintf(stderr, "Test 7 Failed\n");
        return 1;
    }
    free(ptr7);
    free(ptr7); //Should produce an error message
    //WORKS

    // Test 8: Giving something too big - malloc(4097)
    printf("\nTest 8: Malloc too big (4097 bytes)\n");
    char *ptr8 = malloc(4097);
    if (ptr8 != NULL) 
    {
        fprintf(stderr, "Test 8 Failed: malloc(4097) should return NULL\n");
        free(ptr8); // Free if allocator returns a non-NULL pointer
    } 
    else 
    {
        printf("malloc(4097) correctly returned NULL\n");
    }
    //WORKS
    // Test 9: No overwrite
    printf("\nTest 9: No overwrite\n");
    char *ptr9_a = malloc(50);
    char *ptr9_b = malloc(50);
    if (ptr9_a == NULL || ptr9_b == NULL) 
    {
        fprintf(stderr, "Test 9 Failed\n");
        return 1;
    }
    memset(ptr9_a, 'A', 50);
    memset(ptr9_b, 'B', 50);
    for (int i = 0; i < 50; i++) 
    {
        if (ptr9_a[i] != 'A') 
        {
            fprintf(stderr, "Test 9 Failed: Memory overwrite detected in ptr9_a\n");
            free(ptr9_a);
            free(ptr9_b);
            return 1;
        }
        if (ptr9_b[i] != 'B') 
        {
            fprintf(stderr, "Test 9 Failed: Memory overwrite detected in ptr9_b\n");
            free(ptr9_a);
            free(ptr9_b);
            return 1;
        }
    }
    printf("No memory overwrite detected\n");
    free(ptr9_a);
    free(ptr9_b);
    //WORKS
    // Test 10: No overlap
    printf("\nTest 10: No overlap\n");
    char *ptr10_a = malloc(100);
    char *ptr10_b = malloc(100);
    if (ptr10_a == NULL || ptr10_b == NULL) 
    {
        fprintf(stderr, "Test 10 Failed: malloc returned NULL\n");
        return 1;
    }
    if (ptr10_a + 100 > ptr10_b) 
    {
        fprintf(stderr, "Test 10 Failed: Memory regions overlap\n");
        free(ptr10_a);
        free(ptr10_b);
        return 1;
    }
    printf("Memory regions do not overlap\n");
    free(ptr10_a);
    free(ptr10_b);
    //WORKS
    // Test 11: Leakage
    printf("\nTest 11: Leakage test\n");
    //DO NOT FREE - Testing leak checker
    char *ptr11 = malloc(200);
    if (ptr11 == NULL) 
    {
        fprintf(stderr, "Test 11 Failed: malloc returned NULL\n");
        return 1;
    }
    // Do not free ptr11 to test leakage
    // The leak checker should report 200 bytes leaked
    //WORKS

    // Test 12: Coalescing
    printf("\nTest 12: Coalescing\n");
    char *ptr12_a = malloc(100);
    char *ptr12_b = malloc(100);
    char *ptr12_c = malloc(100);
    if (ptr12_a == NULL || ptr12_b == NULL || ptr12_c == NULL) {
        fprintf(stderr, "Test 12 Failed: malloc returned NULL\n");
        return 1;
    }
    free(ptr12_a);
    free(ptr12_c);
    // Now free ptr13_b to trigger coalescing of adjacent free blocks
    free(ptr12_b);
    // Attempt to allocate a larger block that requires coalesced space
    char *ptr12_d = malloc(300);
    if (ptr12_d == NULL)
    {
        fprintf(stderr, "Test 12 Failed\n");
        return 1;
    } 
    else 
    {
        printf("Successfully allocated 300 bytes after coalescing\n");
        free(ptr12_d);
    }

    // End of tests
    printf("\nmemtest completed.\n");
    //WORKS!!
    return 0;
}