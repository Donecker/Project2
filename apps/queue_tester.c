#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, * ptr;
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Multiple Enqueue/Dequeue */
void test_queue_complex(void) {

	int data[3] = { 1,2,3 };
	int* ptr[3];

	queue_t q;

	fprintf(stderr, "\n*** TEST queue_complex ***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[2]);

	queue_dequeue(q, (void**)&ptr[0]);
	queue_dequeue(q, (void**)&ptr[1]);
	queue_dequeue(q, (void**)&ptr[2]);

	// Check if enqueue and dequeue put everything in the correct order
	TEST_ASSERT(ptr[0] == &data[0] && ptr[1] == &data[1] && ptr[2] == &data[2]);
}

/* Destroy */
void test_queue_destroy(void) {
	queue_t q;
	int data = 5;

	fprintf(stderr, "\n*** TEST queue_destroy ***\n");

	q = queue_create();

	// Check if destroy properly destroys queue
	TEST_ASSERT(queue_destroy(q) == 0);

	q = queue_create();

	queue_enqueue(q, &data);

	// Check if destroy returns -1 when queue is not empty
	TEST_ASSERT(queue_destroy(q) == -1);

	// Check if destroy returns -1 when queue is NULL
	TEST_ASSERT(queue_destroy(NULL) == -1);
}

/* Improper Enqueue/Dequeue */
void test_queue_improper(void) {

	int data = 5;
	int* ptr;

	queue_t q;

	fprintf(stderr, "\n*** TEST queue_improper ***\n");

	// Check if enqueue returns -1 when queue is null
	TEST_ASSERT(queue_enqueue(NULL, &data) == -1);

	// Check if dequeue returns -1 when queue is null
	TEST_ASSERT(queue_dequeue(NULL, (void**)&ptr) == -1);

	q = queue_create();

	// Check if enqueue returns -1 when data is null
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);

	// Check if dequeue returns -1 when data is null
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);

}

/* Delete */
void test_queue_delete(void) {

	int data[3] = { 1,2,3 };
	int* ptr[3];

	queue_t q;

	fprintf(stderr, "\n*** TEST queue_delete ***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[2]);

	// Check if queue can delete &data[0] twice
	TEST_ASSERT(queue_delete(q, &data[0]) == 0);
	TEST_ASSERT(queue_delete(q, &data[0]) == 0);

	queue_dequeue(q, (void**)&ptr[0]);
	queue_dequeue(q, (void**)&ptr[1]);

	// Check if the dequeued things from the queue are the non deleted items, in order
	TEST_ASSERT(ptr[0] == &data[1] && ptr[1] == &data[2]);
}

/* Length */
void test_queue_length(void) {

	int data[3] = { 1,2,3 };
	int* ptr[3];

	queue_t q;

	fprintf(stderr, "\n*** TEST queue_length ***\n");

	q = queue_create();

	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);

	// Check if length is correct after enqueue
	TEST_ASSERT(queue_length(q) == 2);

	queue_enqueue(q, &data[2]);
	queue_dequeue(q, (void**)&ptr[0]);
	queue_dequeue(q, (void**)&ptr[1]);

	// Check if length is correct after dequeue
	TEST_ASSERT(queue_length(q) == 1);

	queue_dequeue(q, (void**)&ptr[2]);

	// Check if length is correct after dequeue
	TEST_ASSERT(queue_length(q) == 0);

	// Check if length returns -1 if queue is NULL
	TEST_ASSERT(queue_length(NULL) == -1);

	queue_enqueue(q, &data[0]);
	queue_delete(q, &data[0]);

	// Check if length is correct after deleting
	TEST_ASSERT(queue_length(q) == 0);
}

void iterator_delete(queue_t queue, void* data) {
	queue_delete(queue, data);
}

/* Iterator */
void test_queue_iterator_delete_all(void) {

	int data[3] = { 1,2,3 };

	queue_t q;

	fprintf(stderr, "\n*** TEST queue_iterator_delete_all ***\n");

	q = queue_create();

	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[2]);

	queue_iterate(q, iterator_delete);
	TEST_ASSERT(queue_length(q) == 0);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_queue_complex();
	test_queue_destroy();
	test_queue_improper();
	test_queue_delete();
	test_queue_length();
	test_queue_iterator_delete_all();

	return 0;
}
