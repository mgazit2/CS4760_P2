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
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

/*Constant Variables*/

#define TIME_SIZE 50 
#define MAX 100

/*Prototypes */
void get_time();
void c_sig_handler(int sig);

/*Global Variabls */
char curr_time[TIME_SIZE];
enum state {vacant, idle, want_in, in_cs};
int id;

int main (int argc, char* argv[])
{
	//setvbuf(stdout, NULL, _IONBF, 0); // for deebugging purposes
	signal(SIGTERM, c_sig_handler);
	signal(SIGINT, c_sig_handler);

	/*if (argc < 3)
	{
		printf("Child lacks necessary number of arguments... Exiting...\n");
		return EXIT_FAILURE;
	}
	*/
	id = atoi(argv[1]);
		
	//id = argv[1];
	//printf("%d\n", id);

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
	
	//Shared memory vars for process counter
	int proc_num_key = ftok("makefile", 7);
	int proc_num_id;
	int *proc_num;

	//Shared memory vars for shared array of ints
	int shared_arr_key = ftok("makefile", 8);
	int shared_arr_id;
	int *shared_arr;

	if ((shared_arr_id = shmget(shared_arr_key, sizeof(int) * MAX, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
	{
		perror("ERROR: Shmget failed to allocatee memory for shared array of integers\n");
		exit(1);
	}
	else
	{
		shared_arr = (int *)shmat(shared_arr_id, NULL, 0);
	}

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
	
	if ((proc_num_id = shmget(proc_num_key, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
  {
     perror("ERROR: Shmget failed to allocated memory for shared processes count\n");
    exit(1);
  }
  else
  {
    proc_num = (int *)shmat(proc_num_id, NULL, 0);
  }
	//int id = rand() % PROCS;
	int i;
	do
	{
		flags[id - 1] = want_in;
		i = *turn;

		while (i != id - 1)
		{
			i = (flags[i] != idle || flags[i] != vacant) ? *turn : (i + 1) % PROCS;
		}

		flags[id - 1] = in_cs;
		for (i = 0; i < PROCS; i++)
		{
			if ((i != (id - 1) && (flags[i] == in_cs))) break;
		}

	} while ( (i < PROCS) || ((*turn != id - 1) && (flags[*turn] != idle || flags[*turn] != vacant)) );
	*turn = id - 1;

	/*CRITICAL SECTION */	
	sleep(rand() % 3);

	get_time();
	int pid = getpid();

	fprintf(file, "\n%s | %d | %d | %d\n", curr_time, pid, id, 0);
	--(*proc_num);

	sleep(rand() % 3);
	
	i = (*turn + 1) % PROCS;
	while (flags[i] == idle || flags[i] == vacant)
	{
		i = (i + 1) % PROCS;
	}
	*turn = i;

	flags[id - 1] = vacant;
	//kill(getppid(), SIGKILL);
	
	fclose(file);
	return EXIT_SUCCESS;
}

void get_time()
{
	time_t curr; // current time variable
	struct tm *loc_time; // time struct for local time

	curr = time(NULL);

	loc_time = localtime(&curr);

	strftime (curr_time, TIME_SIZE, "%H:%M:%S\n", loc_time); 	
	//fputs(curr_time, stdout);
}

void c_sig_handler(int sig)
{
  printf("\nChild: Received termination signal, exiting...\n");
	exit(1);
}
