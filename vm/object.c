/**
 * XiaoVM
 * 
 * Copyright (c) 2013 Mike Manilone
 * All rights reserved
 */
#include "object.h"

#define LOCK   g_mutex_lock (&obj->lock)
#define UNLOCK g_mutex_unlock (&obj->lock)

static void 
_xvm_object_initialize (struct XVM_Object *obj, 
                        enum XVM_ObjectType type)
{
    obj->refcount = 1;
    obj->type = type;
    g_mutex_init (&obj->lock);
}

/* free "internal" data */
static void 
_xvm_object_finalize_internal (struct XVM_Object *obj)
{
    g_mutex_clear (&obj->lock);
}

/* free "real" data */
static void 
_xvm_object_default_finalizer (struct XVM_Object *obj)
{
    switch (obj->type)
    {
    case XVM_TYPE_STRING:
        g_free (obj->data.v_string);
        break;
    case XVM_TYPE_NUMBER:
        break;
    case XVM_TYPE_OTHER:
        /* should consider using customized finalizer... */
        break;
    default:
        g_assert_not_reached ();
        break;
    }
}

static void 
_xvm_object_default_copier (struct XVM_Object *obj, struct XVM_Object *dest)
{
    switch (obj->type)
    {
    case XVM_TYPE_STRING:
        dest->data.v_string = g_strdup (obj->data.v_string);
        break;
    case XVM_TYPE_NUMBER:
        dest->data.v_number = obj->data.v_number;
        break;
    case XVM_TYPE_OTHER:
        /* should consider using customized copier */
        dest->data.v_pointer = obj->data.v_pointer;
        break;
    default:
        g_assert_not_reached ();
        break;
    }
}

struct XVM_Object *
xvm_object_new (enum XVM_ObjectType type)
{
    struct XVM_Object *obj = (struct XVM_Object *)g_malloc0 (sizeof (struct XVM_Object));
    _xvm_object_initialize (obj, type);
    return obj;
}

void 
xvm_object_set_finalizer (struct XVM_Object *obj, XVM_ObjFinalizer finalizer)
{
    xvm_object_ref (obj);
    LOCK;
    obj->finalizer = finalizer;
    UNLOCK;
    xvm_object_unref (obj);
}

void 
xvm_object_set_data_updater (struct XVM_Object *obj, XVM_ObjDataUpdater updater)
{
    xvm_object_ref (obj);
    LOCK;
    obj->dataupdater = updater;
    UNLOCK;
    xvm_object_unref (obj);
}

void 
xvm_object_set_copier (struct XVM_Object *obj, XVM_ObjCopier copier)
{
    xvm_object_ref (obj);
    LOCK;
    obj->copier = copier;
    UNLOCK;
    xvm_object_unref (obj);
}

struct XVM_Object *
xvm_object_update_data (struct XVM_Object *obj, 
                        gpointer _data)
{
    gpointer *data = (gpointer *)_data;
    xvm_object_ref (obj);
    LOCK;
    if (obj->dataupdater)
    {
        obj->dataupdater (obj, data);
    }
    else 
    {
        /* the default updater */
        switch (obj->type)
        {
        case XVM_TYPE_STRING:
            if (obj->data.v_string)
                g_free (obj->data.v_string);
            obj->data.v_string = *(char **)data;
            break;
        case XVM_TYPE_NUMBER:
            obj->data.v_number = *(gint *)data;
            break;
        case XVM_TYPE_OTHER:
            obj->data.v_pointer = *data;
            break;
        default:
            g_assert_not_reached ();
            break;
        }
    }
    UNLOCK;
    xvm_object_unref (obj);
    return obj;
}

struct XVM_Object *
xvm_object_update_type (struct XVM_Object *obj, enum XVM_ObjectType type)
{
    obj->type = type;
    return obj;
}

struct XVM_Object *
xvm_object_ref (struct XVM_Object *obj)
{
    LOCK;
    obj->refcount ++;
    UNLOCK;
    return obj;
}


void 
xvm_object_unref (struct XVM_Object *obj)
{
    LOCK;
    obj->refcount --;
    UNLOCK;
    if (obj->refcount <= 0)
    {
        xvm_object_destroy (obj);
    }
}

void 
xvm_object_destroy (struct XVM_Object *obj)
{
    LOCK;
    if (obj->finalizer)
        obj->finalizer (obj);
    else 
        _xvm_object_default_finalizer (obj);
    UNLOCK;
    _xvm_object_finalize_internal (obj);
}

struct XVM_Object *
xvm_object_copy (struct XVM_Object *obj)
{
    struct XVM_Object *dest = xvm_object_new (obj->type);
    dest->finalizer = obj->finalizer;
    dest->dataupdater = obj->dataupdater;
    dest->copier = obj->copier;
    if (obj->copier)
        obj->copier (obj, dest);
    else 
        _xvm_object_default_copier (obj, dest);
    return dest;
}

