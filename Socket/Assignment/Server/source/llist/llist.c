#include <stdlib.h>
#include <stdio.h>
#include "llist.h"

/**
 * @brief Creates a new linked list with the given initial data.
 * @param new_data Pointer to the initial data for the new list.
 * @return Pointer to the newly created list.
 */
llist *llist_create(void *new_data)
{
    struct node *new_node;

    llist *new_list = (llist *)malloc(sizeof (llist));
    *new_list = (struct node *)malloc(sizeof (struct node));
    
    new_node = *new_list;
    new_node->data = new_data;
    new_node->next = NULL;
    return new_list;
}
/**
 * @brief Frees the memory occupied by the linked list and its nodes.
 * @param list Pointer to the linked list to be freed.
 */
void llist_free(llist *list)
{
    struct node *curr = *list;
    struct node *next;

    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }

    free(list);
}

/**
 * @brief Adds an element to the linked list in sorted order based on the provided comparison function.
 * @param data Pointer to the data to be added.
 * @param list Pointer to the linked list.
 * @param comp Comparison function for sorting elements.
 * @return 1 on success, 0 on failure.
 */
int llist_add_inorder(void *data, llist *list,
                       int (*comp)(void *, void *))
{
    struct node *new_node;
    struct node *curr;
    struct node *prev = NULL;
    
    if (list == NULL || *list == NULL) {
        fprintf(stderr, "llist_add_inorder: list is null\n");
        return 0;
    }
    
    curr = *list;
    if (curr->data == NULL) {
        curr->data = data;
        return 1;
    }

    new_node = (struct node *)malloc(sizeof (struct node));
    new_node->data = data;

    // Find spot in linked list to insert new node
    while (curr != NULL && curr->data != NULL && comp(curr->data, data) < 0) {
        prev = curr;
        curr = curr->next;
    }
    new_node->next = curr;

    if (prev == NULL) 
        *list = new_node;
    else 
        prev->next = new_node;

    return 1;
}
/**
 * @brief Pushes an element to the front of the linked list.
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be added.
 */
void llist_push(llist *list, void *data)
{
    struct node *head;
    struct node *new_node;
    if (list == NULL || *list == NULL) {
        fprintf(stderr, "llist_add_inorder: list is null\n");
    }

    head = *list;
    
    // Head is empty node
    if (head->data == NULL)
        head->data = data;

    // Head is not empty, add new node to front
    else {
        new_node = malloc(sizeof (struct node));
        new_node->data = data;
        new_node->next = head;
        *list = new_node;
    }
}
/**
 * @brief Pops an element from the front of the linked list.
 * @param list Pointer to the linked list.
 * @return Pointer to the popped data.
 */
void *llist_pop(llist *list)
{
    void *popped_data;
    struct node *head = *list;

    if (list == NULL || head->data == NULL)
        return NULL;
    
    popped_data = head->data;
    *list = head->next;

    free(head);

    return popped_data;
}
/**
 * @brief Prints the elements in the linked list using the provided print function.
 * @param list Pointer to the linked list.
 * @param print Function pointer to the printing function for elements.
 */
void llist_print(llist *list, void (*print)(void *))
{
    struct node *curr = *list;
    while (curr != NULL) {
        print(curr->data);
        printf(" ");
        curr = curr->next;
    }
    putchar('\n');
}
/**
 * @brief Deletes a node from the linked list at the specified position.
 * @param list Pointer to the linked list.
 * @param position Position of the node to be deleted.
 */
void llist_delete_at_position(llist *list, int position) 
{
    if(position == 0)
    {
        llist_delete_at_beginning(list);
        return;
    }
    if (list == NULL || *list == NULL) {
        fprintf(stderr, "llist_delete_at_position: list is null\n");
        return;
    }

    struct node *curr = *list;
    struct node *prev = NULL;
    int current_position = 0;

    // Traverse the list to find the node at the specified position
    while (curr != NULL && current_position != position) {
        prev = curr;
        curr = curr->next;
        current_position++;
    }

    // If the position is out of bounds, do nothing
    if (curr == NULL) {
        fprintf(stderr, "llist_delete_at_position: position out of bounds\n");
        return;
    }

    // Update pointers to skip the node at the specified position
    if (prev == NULL) {
        // If deleting the first node
        *list = curr->next;
    } else {
        // If deleting a node in the middle or at the end
        prev->next = curr->next;
    }

    // Free the memory occupied by the node
    free(curr);
}
/**
 * @brief Deletes the first node from the linked list.
 * @param list Pointer to the linked list.
 */
void llist_delete_at_beginning(llist *list) 
{
    if (list == NULL || *list == NULL) {
        fprintf(stderr, "llist_delete_at_beginning: list is null\n");
        return;
    }

    struct node *head = *list;
    *list = head->next; // Update the list pointer to skip the first node
    free(head); // Free the memory occupied by the first node
}
/**
 * @brief Deletes the last node from the linked list.
 * @param list Pointer to the linked list.
 */
void llist_delete_at_end(llist *list) {
    if (list == NULL || *list == NULL) {
        fprintf(stderr, "llist_delete_at_end: list is null\n");
        return;
    }

    struct node *curr = *list;
    struct node *prev = NULL;

    // Traverse the list until the last node
    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }

    // If the list has only one node, set the list pointer to NULL
    if (prev == NULL) {
        free(curr);
        *list = NULL;
    } else {
        // Update the second-to-last node's next pointer to NULL
        prev->next = NULL;
        free(curr); // Free the memory occupied by the last node
    }
}
/**
 * @brief Traverses the linked list and performs the specified operation on each node.
 * @param list Pointer to the linked list.
 * @param operation Function pointer to the operation to be performed on each node.
 */
void llist_traverse(llist *list, void (*operation)(void *)) 
{
    int node_index = 0;
    if (list == NULL || *list == NULL) {
        fprintf(stderr, "llist_traverse: list is null\n");
        return;
    }

    struct node *curr = *list;

    // Traverse the list and perform the operation on each node
    while (curr != NULL) {
        operation(curr->data);
        curr = curr->next;
        node_index++;
    }
}
/**
 * @brief Inserts a node with the given data at the end of the linked list.
 * @param list Pointer to the linked list.
 * @param data Pointer to the data to be inserted.
 */
void llist_insert_at_end(llist *list, void *data) 
{
    if (list == NULL) {
        fprintf(stderr, "llist_insert_at_end: list is null\n");
        return;
    }

    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    if (new_node == NULL) {
        fprintf(stderr, "llist_insert_at_end: memory allocation failed\n");
        return;
    }

    new_node->data = data;
    new_node->next = NULL;

    if (*list == NULL) {
        // If the list is empty, set the new node as the first node
        *list = new_node;
    } else {
        struct node *curr = *list;
        // Traverse the list until the last node
        while (curr->next != NULL) {
            curr = curr->next;
        }
        // Update the last node's next pointer to point to the new node
        curr->next = new_node;
        printf("Update node success\r\n");
    }
}