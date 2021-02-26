Matan Gazit
CS 4760
Assignment 2
README

Purpose: 

		The goal of this homework is to become familiar with concurrent processing in Linux using shared memory.  
	The task is fairly simple but the trick is to implement it with multiple processes using shared memory and signals 
	to communicate between processes, and to exercise some control over them.

Make:

	To make, simply type cmd "make" into the cmd line while in the project directory. Run using ./master -h for help the first time

	To clean up the project directory between execution, use "make clean" in the cmd line

ISSUES:

	Signal issues: I believe my parent is part of the group of processes that is being terminated when a termination signal is sent to *slave_group. All processes are thus killed
			at termination time because of this, but it is not incrementaly. I was not able to figure out how to clean up this issue at the time I am writing this. Will add
			a revision if solved before submission 2/23/21

	bin_adder issues: I was unable to implement the necessary summation of the data from input file as detailed in the project 2 outline. I was able to succesfully
		accomplish all of the tasks requested, I believe, but could manage to solve the final problem of the project in the requested method as I was not able to succesfully
		implement a means of tracking the depth of the binary tree.

	parent spawns all children issue: I ran into a problem where spawning children from children led to memory leaks. I got around this problem by having only the parent spawn
		children. I know this is not what was detailed in the project outline, but it allowed the program to run succesfully without returning error and failing to deallocate
		shared memory.
	
	sleep(rand() % 3) issue: having the children sleep for any time less than this led to errors in the allocation of the flag shared memory segment
