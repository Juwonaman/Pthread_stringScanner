#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 10240
#define NUM_THREADS 10

int n1,n2;
char *s1,*s2;
FILE *fp;
int countArray[NUM_THREADS]={0};
pthread_mutex_t mutex;

//read input file and generate string s1/s2 and length n1/n2
int readf(FILE *file) {
    if ((file = fopen("strings.txt", "r")) == NULL) {
        printf("ERROR: can't open strings.txt!\n");
        return 0;
    }
    s1 = (char *) malloc(sizeof(char) * MAX);
    if (s1 == NULL) {
        printf("ERROR: Out of memory!\n");
        return -1;
    }
    s2 = (char *) malloc(sizeof(char) * MAX);
    if (s1 == NULL) {

    }
    /*read s1 s2 from the file*/
    s1 = fgets(s1, MAX, file);
    s2 = fgets(s2, MAX, file);
    n1 = strlen(s1);  /*length of s1*/
    n2 = strlen(s2) - 1; /*length of s2*/

    if (s1 == NULL || s2 == NULL || n1 < n2) { /*when error exit*/
        return -1;
    }

    return 0;
}

int num_substring(int t)
{
    int partSize = n1 / NUM_THREADS;
    int count = 0;
    int start = t * partSize;
    int end = (t + 1) * partSize;

    int i,j,k;
    int partialTotal =0;

    if (t == (NUM_THREADS - 1)) {
        end = n1;
    }
    else {
        end = end + (n2 -1);
    }

    for (i = start; i <= (n1-n2) && i < end; i++){
        count=0;
        for(j = i,k = 0; k < n2; j++,k++){  /*search for the next string of size of n2*/
            if (*(s1+j)!=*(s2+k)){
                break;
            }else{
                count++;
            }

            if(count==n2){
                partialTotal++;		/*find a substring in this step*/
            }
        }
    }

    return partialTotal;
}

void *calSubStringThread(void *threadid){
    long tid = (long) threadid;
    printf("This is thread %ld, ", tid);

    pthread_mutex_lock(&mutex);
    int num = num_substring(tid);
    pthread_mutex_unlock(&mutex);

    printf("find num of is: %d\n", num);
    countArray[tid] = num; // Store the number in an array
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int t, rc;
    int totalNum = 0;
    pthread_mutex_init(&mutex, NULL);

    if (readf(fp) != 0) {
        return -1;
    }

    for(t=0; t<NUM_THREADS; t++){
        rc = pthread_create(&threads[t], NULL, calSubStringThread, (void *) (size_t) t);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for(t=0; t<NUM_THREADS; t++){
        pthread_join(threads[t], NULL);
    }

    for (t = 0; t < NUM_THREADS; t++) {
        totalNum += countArray[t];
    }

    printf("The number of substrings is: %d\n", totalNum);
    pthread_mutex_destroy(&mutex);
    return 1;
}