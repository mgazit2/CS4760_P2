Matan Gazit
CS 4760
Assignment 2
README

Purpose: 

		The goal of this homework is to become familiar with concurrent processing in Linux using shared memory.  
	The task is fairly simple but the trick is to implement it with multiple processes using shared memory and signals 
	to communicate between processes, and to exercise some control over them.

Make:

	To make, simply type cmd "make" into the cmd line while in the project directory. Run using ./master -h 
	for help the first time

	To clean up the project directory between execution, use "make clean" in the cmd line

ISSUES:

	NOTE: The program does not exit normally, but rather relies on one of the signals to end accordingly... I BELIEVE THIS TO BE SOLVED

	Signal issues: I believe my parent is part of the group of processes that is being terminated when a termination signal is
		sent to *slave_group. All processes are thus killed at termination time because of this, but it is not incrementaly. 
		I was not able to figure out how to clean up this issue at the time I am writing this. Will add	a revision if solved before submission 2/23/21. 
		I BELIEVE THIS TO BE SOLVED!

	bin_adder issues: I was unable to implement the necessary summation of the data from input file as detailed in the project 2 outline. I was able to succesfully
		accomplish all of the tasks requested, I believe, but could manage to solve the final problem of the project in the requested method as I was not able to succesfully
		implement a means of tracking the depth of the binary tree.

	parent spawns all children issue: I ran into a problem where spawning children from children led to memory leaks. I got around this problem by 
		having only the parent spawn children. I know this is not what was detailed in the project outline, but it allowed the 
		program to run succesfully without returning error and failing to deallocate shared memory.
	
	sleep(rand() % 3) issue: having the children sleep for any time less than this led to errors in the allocation of the flag shared memory segment

	I strongly believe that the reason for which this is not working is because I was not allocating to a variable memory size, but rather using a 
		constant... I realized this	too late, and it was plain in the project outline...

REFERENCES:

	The first reference is a github repo which served a s atemplate for my my project 2. Being largely unfamiliar
	with most of the tools we were required to implement, this repo helped in creating the initial structure for
	an adequate means of interprocess communication, as requested by the project 2 outline. The rest of the
	framework was constructed by myself with the help of the rest of the references. I stubbornly decided to keep
	the language of my project as C, instead of following the C++ structure:
	https://github.com/jeffcaljr/Unix-Concurrent-Processes-and-Shared-Memory

	Jared's repo for Project 2 was also used as a reference when working on this project:
	https://github.com/jaredible/CS4760-Project-2

	Used for reading integers from a file
	https://stackoverflow.com/questions/4600797/read-int-values-from-a-text-file-in-c

	As always, the man pages were invaluable in researching this project
	https://man7.org/linux/man-pages/man2/waitid.2.html
	https://man7.org/linux/man-pages/man7/shm_overview.7.html
	https://man7.org/linux/man-pages/man2/shmop.2.html

	As a final note for the references section, the Peterson algorithm used in 'child.c' was also used to create
	a queue for the many processes created during the execution runtime of this program
