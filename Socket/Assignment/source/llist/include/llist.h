#ifndef LLIST_H
#define LLIST_H

/**
 * @file llist.h
 * @brief Header file for Generic Linked List implementation
 */

/**
 * @struct node
 * @brief Node structure representing each element in the linked list.
 */
struct node {
    void *data; /**< Pointer to the data stored in the node */
    struct node *next; /**< Pointer to the next node in the list */
};

/**
 * @typedef llist
 * @brief Pointer to the linked list structure.
 */
typedef struct node* llist;

/**
 * @brief Creates a new linked list with the given initial data.
 * @param new_data Pointer to the initial data for the new list.
 * @return Pointer to the newly created list.
 */
llist *llist_create(void *new_data);

/**
 * @brief Frees the memory occupied by the linked list and its nodes.
 * @param list Pointer to the linked list to be freed.
 */
void llist_free(llist *list);

/**
 * @brief Adds an element to the linked list in sorted order based on the provided comparison function.
 * @param data Pointer to the data to be added.
 * @param list Pointer to the linked list.
 * @param comp Comparison function for sorting elements.
 * @return 1 on success, 0 on failure.
 */
int llist_add_inorder(void *data, llist *list, int (*comp)(void *, void *));

/**
 * @brief Pushes an element to the front of the linked list.
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be added.
 */
void llist_push(llist *list, void *data);

/**
 * @brief Pops an element from the front of the linked list.
 * @param list Pointer to the linked list.
 * @return Pointer to the popped data.
 */
void *llist_pop(llist *list);

/**
 * @brief Prints the elements in the linked list using the provided print function.
 * @param list Pointer to the linked list.
 * @param print Function pointer to the printing function for elements.
 */
void llist_print(llist *list, void (*print)(void *));

/**
 * @brief Deletes a node from the linked list at the specified position.
 * @param list Pointer to the linked list.
 * @param position Position of the node to be deleted.
 */
void llist_delete_at_position(llist *list, int position);

/**
 * @brief Deletes the first node from the linked list.
 * @param list Pointer to the linked list.
 */
void llist_delete_at_beginning(llist *list);

/**
 * @brief Deletes the last node from the linked list.
 * @param list Pointer to the linked list.
 */
void llist_delete_at_end(llist *list);

/**
 * @brief Traverses the linked list and performs the specified operation on each node.
 * @param list Pointer to the linked list.
 * @param operation Function pointer to the operation to be performed on each node.
 */
void llist_traverse(llist *list, void (*operation)(void *));
/**
 * @brief Inserts a node with the given data at the end of the linked list.
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be inserted.
 */
void llist_insert_at_end(llist *list, void *data);

#endif /* LLIST_H */
