/* Author:
 Hannah Roach
 CSC 389B/389D Introduction to Operating Systems
 (Online, Spring 2018)
 4/22/2018
 hroac2@uis.edu
 UIN # 678363897
 
 Compile:
gcc roach_hannah_project2.c -o roach_hannah_project2.out
./roach_hannah_project2.out
 
 Brief Description:
 This program is a shell interface with various features. The most recent feature of this program is the implementation of the "mfu" command, which returns the command and frequency of previous commands. Other features include "recent", "!!", "!N", and exit.
 
 */
#include <unistd.h>
#define MAX_LINE 80
#define MAX_ARGS 10
#include <unistd.h>
#include "shell_history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*************************************************************************************************************************************
 *This section of the code is copied from the following blackboard link.
 * https://bb.uis.edu/bbcswebdav/pid-1182005-dt-content-rid-7216452_1/courses/181CSC38911803/CSC389-Project2-file-operation.c
 * the read and write methods were from the example code.
 *************************************************************************************************************************************/

struct record {  // a structure with two components
    int count;    // the number of occurrences
    char cmd[80]; // the command itself
} entry;         // variable of the structure

// a sample function to write structure data into a file
void write_to_file(char *filename,int sample_count[5],char sample_cmd[5][80], int number)
{
    FILE *fptr;  // pointer to the file
//    int sample_count[5] = {2, 4, 3, 9, 5};  // sample integer data
//    char sample_cmd[5][80] = {"ls", "ls -la", "ps", "cal", "date"};  // sample character string data
    int i;
    
    if ((fptr = fopen(filename,"wb")) == NULL) {  // open a binary file for writing (overwrite contents)
        printf("fopen [wb] error!\n");  // file open failed
        exit(1);
    }
    for (i = 0; i < number && i<5; i++) {  // write 5 samples
        entry.count = sample_count[i];    // supply the sample data
        strcpy(entry.cmd, sample_cmd[i]); // supply the sample data
        fwrite(&entry, sizeof(struct record), 1, fptr); // write one structure into the file
    }
    fclose(fptr); // close the file
}

// a sample function to read structure data from a file
void read_from_file(char *filename, int number)
{
    FILE *fptr;  // pointer to the file
    int i;
    
    if ((fptr = fopen(filename,"rb")) == NULL){  // open a binary file for reading
        printf("fopen [rb] error!\n");  // file open failed
        exit(1);
    }
    for (i = 0; i<number && i < 5; i++) {  // read 5 samples
        fread(&entry, sizeof(struct record), 1, fptr); // read one structure from the file
        printf("The command [%s] appeared %d times.\n", entry.cmd, entry.count);
    }
    fclose(fptr); // close the file
}
/*************************************************************************************************************************************
 * End of copied section
 *************************************************************************************************************************************/

