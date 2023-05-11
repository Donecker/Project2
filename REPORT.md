# Project 2 Report

Project #2 Report - Libuthread library

## Notes

Hello my name is Stephen Donecker and I wrote the entirety of this project.
My partner told me their grandma died and stopped responding to any
communication. I don't blame them as this is a tragic event. I just wanted to
let the reader know this incase my teammate doesn't submit a peer feedback
report.

## Implementation

### Queue

The Queue was an important data structure that was relied apon by the rest of
the library. I used the comments from the Queue header file as a guide on how
to implement it. When creating the Queue, I made sure that every function would
check the corresponding preconditions and return if anything was recieved that
wasn't expected. 

The Queue was implemented similarrly to a linked list. This allowed it to be
dynamically expanded and gave access to the front and end elements
which is needed for enqueue and dequeue.

If the Queue was successfully created, enqueue and dequeue could be
called which would insert the data value into the Queue. Enqueue adds the item
to the end of the Queue, and dequeue takes the oldest item from the front. It
was important that the Queue was implemented with the void pointers. This
allowed the Queue to hold any type of data. This made it the user's job to
make sure that they correctly cast the data back into the correct type.

Delete was also an important function to implement because this allowed the
Queue to remove an item from any position in the Queue. This was accomplished
by going through the entire queue and checking if the queue contained the same
address as the data we were trying to remove.

//Add iterator description here

### Uthread

The Uthread part of the library took the longest to implement. Understanding
the idle loop and the context switching took a good chunk of time to get
correct.

The program begins with the run function. I assumed preempt to be false which
let me focus on the core implementation.





## Testing

The Queue was the most important thing to test because of how many things
relied on the Queue working. I decided to make as many unit tests as I could
which would test each item seperatly. It was important that these items were
seperated from eachother so they problem could be easily identified if it came
up. In the case that something needs another to be tested (like how length
needs enqueue and create to work so length can be tested), I would make sure
that the test happened after everything it depended on.

## Sources

I mainly used the past projects (project 0 and 1 from this class) and the 
lecture slides to figure out how to implement things. The lecture 
slides/lectures were extremly good at explaining Semiphores and Threads. Also
the in person lectures example for the makefile was really good.

Another thing I used was GNU on how to work with signals. They were one of the
most confusing things to learn.