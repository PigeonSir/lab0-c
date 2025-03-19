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

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

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
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
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
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
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
        node = cmp(left, right, descend) > 0 ? &right : &left;
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
    struct list_head *l = head->next, *r = l->next;
    while (r != head) {
        queue_contex_t *L = list_entry(l, queue_contex_t, chain),
                       *R = list_entry(r, queue_contex_t, chain);
        L->q->prev->next = NULL;
        R->q->prev->next = NULL;
        L->q->next = mergeTwo(L->q->next, R->q->next, descend);
        L->size += R->size;
        INIT_LIST_HEAD(R->q);
        r = r->next;
    }
    queue_contex_t *node = list_entry(head->next, queue_contex_t, chain);
    struct list_head *cur = node->q->next, *prevPtr = node->q;
    while (cur) {
        cur->prev = prevPtr;
        prevPtr = cur;
        cur = cur->next;
    }
    node->q->prev = prevPtr;
    prevPtr->next = node->q;

    return node->size;
}

int cmp(struct list_head *a, struct list_head *b, bool descend)
{
    element_t *A = list_entry(a, element_t, list);
    element_t *B = list_entry(b, element_t, list);
    int ret = strcmp(A->value, B->value);
    return descend ? -ret : ret;
}


/* The Merge function for list_sort */
static struct list_head *merge(struct list_head *a,
                               struct list_head *b,
                               bool descend)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(a, b, descend) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}


static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b,
                        bool descend)
{
    struct list_head *tail = head;
    int count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(a, b, descend) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        if (unlikely(!++count))
            cmp(b, b, descend);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

void q_listSort(struct list_head *head, bool descend)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(b, a, descend);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(pending, list, descend);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(head, pending, list, descend);
}
