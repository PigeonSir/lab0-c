#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *pos, *n;
    list_for_each_entry_safe (pos, n, head, list) {
        q_release_element(n);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    int len = strlen(s);
    new->value = malloc(len + 1);
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, len);
    new->value[len] = '\0';
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    int len = strlen(s);
    new->value = malloc(len + 1);
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, len);
    new->value[len] = '\0';
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *obj = list_first_entry(head, element_t, list);
    if (!sp)
        return NULL;
    strncpy(sp, obj->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(head->next);
    return obj;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *obj = list_last_entry(head, element_t, list);
    if (!sp)
        return NULL;
    strncpy(sp, obj->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(head->prev);
    return obj;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int ret = 0;
    struct list_head *li;
    list_for_each (li, head)
        ret++;
    return ret;
}


/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head **temp = &head->next;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        temp = &(*temp)->next;
    }
    struct list_head *del = *temp;
    list_del(del);
    q_release_element(list_entry(del, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;
    element_t *pos, *safe, *temp = NULL;
    list_for_each_entry_safe (pos, safe, head, list) {
        if (&safe->list != head && strlen(pos->value) == strlen(safe->value) &&
            !strncmp(pos->value, safe->value, strlen(safe->value))) {
            temp = safe;
            list_del(&pos->list);
            q_release_element(pos);
        } else if (temp) {
            list_del(&temp->list);
            q_release_element(temp);
            temp = NULL;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head) {}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
