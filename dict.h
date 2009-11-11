/* Copyright 2009
 * Kaz Kylheku <kkylheku@gmail.com>
 * Vancouver, Canada
 * All rights reserved.
 *
 * BSD License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior
 *      written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef DICT_H
#define DICT_H

#include <limits.h>
#ifdef KAZLIB_SIDEEFFECT_DEBUG
#include "sfx.h"
#endif

/*
 * Blurb for inclusion into C++ translation units
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long dictcount_t;
#define DICTCOUNT_T_MAX ULONG_MAX

/*
 * The dictionary is implemented as a red-black tree
 */

typedef enum { dnode_red, dnode_black } dnode_color_t;

typedef struct dnode_t {
    #if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    struct dnode_t *dict_left;
    struct dnode_t *dict_right;
    struct dnode_t *dict_parent;
    dnode_color_t dict_color;
    const void *dict_key;
    void *dict_data;
    #else
    int dict_dummy;
    #endif
} dnode_t;

typedef int (*dict_comp_t)(const void *, const void *);
typedef dnode_t *(*dnode_alloc_t)(void *);
typedef void (*dnode_free_t)(dnode_t *, void *);

typedef struct dict_t {
    #if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    dnode_t dict_nilnode;
    dictcount_t dict_nodecount;
    dictcount_t dict_maxcount;
    dict_comp_t dict_compare;
    dnode_alloc_t dict_allocnode;
    dnode_free_t dict_freenode;
    void *dict_context;
    int dict_dupes;
    #else
    int dict_dummmy;
    #endif
} dict_t;

typedef void (*dnode_process_t)(dict_t *, dnode_t *, void *);

typedef struct dict_load_t {
    #if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    dict_t *dict_dictptr;
    dnode_t dict_nilnode;
    #else
    int dict_dummmy;
    #endif
} dict_load_t;

extern dict_t *dict_create(dictcount_t, dict_comp_t);
extern void dict_set_allocator(dict_t *, dnode_alloc_t, dnode_free_t, void *);
extern void dict_destroy(dict_t *);
extern void dict_free_nodes(dict_t *);
extern void dict_free(dict_t *);
extern dict_t *dict_init(dict_t *, dictcount_t, dict_comp_t);
extern void dict_init_like(dict_t *, const dict_t *);
extern int dict_verify(dict_t *);
extern int dict_similar(const dict_t *, const dict_t *);
extern dnode_t *dict_lookup(dict_t *, const void *);
extern dnode_t *dict_lower_bound(dict_t *, const void *);
extern dnode_t *dict_upper_bound(dict_t *, const void *);
extern dnode_t *dict_strict_lower_bound(dict_t *, const void *);
extern dnode_t *dict_strict_upper_bound(dict_t *, const void *);
extern void dict_insert(dict_t *, dnode_t *, const void *);
extern dnode_t *dict_delete(dict_t *, dnode_t *);
extern int dict_alloc_insert(dict_t *, const void *, void *);
extern void dict_delete_free(dict_t *, dnode_t *);
extern dnode_t *dict_first(dict_t *);
extern dnode_t *dict_last(dict_t *);
extern dnode_t *dict_next(dict_t *, dnode_t *);
extern dnode_t *dict_prev(dict_t *, dnode_t *);
extern dictcount_t dict_count(dict_t *);
extern int dict_isempty(dict_t *);
extern int dict_isfull(dict_t *);
extern int dict_contains(dict_t *, dnode_t *);
extern void dict_allow_dupes(dict_t *);
extern int dnode_is_in_a_dict(dnode_t *);
extern dnode_t *dnode_create(void *);
extern dnode_t *dnode_init(dnode_t *, void *);
extern void dnode_destroy(dnode_t *);
extern void *dnode_get(dnode_t *);
extern const void *dnode_getkey(dnode_t *);
extern void dnode_put(dnode_t *, void *);
extern void dict_process(dict_t *, void *, dnode_process_t);
extern void dict_load_begin(dict_load_t *, dict_t *);
extern void dict_load_next(dict_load_t *, dnode_t *, const void *);
extern void dict_load_end(dict_load_t *);
extern void dict_merge(dict_t *, dict_t *);

#if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
#ifdef KAZLIB_SIDEEFFECT_DEBUG
#define dict_isfull(D) (SFX_CHECK(D)->dict_nodecount == (D)->dict_maxcount)
#else
#define dict_isfull(D) ((D)->dict_nodecount == (D)->dict_maxcount)
#endif
#define dict_count(D) ((D)->dict_nodecount)
#define dict_isempty(D) ((D)->dict_nodecount == 0)
#define dnode_get(N) ((N)->dict_data)
#define dnode_getkey(N) ((N)->dict_key)
#define dnode_put(N, X) ((N)->dict_data = (X))
#endif

#ifdef __cplusplus
}

#include <functional>

