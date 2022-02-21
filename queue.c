#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *queue = malloc(sizeof(struct list_head));
    if (!queue)
        return NULL;
    INIT_LIST_HEAD(queue);
    return queue;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *tmp = l->next;
    while (!list_empty(l)) {
        list_del(tmp);
        element_t *ele = container_of(tmp, element_t, list);
        free(ele->value);
        free(ele);
        tmp = l->next;
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = malloc(strlen(s) + 1);
    if (!ele->value) {
        free(ele);
        return false;
    }
    strncpy(ele->value, s, strlen(s) + 1);
    list_add(&ele->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele)
        return false;
    ele->value = malloc(strlen(s) + 1);
    if (!ele->value) {
        free(ele);
        return false;
    }
    strncpy(ele->value, s, strlen(s) + 1);
    list_add_tail(&ele->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *target = head->next;
    element_t *tmp = container_of(target, element_t, list);
    list_del_init(target);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return tmp;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *target = head->prev;
    element_t *tmp = container_of(target, element_t, list);
    list_del_init(target);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return tmp;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *node;
    int count = 0;
    list_for_each (node, head)
        count++;
    return count;
}

/*
 * https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *node;
    int count = 0, mid = 0;
    list_for_each (node, head)
        count++;
    mid = (float) count / 2 > count / 2 ? count / 2 + 1 : count / 2;
    count = 0;
    element_t *ele;
    list_for_each_entry (ele, head, list) {
        if (++count == mid) {
            list_del(&ele->list);
            free(ele->value);
            free(ele);
            break;
        }
    }
    return true;
}

/*
 * https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    if (list_empty(head) || list_is_singular(head))
        return true;
    struct list_head *start = head->next, *end = start->next;
    while (start != head) {
        element_t *ele1 = container_of(start, element_t, list);
        element_t *ele2 = container_of(end, element_t, list);
        bool found = false;
        while (strcmp(ele1->value, ele2->value) == 0) {
            found = true;
            end = end->next;
            if (end == head)
                break;
            ele2 = container_of(end, element_t, list);
        }
        while (start != end) {
            struct list_head *next = start->next;
            if (found) {
                list_del(start);
                element_t *tmp = container_of(start, element_t, list);
                free(tmp->value);
                free(tmp);
            }
            start = next;
        }
        end = start->next;
    }
    return true;
}

/*
 * https://leetcode.com/problems/swap-nodes-in-pairs/
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *first = head->next, *second = first->next;
    element_t *ele1, *ele2;
    while (first != head && second != head) {
        ele1 = container_of(first, element_t, list);
        ele2 = container_of(second, element_t, list);
        char *tmp = ele1->value;
        ele1->value = ele2->value;
        ele2->value = tmp;
        first = first->next->next;
        second = second->next->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *ptr1 = head, *ptr2 = ptr1->next;
    do {
        ptr2->prev = ptr2->next;
        ptr2->next = ptr1;
        ptr1 = ptr2;
        ptr2 = ptr2->prev;
    } while (ptr1 != head);
}

/* merge 2 list in ascending order */
struct list_head *merge(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node = NULL;
    for (element_t *ele1, *ele2; L1 && L2; *node = (*node)->next) {
        ele1 = container_of(L1, element_t, list);
        ele2 = container_of(L2, element_t, list);
        node = strcmp(ele1->value, ele2->value) < 0 ? &L1 : &L2;
        *ptr = *node;
        ptr = &(*node)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}

/* perform merge sort algorithm on a list */
struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *right = head->next;
    struct list_head *left = head;

    // split list
    while (right && right->next) {
        left = left->next;
        right = right->next->next;
    }
    right = left->next;
    left->next = NULL;

    // sort each list
    struct list_head *l1 = merge_sort(head);
    struct list_head *l2 = merge_sort(right);

    // merge sorted l1 and sorted l2
    return merge(l1, l2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    // set tail to NULL for the convenience of terminating the iteration
    head->prev->next = NULL;

    // merge sort
    head->next = merge_sort(head->next);

    // set prev of each node, and connect head and tail
    struct list_head *tmp;
    for (tmp = head; tmp->next; tmp = tmp->next)
        tmp->next->prev = tmp;
    tmp->next = head;
    head->prev = tmp;
}
