#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "list_sort.h"

typedef struct __element {
    struct list_head list;
    char value[];
} list_ele_t;


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
    int length = strlen(s) + 1;
    if (length <= 0)
        return false;

    list_ele_t *newh = malloc(sizeof(list_ele_t) + sizeof(char) * length);
    if (!newh)
        return false;

    strcpy(newh->value, s);

    list_add_tail(&newh->list, list);
    return true;
}

static void list_free(struct list_head *list)
{
    if (list_empty(list))
        return;
    struct list_head *node, *tmp;
    list_for_each_safe(node, tmp, list)
    {
        list_del(node);
        free(list_entry(node, list_ele_t, list));
    }
}

static int str_cmp(void *priv, struct list_head *a, struct list_head *b)
{
    char *lv = list_entry(a, list_ele_t, list)->value;
    char *rv = list_entry(b, list_ele_t, list)->value;
    return strcmp(lv, rv) > 0;
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
    list_sort(NULL, &head, str_cmp);
    list_travesal(&head);
    assert(validate(&head));
    list_free(&head);

    return 0;
}