void execute(char buffer[MAX_LINE][50], int count){
    pid_t pid;
    int status;
    if ((pid = fork()) < 0)
    {
        exit(1);
    } else if (pid == 0) {
        
        execlp(buffer[count], buffer[count], (char *)0);
        if (execvp((char *)buffer[50], (char **)buffer[count]) < 0)
        {
            exit(1);
        }
    } else {
        
        while (wait(&status)!= pid);
    }
}
int main(int argc, char *argv[])
{
    int should_run = 1;
    int count=0;
    char buffer[MAX_LINE][50] = {""};
    setvbuf(stdout, NULL, _IONBF, 0);
    int number_of_commands = 0;
    char *history_file = "history.bin";
    while (should_run == 1 && count < MAX_ARGS)
    {
        printf("COMMAND-> ");
        if (!fgets (buffer[count], sizeof buffer[count], stdin))
        {
            break;
        }
        
        size_t buffer_length = strlen(buffer[count]);
        if (buffer[count][buffer_length-1] == '\n')
        {
            buffer[count][--buffer_length] = 0;
        }
        if(buffer_length == 0)
        {
            printf("No Command Entered\n" );
            buffer[count][--buffer_length] = 0;
        } else {
            number_of_commands++;
        }
        
        if (strcmp(buffer[count], "exit") == 0) {
            should_run = 0;
            continue;
        }
        char * entry = buffer[count];
        if(entry[1]>='0' && entry[1]<='9' && entry[0] == '!') {
            char a = entry[1];
            int number = a - '0';
            printf("\n==================================\n");
            printf("Entry Number: %d ", number);
            printf("\n==================================\n");
            if(number_of_commands == 1)
            {
                printf("No commands in history.\n");
            }
            else if(strcmp(buffer[number-1], "") == 0)
            {
                printf("No such command in history.\n");
            } else {
                printf("%s \n", buffer[number-1]);
                execute(buffer, number-1);
            }
            printf("==================================\n\n");
        } else if (strcmp(buffer[count],"!!") == 0)
        {
            printf("\n==================================\n");
            printf("Last Entry");
            printf("\n==================================\n");
            int tk_number = count-1;
            char* last_entry = (char *)buffer[tk_number];
            if(number_of_commands == 1)
            {
                printf("No commands in history.\n");
            } else if(strcmp(last_entry,"") ==0)
            {
                printf("No previous command. \n");
            } else {
                printf("%s\n", last_entry);
                execute(buffer, tk_number);
            }
            printf("==================================\n\n");
        } else if (strcmp(buffer[count], "recent") ==0)
        {
            printf("\n==================================\n");
            printf("List of Entries");
            printf("\n==================================\n");
            int tk_number = 0;
            int j = 0;
            if(number_of_commands == 1)
            {
                printf("No commands in history.\n");
            } else {
                for (j=count-1; j>=0; j--){
                    char* token = (char *)buffer[j];
                    while ((token = strtok(token, " "))  != NULL)
                    {
                        tk_number++;
                        printf("Entry Number %d: %s\n",tk_number,token);
                        token = NULL;
                    }
                }
            }
            printf("==================================\n\n");
        }
/*************************************************************************************************************************************
 * New code for Project #2
 *************************************************************************************************************************************/
        else if (strcmp(buffer[count], "mfu") ==0)
        {
            printf("\n==================================\n");
            printf("Frequency of Last 5 Entries");
            printf("\n==================================\n");
            int j = 0; int i,cnt; int freq[10];
            
            char sample_cmd[MAX_LINE][50] = {""};
            if(number_of_commands == 1)
            {
                printf("No commands in history.\n");
            }
            else
            {
                
// Copies previously entered commands into a new array of the same size.
                for (i=count; i>=0; i--)
                {
                    strcpy(sample_cmd[i],buffer[i]);
                    freq[i] = -1;
                }
// Finds the frequency of each entry.
                int size;
                size = sizeof(sample_cmd);
                int number = 0;
                for (i = 0; i<count ; i++)
                {
                    cnt = 1;
                    for (j = i+1; j<count; j++)
                    {
                        if(strcmp(sample_cmd[i], sample_cmd[j]) ==0)
                        {
                            cnt++;
                            freq[j] = 0;
                        }
                    }
                    if (freq[i] != 0)
                    {
                        freq[i] = cnt;
                        number++;
                    }
               }
// Creates an array of the same size as the number of distinct entries.
                char* new_sample_cmd[number];
                int new_freq[number];
                for(i = 0, j=0; j<number ; i++)
                {
                    if(freq[i] != 0)
                    {
                        new_sample_cmd[j]= sample_cmd[i];
                        new_freq[j] = freq[i];
                        j++;
                    }
                }
                
// Sort the list of frequencies and commands in descending order.
                int temp_freq;
                char* temp_cmd;
                for (size_t pass = 0; pass < number - 1; ++pass)
                {
                    for (size_t n = 0; n < number - 1; ++n)
                    {
                        if (new_freq[n]< new_freq[n + 1])
                        {
                            temp_freq = new_freq[n];
                            new_freq[n] = new_freq[n + 1];
                            new_freq[n + 1] = temp_freq;

                            temp_cmd = new_sample_cmd[n];
                            new_sample_cmd[n] = new_sample_cmd[n + 1];
                            new_sample_cmd[n + 1] = temp_cmd;
                        }
                    }
                }
// Copy back int original data type
                char sample_cmd[number][80] ;
                //    char sample_cmd[MAX_LINE][50] = {""};
                int j;
                for(j = 0; j<number; j++)
                {
                    strcpy((char*)sample_cmd[j],new_sample_cmd[j]);
                }
                
// Prints the results to the consol.
//                for (i = 0; i < number; ++i)
//                {
//                    printf("Entry: %s -> %d \n", sample_cmd[i], new_freq[i]);
//                }
// Write to file and read from file.
                write_to_file(history_file, new_freq, sample_cmd, number);
                read_from_file(history_file, number);
            }
        }
        else
        {
            execute(buffer, count);
        }
        count++;
        fflush(stdout);
    }
    return 0;
}


