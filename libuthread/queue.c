#include "queue.h"

#include <stdlib.h>
#include <string.h>

struct queue_node {
	void* data;
	struct queue_node* next;
};

struct queue_node* new_node() {
	// Allocate memory for new node
	struct queue_node* node = (struct queue_node*)malloc(sizeof(struct queue_node));

	// Memory was not allocated
	if (node == NULL) {
		return NULL;
	}

	// Link the node to nothing since it is at the end
	node->next = NULL;

	return node;
}

struct queue {
	struct queue_node* front;
	struct queue_node* back;
	int length;
};

queue_t queue_create(void) {
	// Allocate memory for new queue
	queue_t queue = (queue_t)malloc(sizeof(struct queue));

	// Memory was not allocated
	if (queue == NULL) {
		return NULL;
	}

	// Set variables to default
	queue->front = NULL;
	queue->back = NULL;
	queue->length = 0;

	return queue;
}

int queue_destroy(queue_t queue) {

	// Check preconditions
	if (queue == NULL) {
		return -1;
	}

	// Check if queue is not empty
	if (queue->length != 0) {
		return -1;
	}

	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data) {

	// Check preconditions
	if (queue == NULL) {
		return -1;
	}
	if (data == NULL) {
		return -1;
	}

	struct queue_node* node = new_node();

	// Check if node was successfully created
	if (node == NULL) {
		return -1;
	}

	node->data = data;

	// Check if queue is empty
	if (queue->length == 0) {
		queue->front = node;
	}
	else {
		queue->back->next = node;
	}

	queue->back = node;
	queue->length += 1;

	return 0;
}

int queue_dequeue(queue_t queue, void **data) {

	// Check preconditions
	if (queue == NULL) {
		return -1;
	}
	if (data == NULL) {
		return -1;
	}
	if (queue->length == 0) {
		return -1;
	}

	// Set data equal to the oldest value
	*data = queue->front->data;

	// Delete that node
	return queue_delete(queue, *data);
}

int queue_delete(queue_t queue, void *data) {

	// Check preconditions
	if (queue == NULL) {
		return -1;
	}
	if (data == NULL) {
		return -1;
	}

	struct queue_node* previous_node = queue->front;
	struct queue_node* current_node = queue->front;
	for (int i = 0; i < queue->length; i++) {
		if (i > 1) {
			previous_node = previous_node->next;
		}
		if (current_node->data == data) {
			// If it's the first or last item
			if (i == 0) {
				// Set current_node to the next one as a placeholder
				current_node = queue->front->next;

				// Free front node
				free(queue->front);

				// Set the next to be the queue's new front node
				queue->front = current_node;

				// Decrement length
				queue->length -= 1;

				return 0;
			}
			else if (i == queue->length - 1) {
				// If this is the last item, free it
				free(current_node);

				// Set previous node's next to null
				previous_node->next = NULL;

				// Set queue's last node to previous node
				queue->back = previous_node;

				// Decrement length
				queue->length -= 1;

				return 0;
			}
			else {
				// Get the current node's next as a placeholder
				struct queue_node* next_node = current_node->next;

				// Free current node
				free(current_node);

				// Set previous node to the current's next node
				previous_node->next = next_node;

				// Decrement length
				queue->length -= 1;

				return 0;
			}
		}
		// Go to the next node
		current_node = current_node->next;
	}

	// Didn't find data in queue
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func) {

	// Check preconditions
	if (queue == NULL) {
		return -1;
	}
	if (func == NULL) {
		return -1;
	}

	struct queue_node* current = queue->front;
	struct queue_node* next;
	while (current != NULL) {
		next = current->next;
		func(queue, current->data);
		current = next;
	}

	return 0;
}

int queue_length(queue_t queue) {

	// Check preconditions
	if (queue == NULL) {
		return -1;
	}

	return queue->length;
}

