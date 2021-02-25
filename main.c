/*
 *Matan Gazit
 *CS 4760
 *Assignment 2
 *main.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

/*Constant Definitions */
#define DEF_MAX_PROCESSES 20
#define DEF_MAX_TIME 100
#define MAX 100

/*Prototypes */
void print_usage();
void spawn_slave(int count); // spawns a slave process so long as <DEF_MAX_PROCESSES
void spawn(int count); // extension for spawn_slave for readability purposes
void deallocate();
void c_sig_handler(int sig);
void timeout(int sig);

/*Static Variables*/
static char* _program_name;
static char* _error_str[100];

/* Globals */
int start_time; // timer
int curr_proc_count; // current # of processes in system
int state = 0; //easy var call for wait function
int active_procs = 0; // number of processes currently running
int user_max_procs;
int user_max_time;
char* data;
enum state {vacant, idle, want_in, in_cs};

/*Shared Memory Variables */
int shared_sum_key;
int shared_sum_id;
int *shared_sum;

int slave_group_key;
int slave_group_id;
pid_t *slave_group;

int slave_count_key;
int slave_count_id;
int *slave_count;

int filename_key;
int filename_id;
char* filename;

int flags_key;
int flags_id;
int *flags;

int turn_key;
int turn_id;
int *turn;

int proc_num_key;
int proc_num_id;
int *proc_num;

int shared_arr_key;
int shared_arr_id;
int *shared_arr;

