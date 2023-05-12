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

The iterator was a little wierd. It makes sense why we need to implement it, 
queues don't have peek and cannot be indexed normally. But I still finsihed
the libuthread library without using iterator (Except for testing to make sure
queue's had the right values). Since func queue_func_t was already defined, it
was as easy as going through each node and passing in the data to func.

### Uthread

The Uthread part of the library took the longest to implement. Understanding
the idle loop and the context switching took a good chunk of time to get
correct.

The program begins with the run function. I assumed preempt to be false which
let me focus on the core implementation. I wanted the idle loop to do as little
as possible so I had it tasked with deleting the threads. When a thread is
ready for deletion it can't kill its stack and context switch at the same time.
The exit function puts its thread in the finished queue and switches and yields
to the next thread. The idle loop checks for any threads in the finished queue
and frees the stack and heap space.

Before the idle thread enters the loop, it creates a thread with the passed
arguments from the function call. This create function makes sure everything
is properly initialized before it is sent into the ready queue. This takes 
alot from the context.c file which helps define the lower level functions 
for creating stacks and initializing context's

The yield function is where a lot of the heavy lifting comes in. It is not too
many lines of code but it manages all of the context switching. I originally
set states for each of the threads but found it easier to have seperate queue's
for everything I needed. The yield function takes the current running thread
and puts it at the end of the queue. Afterwards it takes the item in the front
of the queue and replaces it in the current thread. To finilize the yield, it
context switches which starts running the code of the new current thread.

### Semaphores

The semaphores were extremly easy to implement and the most fun. Most of the 
work was already done by the uthread library. The best part of the semaphore
library was the create function returning the semaphore to the user. This made
it so I didn't have to keep track of all the semaphores and the user would just
pass the semaphore as an argument to the function. 

The semaphore struct has two very important member variables: 

*count: the amount of resources in the semaphore
*blocked: a queue of blocked threads requesting the same resource

I thought it best for the semaphore to keep track of the blocked threads. This
made it so uthread wouldn't have to have a data structure filled with blocked
queues which would be messy to handle.

Whenever a thread would call sem_down, the function would check to see if there
were any resources left to give. If there were none, it would add itself to the
semaphores blocked queue and tell uthread to block the thread. Uthread would
take the next thread and put it into the current thread. This is similar to
yielding but we don't want to put the blocked thread back in the ready queue.

When sem_up was called, it would give up a resource from the semaphore and
unblock a thread. Uthread would simply take the unblocked thread and put it
back into the ready queue. This would not garentee that the thread would get
the resource. The interesting thing is semaphores are not built to check the 
maximum resources it can hold. Therefore a user could infinitly call sem_up
and the program would not stop them.

To implement the blocking, I made a spinlock that would check if there are any
resources in the semaphore. This looks like a regular spin lock, but since
the thread puts itself in a blocked queue, it doesn't infinitly yield. Instead,
it yields to the next thread and only comes back to that while statement when
a resource is freed.

### Preemption

Preemption was by far the most confusing part to implement. It was very few
lines of code but it wasn't very intuitive. I took a lot of inspiration from
the lecture slides that wen't over how to do signals. 

When you start preemption, it checks to see if the bool was true. If not, it
blocks every function from preforming its task. I did this so there wasn't a
need to put if statements in the uthread code to check if preemption was
enabled.

If it was true, it sets up the signal handler. This signal handler only listens
for the SIGVTALRM signal. Once the handler is setup, I use an itimer to send a
virtual signal from itself at a rate of 100 HZ. Everytime the signal handler
recieves this signal, it yields to the next thread.

If preemption was always on, critical sections in the library would not be
protected and things would cease to function properly. Thats what enable and 
disable is for. Whenever disable is called, it blocks the signals being sent
from preemption and preforms normal execution of threads. This allows me to
protect the important parts like the queues from being interupted.

It took a pretty long time to figure out what sections needed preemption
disabled. The best way to do it was to go line by line and think "what would
happen if it yielded". This would tell me which parts of the code should have
preemption disabled.

The last part of preemption was the stop function. After the program is done
with all of its threads, calling stop will first restore the old signal handler
(which is basically crashing at any signal). Secondly, it will restore the
real timer for itimer.

## Testing

The Queue was the most important thing to test because of how many things
relied on the Queue working. I decided to make as many unit tests as I could
which would test each item seperatly. It was important that these items were
seperated from eachother so they problem could be easily identified if it came
up. In the case that something needs another to be tested (like how length
needs enqueue and create to work so length can be tested), I would make sure
that the test happened after everything it depended on.

The semaphores were extremly easy to test. There were predefined tests in the
apps directory that tested multiple features of the semaphore.

The preemption was quite the opposite. Testing preemption wasn't as intuitive
as the queue because there was no concrete functions that you know the result
of. The way I decided to do it was by having a infinite while loop in two
threads. They would print to the screen what thread they are in. This way
when preemption is true, I can see the threads being preempted.

## Sources

I mainly used the past projects (project 0 and 1 from this class) and the 
lecture slides to figure out how to implement things. The lecture 
slides/lectures were extremly good at explaining Semaphores and Threads. Also
the in person lectures example for the makefile was really good.

Another thing I used was GNU on how to work with signals. They were one of the
most confusing things to learn.