/* Matan Gazit
 * CS 4760
 * Assignment 2
 * child.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main (int argc,char* argv[])
{
	int PROCS; // variable will hold number of processes that can execute at a time

	/*Shared Memory Variables */

        //Shared memory vars for the sum, which will be calculated
        int shared_sum_key = ftok("makefile", 1);
        int shared_sum_id;
        int *shared_sum;
        
        //Shared memory vars for the slave group
        int slave_group_key = ftok("makefile", 2);
        int slave_group_id;
        int *slave_group;
        
        //Shared memory vars for slave number count
        int slave_count_key = ftok("makefile", 3);
        int slave_count_id;
        int *slave_count;
        
        //Shared memory vars for the output file's name/path
        int filename_key = ftok("makefile", 4);
        int filename_id;
        char* filename;
        FILE *file;

        //Shared memory vars for flags
        int flags_key = ftok("makefile", 5);
        int flags_id;
        int *flags;
        
        //Shared memory vars for turn token
        int turn_key = ftok("makefile", 6);
        int turn_id;
        int *turn;	

	if ((shared_sum_id = shmget(shared_sum_key, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
        {
                perror("ERROR: Shmget failed to allocated memory for shared sum\n");
                exit(1);
        }
        else
        {
                shared_sum = (int *)shmat(shared_sum_id, NULL, 0);
                (*shared_sum) = 0;
        }

        if ((slave_group_id = shmget(slave_group_key, sizeof(pid_t), IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
        {
                perror("ERROR: Shmget failed to allocated memory for slave group\n");
                exit(1);
        }
        else
        {
                slave_group = (pid_t *)shmat(slave_group_id, NULL, 0);
        }

        if ((slave_count_id = shmget(slave_count_key, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
        {
                perror("ERROR: Shmget failed to allocated memory for slave counter\n");
                exit(1);
        }
        else
        {
                slave_count = (int *)shmat(slave_count_id, NULL, 0);
                PROCS = *slave_count;
        }
	
	if ((filename_id = shmget(filename_key, sizeof(char) * 26, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
        {
                perror("ERROR: Shmget failed to allocated memory for shared filename\n");
                exit(1);
        }
        else
        {
                filename = (char *)shmat(filename_id, NULL, 0);
                file = fopen(filename, "a");
        }

        if ((flags_id = shmget(flags_key, sizeof(int) * PROCS, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
        {
                perror("ERROR: Shmget failed to allocated memory for flags array\n");
                exit(1);
        }
        else
        {
                flags = (int *)shmat(flags_id, NULL, 0);
        }

        if ((turn_id = shmget(turn_key, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
        {
                perror("ERROR: Shmget failed to allocated memory for shared turn array\n");
                exit(1);
        }
        else
        {
                turn = (int *)shmat(turn_id, NULL, 0);
        }

	++(*shared_sum);

	printf("%d\n", *shared_sum);	
	
	printf("I am a child\n");
	sleep(5);
	return EXIT_SUCCESS;
}