int main (int argc, char* argv[])
{
	signal(SIGINT, c_sig_handler);
	signal(SIGALRM, timeout);
	
	//int user_max_procs = 20;
	_program_name = argv[0];
	const char* file_out = "output"; // shared output file for the program

	/*Shared Memory Variables */
	
	//Shared memory vars for the sum, which will be calculated
	shared_sum_key = ftok("makefile", 1);
	
	//Shared memory vars for the slave group
	slave_group_key = ftok("makefile", 2);

	//Shared memory vars for slave number count
	slave_count_key = ftok("makefile", 3);

	//Shared memory vars for the output file's name/path
	filename_key = ftok("makefile", 4);

	//Shared memory vars for flags
	flags_key = ftok("makefile", 5);

	//Shared memory vars for turn token
	turn_key = ftok("makefile", 6);
	
	//Shared memory for number of processes in system
	proc_num_key = ftok("makefile", 7);

	//Shared memory for array of integers to add
	shared_arr_key = ftok("makefile", 8);

	user_max_procs = -1;
	user_max_time = -1;
	//int shm_id; // stores shared memory ID
	
	bool fail = false;
	
	setvbuf(stdout, NULL, _IONBF, 0); // for debugging purposes
	_program_name = argv[0];
	snprintf(_error_str, sizeof _error_str, "%s: Error", _program_name); // for perror

	int i; // initialize an iterator
	
	if (argc == 1)
	{
		printf("Run program with -h for usage options...\n");
		return EXIT_SUCCESS;
	}

	while (true)
	{
		int c = getopt(argc, argv, "hs:t:");
		if (c == -1) break;
		switch (c)
		{	
			case 'h':
				print_usage();
				return EXIT_SUCCESS;
				break;
			case 's':
				user_max_procs = atoi(optarg);
				if (user_max_procs > DEF_MAX_PROCESSES)
				{
					printf("No more than %d processes allowed!\n", DEF_MAX_PROCESSES);
					printf("Setting allowed processes to default value: %d\n", DEF_MAX_PROCESSES);
					user_max_procs = DEF_MAX_PROCESSES;
				}
				//printf("Read for arg [-s]: %d\n", user_max_procs);
				break;
			case 't':
				user_max_time = atoi(optarg);
				if (user_max_time > DEF_MAX_TIME)
                                {
                                  printf("No longer than %d seconds allowed!\n", DEF_MAX_TIME);
                                  printf("Setting allowed processes to default value: %d (seconds)\n", DEF_MAX_TIME);
                                  user_max_procs = DEF_MAX_TIME;
                                }
				//printf("Read for arg [-t]: %d\n", user_max_time);
				break;
			default:
				printf("Unknown input read... Exiting...\n");
				fail = true;
				break;
		} // end swtich(getopt)
	} // end while(getopt)
	
	if (fail == true) return EXIT_FAILURE; // end program if a wrong option was selected

	if ((shared_arr_id = shmget(shared_arr_key, sizeof(int) * MAX, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
  {
	  perror("ERROR: Shmget failed to allocated memory for shared integer array\n");
	  exit(1);
	}
	else
	{
	  shared_arr = (int *)shmat(shared_arr_id, NULL, 0);
	}

	//Create file using path from last cmdln argument
	FILE *datafile;
	if ((datafile = fopen(argv[argc - 1], "r")) == NULL)
	{
		perror("Please ensure that the input file is the last argument");
		return EXIT_FAILURE;
	}
	else
	{
		// Read contents of data file and add contents to a shared memory array of ints
		// Will stop reading in data and move on if it encounters a non decimal input in shared_arr
		int n_ints = 0;
		while(n_ints < MAX && fscanf(datafile, "%d", &shared_arr[n_ints]) == 1) 
		{ 
			//printf("%d\n", shared_arr[n_ints]);
			++n_ints;
		}
		fclose(datafile);
		data = argv[argc - 1];
	}
	
	// if block checks to see if user decided on time/process count	
	if (user_max_procs == -1) user_max_procs = DEF_MAX_PROCESSES;
	if (user_max_time == -1) user_max_time = DEF_MAX_TIME;

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
    *slave_count = user_max_procs;
	}
	
	// filename is assumed to be < 25 characters in length
	if ((filename_id = shmget(filename_key, sizeof(char) * 26, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
  {
    perror("ERROR: Shmget failed to allocated memory for shared filename\n");
    exit(1);
  }
  else
  {
    filename = (char *)shmat(filename_id, NULL, 0);
    strcpy(filename, file_out);
  }

	if ((flags_id = shmget(flags_key, sizeof(int) * user_max_procs, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
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
		*proc_num = 1;
  }

	start_time = time(0);
	alarm(user_max_time);

	int count = 1;

	while (*proc_num < user_max_procs)
	{
		spawn_slave(*proc_num); // spawn a slave with count id
		++(*proc_num); // increment count to keep track of process #
	}
	
	while ((time(0) - start_time < user_max_time) && (*proc_num > 0))
	{
		wait(NULL);
		if (*proc_num < user_max_procs && *proc_num != 0)
		{
			//--(*proc_num);
			int new_spawn_id = 1;
			while (flags[new_spawn_id - 1] != vacant) { ++new_spawn_id; }
			flags[new_spawn_id - 1] = idle;
			spawn_slave(new_spawn_id);
		}
		//if (*proc_num > 0) --(*proc_num); // was int curr_proc_count
		//printf("There are currently %d processes in the system...\n", *proc_num);
		//spawn_slave(*proc_num);
	}

	//sleep();	

	deallocate();
	
	printf("We're back in main\n");
	return EXIT_SUCCESS;
}

void print_usage()
{
	printf("master [-h]\n");
	printf("master [-h] [-s i] [-t time] datafile\n");
}


// TODO: SPAWN SLAVES!
void spawn_slave(int count)
{
	if (*proc_num < user_max_procs)
	{
		spawn(count);
	}
	else
	{
		waitpid(-(*slave_group), &state, 0);
		--(*proc_num); // was int curr_proc_count
		printf("Waiting. There are currently %d processes in the system!\n", *proc_num);
		spawn(count);
	}
}

void spawn(int count)
{
	char id[10];
	sprintf(id, "%d", count); 
	++(*proc_num);
	printf("value of proc_num is: %d\n", *proc_num);
	//++curr_proc_count;
	if (fork() == 0)
	{
		printf("There are currently %d processes in the system!\n", *proc_num);
		if (count == 1)
		{
			(*slave_group) = getpid(); // get group pid for the first only
		}
		setpgid(0, (*slave_group));
		printf("We spawned a child!\n");
		execl("./slave", "slave", id, NULL);
		//++(*proc_num);
		exit(0);
	}
}

void deallocate()
{
	shmdt(shared_sum);
	shmctl(shared_sum_id, IPC_RMID, NULL);
	
	shmdt(slave_group);
  shmctl(slave_group_id, IPC_RMID, NULL);

	shmdt(flags);
  shmctl(flags_id, IPC_RMID, NULL);
	
	shmdt(turn);
  shmctl(turn_id, IPC_RMID, NULL);

  shmdt(slave_count);
  shmctl(slave_count_id, IPC_RMID, NULL);

	shmdt(filename);
  shmctl(filename_id, IPC_RMID, NULL);

	shmdt(proc_num);
  shmctl(proc_num_id, IPC_RMID, NULL);
	
	shmdt(shared_arr);
	shmctl(shared_arr_id, IPC_RMID, NULL);

	printf("Succesfully deallocated memory for all shared variables\n");
}

void c_sig_handler(int sig)
{
	int i = 0;
	printf("\nReceived termination signal, exiting...\n");
	killpg((*slave_group), SIGTERM);
	for (; i < user_max_procs; i++)
	{
		wait(NULL);
	}
	printf("Exiting Process\n");
	deallocate();	
	exit(1);
}

void timeout(int sig)
{
	int i = 0;	
	printf("We're out of time, folks...\n");
	killpg((*slave_group), SIGTERM);
	for (; i < user_max_procs; i++)
	{
		wait(NULL);
	}
	//deallocate();
	printf("Exiting Process\n");
	deallocate();
	exit(1);
}
