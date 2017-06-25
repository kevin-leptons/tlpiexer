/*
NAME

    Linking list

SYNOPSIS

    int ll_init(struct llist *list);
    int ll_free(struct llist *list);
    int ll_add(struct llist *list, struct item_nv *item);
    int ll_get(struct llist *list, struct item_nv *item);
    int ll_set(struct llist *list, struct item_nv *item);
    int ll_del(struct llist *list, struct item_nv *item);

DESCRIPTION

    ll_init() create an empty list pointed by list.

    ll_free() release dynamic memory allocation.

    ll_add() add item into list. If item with name is early exist, return
    error. Item argument will be copy in new memory, it mean that doesn't use
    memory was pointed by item.

    ll_get() retrieve item from list. If item with name isn't exist, return
    error.

    ll_set() update exist item to new value.

    ll_del() remove item from list. item->value will be ignore.

    ll_set(), ll_del() required same own to perform operation. If item->own 
    isn't match with own of current item, return error.

RETURN

    On success return FN_OK. On fail return FN_ER and lerrno set to 
    error number.

ERROR

    EL_PERM
    EL_EXIST
    EL_NOEXT

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#ifndef __TLPIEXER_59_4_LLIST_H__
#define __TLPIEXER_59_4_LLIST_H__

#include <stdlib.h>

#define EL_PERM 1
#define EL_EXIST 2
#define EL_NOEXT 3

#define LL_NAME_LEN 16
#define LL_VALUE_LEN 32
#define LL_USRNAME_LEN 32

size_t lerrno = 0;

struct item_nv
{
    char name[LL_NAME_LEN];
    char value[LL_VALUE_LEN];
    char own[LL_USRNAME_LEN];
};

struct item_ll
{
    struct item_nv nv;
    struct item_ll *next;
    struct item_ll *prev;
};

struct llist
{
    size_t size;
    struct item_ll *first;
};

int ll_init(struct llist *list);
int ll_free(struct llist *list);
int ll_add(struct llist *list, struct item_nv *item);
int ll_get(struct llist *list, struct item_nv *item);
int ll_set(struct llist *list, struct item_nv *item);
int ll_del(struct llist *list, struct item_nv *item);

#endif
