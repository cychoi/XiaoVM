/**
 * XiaoVM
 * 
 * Copyright (c) 2013 Mike Manilone
 * All rights reserved
 */
#include <glib/gi18n.h>
#include "processor.h"
#include "functions.h"

int 
XVM_builtin_io__print (
    struct XVM_Processor *processor, 
    int argc, 
    struct XVM_Object *argv[])
{
    for (int i = 0; i < argc && argv[i]->user_flags != XVM_SpecialFlag; i++)
    {
        switch (argv[i]->type)
        {
        case XVM_TYPE_STRING:
            g_print ("%s", argv[i]->data.v_string);
            break;
        case XVM_TYPE_NUMBER:
            g_print ("%d", argv[i]->data.v_number);
            break;
        case XVM_TYPE_OTHER:
            g_print ("Object<0x%lX>", GPOINTER_TO_SIZE(argv[i]->data.v_pointer));
            break;
        default:
            g_assert_not_reached ();
            break;
        }
    }
    g_print ("\n");
    return XVM_FUNCALL_OK;
}

void 
xvm_builtins_load_io (struct XVM_Processor *processor)
{
    xvm_processor_provide (processor, "io::print", XVM_builtin_io__print);
}

