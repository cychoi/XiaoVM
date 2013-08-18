/**
 * XiaoVM
 * 
 * Copyright (c) 2013 Mike Manilone
 * All rights reserved
 */
#pragma once

#include <glib.h>
#include "object.h"

struct XVM_Processor
{
    GString           *script;
    GHashTable        *functable;
    GPtrArray         *curstack;
    struct XVM_Object *returnval;
};

enum XVM_ProcessorCmd
{
    XVM_PROCESSOR_CMD_CALL = 0x5, 
    XVM_PROCESSOR_CMD_LOAD_STRING = 0x10,
    XVM_PROCESSOR_CMD_LOAD_SPECIAL, 
    XVM_PROCESSOR_CMD_LOAD_NUMBER, 
    XVM_PROCESSOR_CMD_LOAD_ADDRESS, /* not implemented */
    XVM_PROCESSOR_CMD_LOAD_OBJECT, /* not implemented */
};

enum XVM_Special
{
    XVM_SPECIAL_STOP /* end of argv */
};

#define XVM_SpecialFlag 0x1

typedef int (*XVM_Function) (struct XVM_Processor *, int, struct XVM_Object **);

G_BEGIN_DECLS
#define T struct XVM_Processor

T        *xvm_processor_new       (void *data, 
                                   size_t length);
gboolean  xvm_processor_run       (T *processor, 
                                   char **outErr);
void      xvm_processor_destroy   (T *processor);
void      xvm_processor_provide   (T *processor, 
                                   const gchar *name, 
                                   XVM_Function func);
void      xvm_processor_set_error (T *processor, 
                                   const gchar *errorstr);

#undef T
G_END_DECLS
