#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct __element {
    struct list_head list;
    char value[];
} list_ele_t;

static list_ele_t *get_middle(struct list_head *list)
{
    struct list_head *fast = list->next, *slow;
    list_for_each(slow, list)
    {
        if (fast->next == list || fast->next->next == list)
            break;
        fast = fast->next->next;
    }
    return list_entry(slow, list_ele_t, list);
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);
    if (list_empty(lhs)) {
        list_splice_tail(lhs, head);
        return;
    }
    if (list_empty(rhs)) {
        list_splice_tail(rhs, head);
        return;
    }

    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = list_entry(lhs->next, list_ele_t, list)->value;
        char *rv = list_entry(rhs->next, list_ele_t, list)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}

void list_merge_sort(struct list_head *list)
{
    if (list_is_singular(list))
        return;

    struct list_head left;
    struct list_head sorted;
    INIT_LIST_HEAD(&left);
    list_cut_position(&left, list, &get_middle(list)->list);
    list_merge_sort(&left);
    list_merge_sort(list);
    list_merge(&left, list, &sorted);
    INIT_LIST_HEAD(list);
    list_splice_tail(&sorted, list);
}

static bool validate(struct list_head *list)
{
    struct list_head *node;
    list_for_each(node, list)
    {
        if (node->next == list)
            break;
        if (strcmp(list_entry(node, list_ele_t, list)->value,
                   list_entry(node->next, list_ele_t, list)->value) > 0)
            return false;
    }
    return true;
}


static void list_travesal(struct list_head *list)
{
    struct list_head *node;
    list_for_each(node, list)
    {
        if (node->next == list)
            break;
        printf("%s ", list_entry(node, list_ele_t, list)->value);
    }
    puts("\n");
}

static bool ele_insert_head(struct list_head *list, char *s)
{
    if (!list)
        return false;
    size_t length = strlen(s) + 1;
    list_ele_t *newh = malloc(sizeof(list_ele_t) + sizeof(char) * length);
    if (!newh)
        return false;

    strncpy(newh->value, s, length);
    list_add_tail(&newh->list, list);
    return true;
}

int main(void)
{
    FILE *fp = fopen("cities.txt", "r");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    struct list_head head;
    INIT_LIST_HEAD(&head);
    char buf[256];
    while (fgets(buf, 256, fp))
        ele_insert_head(&head, buf);
    fclose(fp);
    list_merge_sort(&head);
    list_travesal(&head);
    assert(validate(&head));

    return 0;
}
