/**
 * XiaoVM
 * 
 * Copyright (c) 2013 Mike Manilone
 * All rights reserved
 */
#pragma once

#include <glib.h>

enum XVM_ObjectType
{
    XVM_TYPE_STRING, 
    XVM_TYPE_NUMBER, 
    XVM_TYPE_OTHER
};

struct XVM_Object; /* dummy */

typedef void (*XVM_ObjFinalizer) (struct XVM_Object *obj);
typedef void (*XVM_ObjDataUpdater) (struct XVM_Object *obj, 
                                    gpointer *data);
typedef void (*XVM_ObjCopier) (struct XVM_Object *src, 
                               struct XVM_Object *dest);

struct XVM_Object
{
    int                 refcount;
    enum XVM_ObjectType type;
    GMutex              lock;
    XVM_ObjFinalizer    finalizer;
    XVM_ObjDataUpdater  dataupdater;
    XVM_ObjCopier       copier;
    
    union {
        gchar   *v_string;
        gint     v_number;
        gpointer v_pointer;
    } data;
    
    int user_flags; /* up to user */
};

G_BEGIN_DECLS
#define T struct XVM_Object

T    *xvm_object_new     (enum XVM_ObjectType type);
void  xvm_object_set_finalizer 
                         (T *obj, XVM_ObjFinalizer finalizer);
void  xvm_object_set_data_updater 
                         (T *obj, XVM_ObjDataUpdater updater);
void  xvm_object_set_copier 
                         (T *obj, XVM_ObjCopier copier);
T    *xvm_object_update_data 
                         (T *obj, gpointer data);
T    *xvm_object_update_type 
                         (T *obj, enum XVM_ObjectType type);
T    *xvm_object_ref     (T *obj);
void  xvm_object_unref   (T *obj);
void  xvm_object_destroy (T *obj);

#undef T
G_END_DECLS
