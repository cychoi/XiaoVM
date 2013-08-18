/**
 * XiaoVM
 * 
 * Copyright (c) 2013 Mike Manilone
 * All rights reserved
 */
#pragma once

#include <glib.h>
#include "processor.h"
#include "object.h"

enum XVM_FunCallResult
{
    XVM_FUNCALL_ERROR, 
    XVM_FUNCALL_OK
};

int  XVM_builtin_io__print   (struct XVM_Processor *processor, 
                              int argc, 
                              struct XVM_Object *argv[]);
void xvm_builtins_load_io    (struct XVM_Processor *processor);
