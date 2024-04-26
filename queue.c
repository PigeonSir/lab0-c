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
    if (!new->value) {
        free(new);
        return false;
    }
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
    if (!head)
        return;
    struct list_head *cur = head->next;
    while (cur != head && cur->next != head) {
        struct list_head *curN;
        curN = cur->next;
        list_move(cur, curN);
        cur = cur->next;
    }
}


/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *cur = head->next, *temp;
    while (cur != head) {
        temp = cur->next;
        cur->next = cur->prev;
        cur->prev = temp;
        cur = temp;
    }
    temp = head->next;
    head->next = head->prev;
    head->prev = temp;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (k == 1 || !head || list_empty(head))
        return;
    int count = 0;
    struct list_head *start = head, temp, *cur, *safe;
    INIT_LIST_HEAD(&temp);
    list_for_each_safe (cur, safe, head) {
        count++;
        if (count == k) {
            list_cut_position(&temp, start, cur);
            q_reverse(&temp);
            list_splice_init(&temp, start);
            count = 0;
            start = safe->prev;
        }
    }
}

struct list_head *merge_two(struct list_head *left, struct list_head *right)
{
    struct list_head *temp = NULL;
    struct list_head **indirect = &temp;
    for (struct list_head **node = NULL; left && right; *node = (*node)->next) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) > 0)
            node = &right;
        else
            node = &left;
        *indirect = *node;
        indirect = &(*indirect)->next;
    }
    if (left)
        *indirect = left;
    if (right)
        *indirect = right;

    return temp;
}

struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *fast = head, *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    fast = slow;
    slow->prev->next = NULL;
    struct list_head *left = mergesort(head), *right = mergesort(fast);
    return merge_two(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;
    element_t *curMax = list_entry(head->prev, element_t, list);
    struct list_head *temp = head->prev;

    while (temp != head) {
        element_t *cur = list_entry(temp, element_t, list);
        if (strcmp(curMax->value, cur->value) > 0) {
            temp = temp->prev;
            list_del(temp->next);
            q_release_element(cur);
        } else {
            curMax = cur;
            temp = temp->prev;
        }
    }

    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *c_entry, *c_safe,
        *ret = list_first_entry(head, queue_contex_t, chain);

    list_for_each_entry_safe (c_entry, c_safe, head, chain) {
        if (c_entry != ret) {
            list_splice_tail(c_entry->q, ret->q);
            ret->size += c_entry->size;
        }
    }
    ret->chain.next = head;
    head->prev = &(ret->chain);
    q_sort(ret->q);
    return ret->size;
}