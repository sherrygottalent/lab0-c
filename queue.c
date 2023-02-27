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
    struct list_head *head = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (node == NULL)
        return false;
    node->value = strndup(s, strlen(s));

    struct list_head *next = head->next;
    next->prev = &node->list;
    node->list.next = next;
    node->list.prev = head;
    head->next = &(node->list);

    if (list_empty(head))
        return false;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if ((head == NULL) || list_empty(head))
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (node == NULL)
        return false;
    node->value = strndup(s, strlen(s));

    struct list_head *prev = head->prev;
    prev->next = &node->list;
    node->list.next = head;
    node->list.prev = prev;
    head->prev = &(node->list);

    if (list_empty(head))
        return false;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if ((head == NULL) || list_empty(head))
        return NULL;

    element_t *rm_ele = list_first_entry(head, element_t, list);
    head->next = rm_ele->list.next;
    list_entry(rm_ele->list.next, element_t, list)->list.prev =
        rm_ele->list.prev;
    list_del(&rm_ele->list);

    if ((sp != NULL) && (sp[0] == '\0')) {
        strncpy(sp, rm_ele->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return rm_ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if ((head == NULL) || list_empty(head))
        return NULL;

    element_t *rm_ele = list_last_entry(head, element_t, list);
    head->prev = rm_ele->list.prev;
    list_entry(rm_ele->list.prev, element_t, list)->list.next = head;
    list_del(&(rm_ele->list));

    if (sp != NULL)
        strncpy(sp, rm_ele->value, bufsize);
    return rm_ele;
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

    int mid = q_size(head) / 2;
    element_t *ele, *safe;
    int idx = 0;
    list_for_each_entry_safe (ele, safe, head, list) {
        if (idx == mid) {
            ele->list.prev->next = ele->list.next;
            ele->list.next->prev = ele->list.prev;
            free(ele);
            break;
        }
        idx++;
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *itr, *itr_safe;
    list_for_each_safe (itr, itr_safe, head) {
        element_t *itr_ele = list_entry(itr, element_t, list);
        struct list_head *ptr = itr->next;
        bool is_dup = false;

        while (ptr != head) {
            if (strcmp(itr_ele->value,
                       list_entry(ptr, element_t, list)->value) == 0) {
                element_t *next_ele = list_entry(ptr->next, element_t, list);
                if (ptr == itr->next) {
                    // adjacent node
                    itr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                } else {
                    itr->prev->next = itr->next;
                    itr->next->prev = itr->prev;
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                }
                list_del(ptr);
                q_release_element(list_entry(ptr, element_t, list));
                ptr = &next_ele->list;
                is_dup = true;
                continue;
            }
            ptr = ptr->next;
        }
        itr_safe = itr->next;
        if (is_dup) {
            list_del(itr);
            q_release_element(list_entry(itr, element_t, list));
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    struct list_head *itr;
    list_for_each (itr, head) {
        tmp = itr->next;
        itr->next = itr->prev;
        itr->prev = tmp;
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    if (q_size(head) == 1)
        return 1;

    int len = 0;
    struct list_head *itr = head->prev;
    element_t *itr_ele = list_last_entry(head, element_t, list);
    while (itr != head && itr->prev != head) {
        element_t *prev_ele = list_entry(itr->prev, element_t, list);
        while (atoi(prev_ele->value) < atoi(itr_ele->value)) {
            // remove node
            list_del(itr->prev);

            itr_ele = list_entry(itr, element_t, list);
            if (itr->prev == head) {
                break;
            }
            prev_ele = list_entry(itr->prev, element_t, list);
        }
        itr = itr->prev;
        if (itr == head)
            break;
        itr_ele = list_entry(itr, element_t, list);
        len += 1;
    }
    return ++len;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
