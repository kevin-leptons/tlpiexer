#include <tlpiexer/59-04-llist.h>

#include <string.h>

#include <tlpiexer/error.h>

int ll_init(struct llist *list)
{
    list->first = NULL;
    list->size = 0;
    return FN_OK;
}

int ll_free(struct llist *list)
{
    struct item_ll *item;

    for (item = list->first; item != NULL; item = item->next)
        free(item);

    return FN_OK;
}

int ll_add(struct llist *list, struct item_nv *item)
{
    struct item_ll *litem;

    // item with name is early exist
    for (litem = list->first; litem != NULL; litem = litem->next) {
        if (strncmp(litem->nv.name, item->name, sizeof(item->name)) == 0)
            break;
    }
    if (litem != NULL) {
        lerrno = EL_EXIST;
        return FN_ER;
    }

    // make new memory block for list item
    litem = malloc(sizeof(*litem));
    if (litem == NULL)
        return FN_ER;
    litem->prev = NULL;
    memcpy(&litem->nv, item, sizeof(litem->nv));

    // insert first item
    if (list->first == NULL) {
        list->first = litem;
        litem->next = NULL;
        return FN_OK;
    }

    // insert not first item
    litem->next = list->first;
    list->first->prev = litem;
    list->first = litem;
    return FN_OK;
}

int ll_get(struct llist *list, struct item_nv *item)
{
    struct item_ll *litem;

    for (litem = list->first; litem != NULL; litem = litem->next) {
        if (strncmp(litem->nv.name, item->name, sizeof(item->name)) == 0)
            break;
    }

    // not exist
    if (litem == NULL) {
        lerrno = EL_NOEXT;
        return FN_ER;
    }

    // exist
    memcpy(item, &litem->nv, sizeof(*item));
    return FN_OK;
};

int ll_set(struct llist *list, struct item_nv *item)
{
    struct item_ll *litem;

    // find item
    for (litem = list->first; litem != NULL; litem = litem->next) {
        if (strncmp(litem->nv.name, item->name, sizeof(item->name)) == 0)
            break;
    }

    // not exist
    if (litem == NULL) {
        lerrno = EL_NOEXT;
        return FN_ER;
    }

    // invalid permision
    if (strncmp(litem->nv.own, item->own, sizeof(item->own)) != 0) {
        lerrno = EL_PERM;
        return FN_ER;
    }

    // update value
    strcpy(litem->nv.value, item->value);

    return FN_OK;
}

int ll_del(struct llist *list, struct item_nv *item)
{
    struct item_ll *litem;

    for (litem = list->first; litem != NULL; litem = litem->next) {
        if (strncmp(litem->nv.name, item->name, sizeof(item->name)) == 0)
            break;
    }

    // not exist
    if (litem == NULL) {
        lerrno = EL_NOEXT;
        return FN_ER;
    }

    // invalid permision
    if (strncmp(litem->nv.own, item->own, sizeof(item->own)) != 0) {
        lerrno = EL_PERM;
        return FN_ER;
    }

    if (litem->prev == NULL) {
        // remove first item
        list->first = litem->next;
        if (list->first != NULL)
            list->first->prev = NULL;
    } else {
        // remove non first item
        litem->prev->next = litem->next;
        if (litem->next != NULL)
            litem->next->prev = litem->prev;
    }

    // free memory
    free(litem);

    return FN_OK;
}
