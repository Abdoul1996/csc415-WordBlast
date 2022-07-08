/**************************************************************
* Class:  CSC-415-04 Summer 2022
* Name:Abdoulfatah Abdillahi
* Student ID:921623492
* GitHub ID: Abdoul1996
* Project: Assignment 4 – Word Blast
*
* File: <abdillahi_abdoulfatah_HW4_main.c>
*
* Description: This program is to read a file and count and tally each of the words that are 6 or more 
*                characters long. The main of this assignments was to familiarize with threads.
*
**************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define MAX_SIZE  10000
#define WORD_SIZE  10


// You may find this Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

int indexArrays = 0;                                // This integer will keep track of the wordsArray size 
int fd;                                            // file descriptor 
size_t chunk;                                     // this variable will store size that is equal to fileSize/number of threads 
size_t fileSize;                                 // stores the total file 
int initArray = 0;
int remainder = 0;
pthread_mutex_t mutex;                        // initializing a mutex lock variable 

                                            
                                            // Data structure to contain words and word frequency
typedef struct word
{
    char *word;
    int frequency;
} word;
struct word wordsArray[MAX_SIZE];                 // Global array 


                                                  // This function will be passed into thread is created and will scan through each chunk of the file
void *runThread(void *ptr) {  
    int inc;
    char * buffer;                                                        
    buffer = malloc(chunk+remainder);               //Each thread would allocated buffer of size chunk+remainder

                                                     // Read from a file desription 
    read(fd, buffer, chunk+remainder);

    char * token;                                                     // Tokenize with strtok_r and loop through buffer
    while (token = strtok_r(buffer, delim, &buffer)) {       
                                                            // Check if token is greater or equal 6 characters
        if ((int)strlen(token) >= 6) {   
            
            for (int i = 0; i < MAX_SIZE; i++) {
                inc = strcasecmp(wordsArray[i].word, token);
                if (inc == 0) {
                    pthread_mutex_lock(&mutex);             // Start critical section
                    wordsArray[i].frequency++;
                    pthread_mutex_unlock(&mutex);            // End critical section
                    break;
                } 
            }
                                                             // If token not in wordArray, add and increase count
            if (inc != 0) {
                if (indexArrays < MAX_SIZE) {
                    pthread_mutex_lock(&mutex);               // Start critical section
                    strcpy(wordsArray[indexArrays].word, token);
                    wordsArray[indexArrays].frequency++;
                    pthread_mutex_unlock(&mutex);               // End critical section
                    indexArrays++;
                }
            }
        }
    }
}

void initWordArray(){
    for(int i=0; i< MAX_SIZE; i++){
        wordsArray[i].word = malloc(WORD_SIZE);
        wordsArray[i].frequency = 0;
    }
   initArray = 1;
}


int main (int argc, char *argv[])
    {
    //***TO DO***  Look at arguments, open file, divide by threads
    //             Allocate and Initialize and storage structures

    int fileSize;
    int numberofThreads = strtol(argv[2], NULL, 10);

    
    

    if(initArray == 0){
        initWordArray();
    }

    if( pthread_mutex_init(&mutex, NULL)){
        printf("Mutex initialized is failed because file or number of threads was not specified[%d]\n", ( pthread_mutex_init(&mutex, NULL)));
        exit(0);
    }

    fd = open(argv[1], O_RDONLY);                               // open file 

    fileSize = lseek(fd, 0, SEEK_END);                          // get filesize using lseek

    lseek(fd, 0, SEEK_SET);                                     // set file position back to beginning 

    chunk = fileSize /numberofThreads;

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish

    pthread_t thread[numberofThreads];                  // Declare thread

    for(int i=0; i<numberofThreads; i++){
        if(i == numberofThreads -1){
            remainder = fileSize % numberofThreads;
        }

        if( pthread_create(&thread[i], NULL, runThread, (void*) &i)){
            printf("Thread creation failed [%d]\n", pthread_create(&thread[i], NULL, runThread, (void*) &i));
            exit(EXIT_FAILURE);
        }
    }

                                                     // waiting for each thread to finish before the program 
    for(int i=0; i < numberofThreads; i++){
        pthread_join(thread[i], NULL);
    }

                                                //This loop is sorting items by having the top 10 words at the end of the array
    word temp;
    for(int i=0; i <MAX_SIZE; i++){
        for(int j =i+1; j <MAX_SIZE; j++){
            if(wordsArray[i].frequency < wordsArray[j].frequency){
                temp = wordsArray[i];
                wordsArray[i] = wordsArray[j];
                wordsArray[j] = temp;
            }
        }
    }


    // ***TO DO *** Process TOP 10 and display

//quicksortMethod(wordsArray,0, indexArrays);           // Sort method


    printf("\n\n");
    printf("Word frency count on %s with %d threads\n", argv[1], numberofThreads);
    printf("printing top 10 words 6 character or more\n");

    for(int i=0; i<10; i++){
        printf("NUmber %d is %s with a count of %d\n", i+1, wordsArray[i].word, wordsArray[i].frequency);
    }

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************
    // ***TO DO *** cleanup

    close(fd);

    for(int i=0; i< MAX_SIZE; i++){
        free(wordsArray[i].word);
    }

 return 0;
    }
