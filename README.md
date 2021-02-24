Matan Gazit
CS 4760
Assignment 2
README

Purpose: 

		The goal of this homework is to become familiar with concurrent processing in Linux using shared memory.  
	The task is fairly simple but the trick is to implement it with multiple processes using shared memory and signals 
	to communicate between processes, and to exercise some control over them.


ISSUES:

	Signal issues: I believe my parent is part of the group of processes that is being terminated when a termination signal is sent to *slave_group. All processes are thus killed
			at termination time because of this, but it is not incrementaly. I was not able to figure out how to clean up this issue at the time I am writing this. Will add
			a revision if solved before submission 2/23/21
