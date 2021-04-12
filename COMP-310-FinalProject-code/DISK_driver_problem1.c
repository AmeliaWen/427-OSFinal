//    Final Project COMP 310
//    Your Name:
//    Your McGill ID: 
//
//    You need to fill in this file for the first problem
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
int open_EXAM (int ind, char* filename){
    printf("open: %d, %s\n", ind, filename);
    // Checking if there is a free block in the data section to create a new file
    if(currentPartition.last_free_block > currentPartition.total_blocks)
        return -1;
    
    // Lookup on the FAT
    int FATindex = 0;
    for (; FATindex < MAX_FILES; FATindex++)
        if (fat[FATindex].filename == NULL || strcmp(fat[FATindex].filename, filename) == 0)
            break;
    
    // Checking if the file is not already open
    int i = 0;
    for (; i < MAX_OPENFILES; i++)
        if(active_file_table_owners[i] == FATindex){
            return FATindex;
    
        }

    if (ind < 0 || ind > MAX_FILES){
        printf("ERROR: not valid index number\n");
        return -1;
    }
    // Opening the partition file
    if (active_file_table[ind]!= NULL){
        printf("ERROR: Index number in use.\n");
        return -1;
    }
    int activeFileIndex = ind;
    /*for(; activeFileIndex < MAX_OPENFILES; activeFileIndex++)
        if(active_file_table[activeFileIndex] == NULL)
            break;
    if(activeFileIndex == MAX_OPENFILES)
        return -1; // Too many files open at the same time
        */

    active_file_table[activeFileIndex]  = fopen(filename, "r+");
    active_file_table_owners[activeFileIndex] = FATindex;
    
    if(FATindex == MAX_FILES && fat[MAX_FILES -1].filename != NULL){
        // File DNE on the FAT and FAT is full
        fclose(active_file_table[activeFileIndex]);
        active_file_table[activeFileIndex] = NULL;
        active_file_table_owners[activeFileIndex] = -1;
        
        return -1;
    }
    else if(fat[FATindex].filename == NULL){
        // Make a new file
        fat[FATindex].filename = malloc(sizeof(500));
        strcpy(fat[FATindex].filename, filename);
        fat[FATindex].current_location = 0;
        fat[FATindex].file_length = 0;
        
        // READ:
        // ALL Seeking is done when doing IO, since the file system is NOT contigous and files do not have a pre-allocated size.
        // Space in the data section will be allocated ON WRITE, so we are done here
        
        // Seeking to the data section
        //fseekToDataSection(active_file_table[activeFileIndex]);
        // Seeking to first block
    }
    else{
        // Open a current file
        // Data structures already initialized
        fat[FATindex].current_location = 0;
        
        // READ:
        // ALL Seeking is done when doing IO, since the file system is NOT contigous and files do not have a pre-allocated size.
        
        // Seeking to the data section
        //fseekToDataSection(active_file_table[activeFileIndex]);
        // Seeking to first block
        
    }
    
    return 0;
}

char *read_EXAM(int file){

   // for (int i = 0 ; i < )
    
    if(file < 0)
        return NULL; // File open opreation probably failed before this call. We should never get here.

    if(fat[file].current_location >= MAX_BLOCKS)
        return NULL;
    
    // Lookup the block to read from
    int blockToRead = fat[file].blockPtrs[fat[file].current_location];
    
    // Seeing the block is valid
    if(blockToRead == -1)
        return NULL;
        
    // Get the file pointer
    FILE *filePtr = NULL;
    int i = 0;
    for (; i < MAX_OPENFILES; i++)
        if(active_file_table_owners[i] == file){
                filePtr = active_file_table[i];
            break;
        }
    if(i == MAX_OPENFILES)
        return NULL; // File was not open before this call
    
    // Seek to the block
    fseekToBlock(filePtr, blockToRead);
    

    // Reading the block char by char into the buffer
    i = 0;
    int count = 0;
    char ch = fgetc(filePtr);
    while (ch !='\0'){
        blockBuffer[i] = ch;
        i++;
    }
    blockBuffer[i] = '\0';
    if ( i%currentPartition.block_size == 0){
        count = i / currentPartition.block_size;
    }else{
        count = i / currentPartition.block_size + 1;
    }

    /*i = 0;
    for (; i < currentPartition.block_size; i++) {
        char c = fgetc(filePtr);
        if(c == '0'){
            blockBuffer[i] = '\0';
            break;
        }
        blockBuffer[i] = c;
    }
    blockBuffer[i] = '\0';
    */
    fat[file].current_location += count;
    
    return blockBuffer;
}
int write_EXAM(int file, char *data){
    
    /*if(file < 0)
        return 0; // File open opreation probably failed before this call. We should never get here.
    
    if(fat[file].current_location >= MAX_BLOCKS)
        return 0; // No more blocks are available for the file
    
    // Appending by default
    fat[file].current_location = fat[file].file_length;
    
    // Get the file pointer
    FILE *filePtr = NULL;
    int i = 0;
    for (; i < MAX_OPENFILES; i++)
        if(active_file_table_owners[i] == file){
            filePtr = active_file_table[i];
            break;
        }
    if(i == MAX_OPENFILES){
        printf("ERROR: Open the file first\n");
        return 1; // File was not open before this call
    }*/
    
    FILE* filePtr = active_file_table[file];
    if (!filePtr){
        printf("ERROR: Open the file first.\n");
        return 1;
    }
    
    // Seek to next free block
    fseekToNextFreeBlock(filePtr);
    printf("reach here?\n");
    
    // Writing the block char by char into the buffer
    for (int i = 0; i < currentPartition.block_size; i++) {
        char c = *(data+i);
        if(c == 0)
            c = '0';
        fputc(c, filePtr);
    }
   updateFATOnDisk();
    return 1;
}
