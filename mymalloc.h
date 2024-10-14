#include <sys/types.h>

void initialize_heap();
void *mymalloc(size_t size, char *file, const int lineNum);
void myfree(void *vpointer, char *file, const int lineNum);

#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)
