#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

#define list_for_each_entry_safe_backward(entry, safe, head, member)   \
    for (entry = list_entry((head)->prev, typeof(*entry), member),     \
        safe = list_entry(entry->member.prev, typeof(*entry), member); \
         &entry->member != (head); entry = safe,                       \
        safe = list_entry(safe->member.prev, typeof(*entry), member))

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
        q_release_element(pos);
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
void q_swap(struct list_head *head)
{
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    q_reverseK(head, q_size(head));
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_is_singular(head) || k == 1)
        return;
    struct list_head *start = head, *end = head->next;
    while (end != head) {
        int i = 1;
        while (end != head && i < k) {
            end = end->next;
            i++;
        }
        if (end == head || i < k)
            break;
        struct list_head *temp = end->next;
        start->next->prev = NULL;
        while (end) {
            struct list_head *node = end->prev;
            start->next = end;
            end->prev = start;
            start = start->next;
            end = node;
        }
        start->next = temp;
        temp->prev = start;
        end = temp;
    }
}

struct list_head *mergeTwo(struct list_head *left,
                           struct list_head *right,
                           bool descend)
{
    struct list_head *temp = NULL;
    struct list_head **indirect = &temp;
    for (struct list_head **node = NULL; left && right; *node = (*node)->next) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) > 0)
            node = descend ? &left : &right;
        else
            node = descend ? &right : &left;
        *indirect = *node;
        indirect = &(*indirect)->next;
    }
    if (left)
        *indirect = left;
    if (right)
        *indirect = right;

    return temp;
}

struct list_head *mergesort(struct list_head *head, bool descend)
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
    struct list_head *left = mergesort(head, descend),
                     *right = mergesort(fast, descend);
    return mergeTwo(left, right, descend);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return;
    head->prev->next = NULL;
    head->next = mergesort(head->next, descend);
    struct list_head *cur = head->next, *prePtr = head;
    while (cur) {
        cur->prev = prePtr;
        prePtr = cur;
        cur = cur->next;
    }
    head->prev = prePtr;
    prePtr->next = head;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    element_t *pos, *n;
    char *min = NULL;
    list_for_each_entry_safe_backward(pos, n, head, list)
    {
        if (!min) {
            min = pos->value;
            continue;
        }
        if (strcmp(min, pos->value) >= 0) {
            min = pos->value;
        } else {
            list_del(&pos->list);
            q_release_element(pos);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    element_t *pos, *n;
    char *max = NULL;
    list_for_each_entry_safe_backward(pos, n, head, list)
    {
        if (!max) {
            max = pos->value;
            continue;
        }
        if (strcmp(max, pos->value) <= 0) {
            max = pos->value;
        } else {
            list_del(&pos->list);
            q_release_element(pos);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return q_size(head);
    struct list_head *l, *r;
    while (!list_is_singular(head)) {
        l = head, r = head->prev;
        while (l != r && l->next != r) {
            l = l->next;
            queue_contex_t *L = list_entry(l, queue_contex_t, chain),
                           *R = list_entry(r, queue_contex_t, chain);
            L->q->prev->next = NULL;
            R->q->prev->next = NULL;
            L->q = mergeTwo(L->q->next, R->q->next, descend);
            L->size += R->size;
            R->q = NULL;
            r = r->prev;
            list_del(r->next);
        }
    }
    return list_first_entry(head, queue_contex_t, chain)->size;
}