namespace kazlib
{
    class dnode : public dnode_t {
    public:
        dnode() { dnode_init(this, 0); }
        dnode(const dnode &) { dnode_init(this, 0); }
        dnode &operator = (const dnode &) { return *this; }
        bool is_in_a_dict()
        {
            return dnode_is_in_a_dict(this);
        }
    };

    template <typename KEY>
    int default_compare(const KEY &left, const KEY &right)
    {
        if (left < right)
            return -1;
        else if (left == right)
            return 0;
        else
            return 1;
    }

    template <
        class NODE_METHOD,
        class KEY_METHOD,
        class COMPARE_METHOD,
        bool ALLOW_DUPES = false
    >
    class dict_base : public dict_t {
    private:
        dict_base(const dict_base &);
        void operator = (const dict_base &);
    protected:
        typedef typename KEY_METHOD::KEY_TYPE KEY;
        typedef typename NODE_METHOD::ITEM_TYPE ITEM;

        dict_base(dictcount_t count)
        {
            dict_init(this, count, &COMPARE_METHOD::compare);
            if (ALLOW_DUPES)
                dict_allow_dupes(this);
        }
        static dnode_t *item2dnode(ITEM *item)
        {
            return NODE_METHOD::item2dnode(item);
        }
        static ITEM *dnode2item(dnode_t *dnode)
        {
            return NODE_METHOD::dnode2item(dnode);
        }
        static void *item2key(ITEM *item)
        {
            return KEY_METHOD::item2key(item);
        }
    public:
        dictcount_t count() 
        {
            return dict_count(this);
        }
        void insert(ITEM *pitem)
        {
            dict_insert(this, item2dnode(pitem), item2key(pitem));
        }
        void insert(ITEM &item)
        {
            insert(&item);
        }
        void erase(ITEM *pitem)
        {
            dict_delete(this, item2dnode(pitem));
        }
        void erase(ITEM &item)
        {
            erase(&item);
        }
        ITEM *lookup(const KEY *pkey)
        {
            return dnode2item(dict_lookup(this, pkey));
        }
        ITEM *lookup(const KEY &key)
        {
            return lookup(&key);
        }
        ITEM *upper_bound(const KEY *pkey)
        {
            return dnode2item(dict_upper_bound(this, pkey));
        }
        ITEM *upper_bound(const KEY &key)
        {
            return upper_bound(&key);
        }
        ITEM *lower_bound(const KEY *pkey)
        {
            return dnode2item(dict_lower_bound(this, pkey));
        }
        ITEM *lower_bound(const KEY &key)
        {
            return lower_bound(&key);
        }
        ITEM *first()
        {
            return dnode2item(dict_first(this));
        }
        ITEM *last()
        {
            return dnode2item(dict_last(this));
        }
        ITEM *next(ITEM *pitem)
        {
            return dnode2item(dict_next(this, item2dnode(pitem)));
        }
        ITEM *next(ITEM &item)
        {
            return next(&item);
        }
        ITEM *prev(ITEM *pitem)
        {
            return dnode2item(dict_prev(this, item2dnode(pitem)));
        }
        ITEM *prev(ITEM &item)
        {
            return prev(&item);
        }
    };

    template <class ITEM, dnode ITEM::* DNODE_OFFSET>
    class dnode_is_member {
    private:
        dnode_is_member(const dnode_is_member &);
        void operator = (const dnode_is_member &);
    public:
        typedef ITEM ITEM_TYPE;
        static ITEM *dnode2item(dnode_t *node)
        {
            if (node == 0)
                return 0;
            const ptrdiff_t offset = (char *) &(((ITEM *) 0)->*DNODE_OFFSET)
                                     - ((char *) 0);
            return (ITEM *) (((char *) node) - offset);
        }
        static dnode_t *item2dnode(ITEM *item)
        {
            return &(item->*DNODE_OFFSET);
        }
    };

    template <class ITEM, typename KEY, KEY ITEM::* KEY_OFFSET>
    class key_is_member {
    private:
        key_is_member(const key_is_member &);
        void operator = (const key_is_member &);
    public:
        typedef KEY KEY_TYPE;
        static void *item2key(ITEM *item)
        {
            return &(item->*KEY_OFFSET);
        }
    };

    template <typename KEY, int (*COMP)(const KEY &, const KEY &)>
    class compare_with_function {
    private:
        compare_with_function(const compare_with_function &);
        void operator = (const compare_with_function &);
    public:
        static int compare(const void *left, const void *right)
        {
            return COMP(*(const KEY *) left, *(const KEY *) right);
        }
    };

    template <class ITEM>
    class dnode_is_base {
    private:
        dnode_is_base(const dnode_is_base &);
        void operator = (const dnode_is_base &);
    public:
        typedef ITEM ITEM_TYPE;
        static ITEM *dnode2item(dnode_t *node)
        {
            return static_cast<ITEM *>(node);
        }
        static dnode_t *item2dnode(ITEM *item)
        {
            return item;
        }
    };

