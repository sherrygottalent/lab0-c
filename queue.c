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
    if (!list_empty(l)) {
        struct list_head *node, *safe;
        list_for_each_safe (node, safe, l) {
            element_t *entry = list_entry(node, element_t, list);
            q_release_element(entry);
        }
    }
    free(l);
    return;
}

element_t *element_new(const char *s)
{
    element_t *entry = malloc(sizeof(element_t));
    if (entry == NULL)
        return NULL;
    size_t buflen = strlen(s) + 1;
    entry->value = malloc(sizeof(char) * buflen);
    strncpy(entry->value, s, buflen);
    return entry;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *entry = element_new(s);
    if (entry == NULL)
        return false;

    list_add(&entry->list, head);
    if (list_empty(head))
        return false;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if ((head == NULL) || list_empty(head))
        return false;

    element_t *entry = element_new(s);
    if (entry == NULL)
        return false;

    list_add_tail(&entry->list, head);
    if (list_empty(head))
        return false;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;

    element_t *entry = list_first_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp != NULL) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;

    element_t *entry = list_last_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp != NULL) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int size = 0;
    if (head == NULL || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return 1;
    else {
        struct list_head *node;
        list_for_each (node, head)
            size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return false;

    int mid = (q_size(head) + 1) / 2;
    struct list_head *node;
    list_for_each (node, head) {
        if (--mid == 0) {
            element_t *del_entry = list_entry(node, element_t, list);
            list_del(node);
            q_release_element(del_entry);
            break;
        }
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return false;

    struct list_head *head_node = head->next, *head_safe;
    struct list_head *node, *safe;
    bool isDup = false;

    while (head_node != head && head_node->next != head) {
        element_t *head_entry = list_entry(head_node, element_t, list);
        list_for_each_safe (node, safe, head_node) {
            safe = node->next;
            element_t *entry = list_entry(node, element_t, list);
            if (!strcmp(head_entry->value, entry->value)) {
                list_del(node);
                q_release_element(entry);
                isDup = true;
            }
            if (safe == head)
                break;
        }

        head_safe = head_node->next;
        if (isDup) {
            list_del(head_node);
            q_release_element(head_entry);
            isDup = false;
        }
        head_node = head_safe;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *node = head->next, *next_node = head->next->next;
    while (node != head && next_node != head) {
        struct list_head *_prev = node->prev;
        struct list_head *_next = next_node->next;
        _prev->next = next_node;
        next_node->next = node;
        next_node->prev = _prev;
        node->next = _next;
        node->prev = next_node;
        _next->prev = node;

        node = _next;
        next_node = _next->next;
    }

    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;

    struct list_head *node;
    list_for_each (node, head) {
        tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    if (k <= 1 || k > q_size(head))
        return;
    /*
    struct list_head *curr = head->next, *last = head;
    struct list_head *grp_start = NULL;
    struct list_head *grp_end = NULL;
    int cnt = 0;
    while (curr != head) {
        printf("[%s]\n", list_entry(curr, element_t, list)->value);
        if (cnt%k == 0) {
            if (cnt/k != 0) {
                // reverse last k element

                struct list_head *local_curr = grp_start;//, *itr_curr =
    grp_start;
                //struct list_head *end = grp_end;//, *itr_curr = grp_start;
                while (local_curr != grp_end){//} && grp_end != grp_start) {
                    struct list_head *tmp = local_curr;
                    local_curr->prev = tmp->next;
                    local_curr->next = tmp->prev;

                    //struct list_head *tmp = local_curr->next->next;
                    //local_curr->next = local_curr->prev;
                    //local_curr->next->next = local_curr;
                    //local_curr->prev = tmp->prev;
                    local_curr = local_curr->prev;
                }

                // update last
                last = grp_start;
                printf("296 leave while last_ele = %s\n", list_entry(last,
    element_t, list)->value); grp_end->next = curr; //curr; printf("289
    grp_end(%s)->next = curr(%s)\n", list_entry(grp_end, element_t,
    list)->value, list_entry(curr, element_t, list)->value);

            }
            grp_start = curr;
        }
        else if(cnt%k == k-1){
            grp_end = curr;
            last->next = grp_end;
        }

        curr = curr->next;
        cnt++;
    }

    int tmpcnt = 0;
    while(head->next!=head && tmpcnt<10){
        printf("%s->", list_entry(head->next, element_t, list)->value);
        tmpcnt++;
    }
    printf("\n\n");

    // update head->prev
    printf("312 - curr->next(%s), curr->prev(%s)\n",
                        list_entry(curr->prev, element_t, list)->value,
                        list_entry(curr->next, element_t, list)->value);

    head->prev = cnt%k == k-1 ? curr->prev : grp_start;
    curr->next = head;
    printf("312 - head->prev(%s)\n", list_entry(head->prev, element_t,
    list)->value); printf("3122 - head->next(%s)\n", list_entry(head->next,
    element_t, list)->value);


    //struct list_head *itr = head->next;
    //while(itr!=head){
    //    printf("[%s]", list_entry(itr, element_t, list)->value);
    //}
    */
    return;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    /* Selection Sort */
    struct list_head *node;
    element_t *local_min_element;
    int n_sorted = 0;
    int size = q_size(head);
    while (n_sorted != size) {
        int i = 0;
        local_min_element = list_first_entry(head, element_t, list);
        list_for_each (node, head) {
            if (i == size - n_sorted)
                break;
            element_t *local_element = list_entry(node, element_t, list);
            local_min_element =
                strcmp(local_element->value, local_min_element->value) <= 0
                    ? local_element
                    : local_min_element;
            i++;
        }
        list_move_tail(&local_min_element->list, head);
        n_sorted++;
    }
    return;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        safe = node->next;
        element_t *entry = list_entry(node, element_t, list);
        bool hasGreater = false;
        struct list_head *tail_node = head->prev;
        while (tail_node != node) {
            element_t *tail_entry = list_entry(tail_node, element_t, list);
            if (strcmp(tail_entry->value, entry->value)) {
                hasGreater = true;
                break;
            }
            tail_node = tail_node->prev;
        }
        if (hasGreater) {
            list_del(node);
            q_release_element(entry);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    return 0;
}
