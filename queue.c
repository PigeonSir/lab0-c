#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define STACKSIZE 10000
/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *obj = malloc(sizeof(struct list_head));
    if (!obj)
        return NULL;
    INIT_LIST_HEAD(obj);
    return obj;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *pos, *n;
    list_for_each_entry_safe (pos, n, l, list) {
        q_release_element(pos);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    new->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, strlen(s) + 1);
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = malloc(strlen(s) + 1);
    strncpy(new->value, s, strlen(s) + 1);
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_element = list_entry(head->next, element_t, list);
    if (sp) {
        strncpy(sp, rm_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return rm_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ret = list_last_entry(head, element_t, list);
    if (sp) {
        memcpy(sp, ret->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return ret;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    int len = q_size(head);
    if (len == 1)
        return NULL;
    int mid = len / 2 - 1;
    struct list_head *tmp = head->next;
    while (mid > 0) {
        tmp = tmp->next;
        mid--;
    }
    tmp = tmp->next;
    list_del(tmp);
    q_release_element(list_entry(tmp, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    struct list_head *temp = head->next;
    while (temp != head && temp != head->prev) {
        if (!strcmp(list_entry(temp, element_t, list)->value,
                    list_entry(temp->next, element_t, list)->value))
            list_del(temp->next);
        temp = temp->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || !head->next)
        return;
    struct list_head *temp = head;
    head = head->next;
    temp->next = head->next;
    head->next = temp;
    if (head->next->next)
        q_swap(head->next->next);
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *cur = head->next, *temp;
    while (cur != head) {
        temp = cur->next;
        cur->next = cur->prev;
        cur->prev = temp;
    }
    temp = head->next;
    head->next = head->prev;
    head->prev = temp;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *merge_two(struct list_head *left, struct list_head *right)
{
    struct list_head *head = left;
    if (strcmp(list_entry(left, element_t, list)->value,
               list_entry(right, element_t, list)->value) > 0)
        list_move_tail(head = (right = right->next)->prev, left);
    struct list_head *tail = head->prev;
    while (left != tail && right->next != left) {
        int cmp = strcmp(list_entry(left, element_t, list)->value,
                         list_entry(right, element_t, list)->value);
        if (cmp <= 0)  // to keep sorting stable, split condition as <= , >
            left = left->next;
        else
            list_move_tail((right = right->next)->prev, left);
    }
    while (right->next != left && right->next != head) {
        int cmp = strcmp(list_entry(left, element_t, list)->value,
                         list_entry(right, element_t, list)->value);

        list_move_tail((right = right->next)->prev, cmp < 0 ? head : left);
        // Shoud be cmp <= 0 because of stability
    }
    return head;
}
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    int count = 0, n = q_size(head);
    struct list_head *s[STACKSIZE];

    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head)
        INIT_LIST_HEAD(s[count++] = cur);

    for (int size_each_list = 1; size_each_list < n; size_each_list *= 2) {
        for (int i = 0; i + size_each_list < n; i += size_each_list * 2) {
            struct list_head *left = s[i];
            struct list_head *right = s[i + size_each_list];
            s[i] = merge_two(left, right);
        }
    }
    list_add_tail(head, s[0]);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