    template <class ITEM>
    class key_is_base {
    private:
        key_is_base(const key_is_base &);
        void operator = (const key_is_base &);
    public:
        typedef ITEM KEY_TYPE;
        static void *item2key(ITEM *item)
        {
            return item;
        }
    };

    template <
        class ITEM,
        typename KEY,
        dnode ITEM::* DNODE_OFFSET,
        KEY ITEM::* KEY_OFFSET,
        bool ALLOW_DUPES = false,
        int (*COMP)(const KEY &, const KEY &) = default_compare
    >
    class dict : public dict_base<dnode_is_member<ITEM, DNODE_OFFSET>,
                                  key_is_member<ITEM, KEY, KEY_OFFSET>,
                                  compare_with_function<KEY, COMP>,
                                  ALLOW_DUPES>
    {
    private:
        dict(const dict &);
        void operator = (const dict &);
    public:
        dict(dictcount_t dict_count = DICTCOUNT_T_MAX)
        : dict_base<dnode_is_member<ITEM, DNODE_OFFSET>,
                    key_is_member<ITEM, KEY, KEY_OFFSET>,
                    compare_with_function<KEY, COMP>,
                    ALLOW_DUPES>(dict_count)
        {
        }
    };

    template <
        class ITEM,
        bool ALLOW_DUPES = false,
        int (*COMP)(const ITEM &, const ITEM &) = default_compare
    >
    class dict_bdbk : public dict_base<dnode_is_base<ITEM>,
                                       key_is_base<ITEM>,
                                       compare_with_function<ITEM, COMP>,
                                       ALLOW_DUPES>
    {
    private:
        dict_bdbk(const dict_bdbk &);
        void operator = (const dict_bdbk &);
    public:
        dict_bdbk(dictcount_t dict_count = DICTCOUNT_T_MAX)
        : dict_base<dnode_is_base<ITEM>,
                    key_is_base<ITEM>,
                    compare_with_function<ITEM, COMP>,
                    ALLOW_DUPES>(dict_count)
        {
        }
    };

    template <
        class ITEM,
        typename KEY,
        KEY ITEM::* KEY_OFFSET,
        bool ALLOW_DUPES = false,
        int (*COMP)(const KEY &, const KEY &) = default_compare
    >
    class dict_bdmk : public dict_base<dnode_is_base<ITEM>,
                                       key_is_member<ITEM, KEY, KEY_OFFSET>,
                                       compare_with_function<ITEM, COMP>,
                                       ALLOW_DUPES>
    {
        dict_bdmk(const dict_bdmk &);
        void operator = (const dict_bdmk &);
    private:
        dict_bdmk(dictcount_t dict_count = DICTCOUNT_T_MAX)
        : dict_base<dnode_is_base<ITEM>,
                    key_is_member<ITEM, KEY, KEY_OFFSET>,
                    compare_with_function<ITEM, COMP>,
                    ALLOW_DUPES>(dict_count)
        {
        }
    };

    template <
        class ITEM,
        typename KEY,
        dnode ITEM::* DNODE_OFFSET,
        bool ALLOW_DUPES = false,
        int (*COMP)(const ITEM &, const ITEM &) = default_compare
    >
    class dict_mdbk : public dict_base<dnode_is_member<ITEM, DNODE_OFFSET>,
                                       key_is_base<ITEM>,
                                       compare_with_function<ITEM, COMP>,
                                       ALLOW_DUPES>
    {
        dict_mdbk(const dict_mdbk &);
        void operator = (const dict_mdbk &);
    private:
        dict_mdbk(dictcount_t dict_count = DICTCOUNT_T_MAX)
        : dict_base<dnode_is_member<ITEM, DNODE_OFFSET>,
                    key_is_base<ITEM>,
                    compare_with_function<ITEM, COMP>,
                    ALLOW_DUPES>(dict_count)
        {
        }
    };

    template <
        class ITEM,
        typename KEY,
        dnode ITEM::* DNODE_OFFSET,
        KEY ITEM::* KEY_OFFSET,
        bool ALLOW_DUPES = false,
        int (*COMP)(const KEY &, const KEY &) = default_compare
    >
    class dict_mdmk : public dict_base<dnode_is_member<ITEM, DNODE_OFFSET>,
                                       key_is_member<ITEM, KEY, KEY_OFFSET>,
                                       compare_with_function<KEY, COMP>,
                                       ALLOW_DUPES>
    {
    private:
        dict_mdmk(const dict_mdmk &);
        void operator = (const dict_mdmk &);
    public:
        dict_mdmk(dictcount_t dict_count = DICTCOUNT_T_MAX)
        : dict_base<dnode_is_member<ITEM, DNODE_OFFSET>,
                    key_is_member<ITEM, KEY, KEY_OFFSET>,
                    compare_with_function<KEY, COMP>,
                    ALLOW_DUPES>(dict_count)
        {
        }
    };


}

#endif

#endif
