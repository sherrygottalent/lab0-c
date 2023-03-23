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
        strncpy(sp, entry->value, bufsize);
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
        strncpy(sp, entry->value, bufsize);
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
    if (!head || list_empty(head))
        return false;

    int mid = q_size(head) / 2;
    element_t *ele, *safe;
    int idx = 0;
    list_for_each_entry_safe (ele, safe, head, list) {
        if (idx == mid) {
            safe = list_entry(ele->list.next, element_t, list);
            list_del(safe->list.prev);
            q_release_element(ele);
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
        struct list_head *ptr = itr->next, *last_ptr;
        bool is_dup = false;

        while (ptr != head) {
            if (strcmp(itr_ele->value,
                       list_entry(ptr, element_t, list)->value) == 0) {
                last_ptr = ptr;
                list_del(ptr);
                q_release_element(list_entry(ptr, element_t, list));
                ptr = last_ptr->next;
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
    if (!head || list_empty(head))
        return;

    struct list_head *curr = head->next, *last = head;
    while (curr) {
        struct list_head *tmp = NULL;
        last->next = curr->next;
        curr->prev = last->next;
        if (last->next->next) {
            tmp = last->next->next;
        }
        last->next->next = curr;
        last->next->prev = last;

        last = curr;
        if (tmp == head) {
            head->prev = last;
            last->next = head;
            break;
        }
        curr = tmp;
        if (curr->next == head) {
            last->next = curr;
            curr->prev = last;
            curr->next = head;
            head->prev = curr;
            break;
        }
    }
    return;
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
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    /*
    struct list_head *itr, sub_itr;
    element_t *first_ele = list_first_entry(head, element_t, list), *curr_ele;

    struct list_head *min_ptr = head->next;
    element_t *min_ele = list_entry(min_ptr, element_t, list);
    // find min
    list_for_each(itr, head){
        curr_ele = list_entry(itr, element_t, list);
        if (curr_ele->value<min_ele->value){
            min_ptr = itr;
            min_ele = list_entry(min_ptr, element_t, list);
        }
    }
    // sort
    itr = head->next;
    while(itr!=min_ptr){

    }
    */
    return;
}

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
    printf("result len = %d\n", len);
    return ++len;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    return 0;
}
