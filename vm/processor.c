/**
 * XiaoVM
 * 
 * Copyright (c) 2013 Mike Manilone
 * All rights reserved
 */
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include "object.h"
#include "functions.h"
#include "processor.h"

static void 
_xvm_processor_reinitialize_stack (struct XVM_Processor *processor)
{
    if (processor->curstack)
        g_ptr_array_free (processor->curstack, TRUE);
    processor->curstack = g_ptr_array_new_with_free_func ((GDestroyNotify)xvm_object_unref);
}

struct XVM_Processor *
xvm_processor_new (void *data, size_t len)
{
    struct XVM_Processor *processor = g_malloc0 (sizeof (struct XVM_Processor));
    processor->script = g_string_new_len ((char *)data, len);
    processor->returnval = xvm_object_new (XVM_TYPE_OTHER);
    processor->functable = g_hash_table_new (g_str_hash, g_str_equal);
    _xvm_processor_reinitialize_stack (processor);
    return processor;
}

void 
xvm_processor_destroy (struct XVM_Processor *processor)
{
    g_hash_table_destroy (processor->functable);
    g_ptr_array_free (processor->curstack, TRUE);
    g_string_free (processor->script, TRUE);
    xvm_object_destroy (processor->returnval);
}

struct XVM_Object *
xvm_processor_next_register (struct XVM_Processor *processor)
{
    struct XVM_Object *object = xvm_object_new (XVM_TYPE_OTHER);
    g_ptr_array_add (processor->curstack, object); /* don't hold the refcount */
    return object;
}

gboolean 
xvm_processor_run (struct XVM_Processor *processor, /* nofree */ gchar **outErr)
{
    gchar *buf = processor->script->str;
    for (size_t i = 0; i < processor->script->len; i++)
    {
        switch (buf[i])
        {
        case XVM_PROCESSOR_CMD_CALL:
        {
            gboolean failed = FALSE;
            ++i; /* skip instruction itself */
            gchar *funcname = buf+i;
            g_debug ("call %s", buf+i);
            /* Call with current stack */
            XVM_Function func = (XVM_Function)g_hash_table_lookup (processor->functable, funcname);
            if (!func)
            {
                *outErr = _("No such function found");
                failed = TRUE;
            }
            else 
            {
                if (func (processor, processor->curstack->len, (struct XVM_Object **)processor->curstack->pdata) != XVM_FUNCALL_OK)
                {
                    *outErr = processor->returnval->data.v_string;
                    failed = TRUE;
                }
            }
            /* Decrease refcount and Destroy the stack */
            _xvm_processor_reinitialize_stack (processor);
            i += strlen (funcname); /* skip the string */
            if (failed == TRUE)
                return FALSE;
            break;
        }
        case XVM_PROCESSOR_CMD_LOAD_STRING:
        {
            ++i; /* skip instruction itself */
            guchar len = buf[i];
            gchar *str = g_strndup (buf+i+1, len);
            g_debug ("load string (len %d) %s", len, str);
            struct XVM_Object *obj = xvm_object_new (XVM_TYPE_STRING); /* take the ownership of @str */
            xvm_object_update_data (obj, &str);
            g_ptr_array_add (processor->curstack, obj); /* the refcount will be decreased when the stack is destroyed */
            i += len;
            break;
        }
        case XVM_PROCESSOR_CMD_LOAD_NUMBER:
        {
            ++i; /* skip instruction itself */
            unsigned char n = buf[i];
            g_debug ("load number %d", n);
            struct XVM_Object *obj = xvm_object_new (XVM_TYPE_NUMBER);
            xvm_object_update_data (obj, &n);
            g_print ("%d\n", obj->data.v_number);
            g_ptr_array_add (processor->curstack, obj);
            break;
        }
        case XVM_PROCESSOR_CMD_LOAD_SPECIAL:
        {
            ++i; /* skip instruction itself */
            gint special = buf[i];
            g_debug ("load special 0x%X", special);
            struct XVM_Object *obj = xvm_object_new (XVM_TYPE_NUMBER);
            xvm_object_update_data (obj, &special);
            obj->user_flags = XVM_SpecialFlag;
            g_ptr_array_add (processor->curstack, obj);
            break;
        }
        default:
            g_assert_not_reached ();
            break;
        }
    }
    return TRUE;
}

void 
xvm_processor_provide (struct XVM_Processor *processor, const gchar *name, XVM_Function func)
{
    g_hash_table_insert (processor->functable, (gpointer)name, func);
}

void 
xvm_processor_set_result_full (
    struct XVM_Processor *processor, 
    enum XVM_ObjectType type, 
    gpointer data)
{
    xvm_object_update_type (processor->returnval, type);
    xvm_object_update_data (processor->returnval, data);
}

void 
xvm_processor_set_result_string (struct XVM_Processor *processor, const gchar *str)
{
    gchar *newstr = g_strdup (str);
    xvm_processor_set_result_full (processor, XVM_TYPE_STRING, &newstr);
}

void 
xvm_processor_set_result_number (struct XVM_Processor *processor, gint number)
{
    xvm_processor_set_result_full (processor, XVM_TYPE_NUMBER, &number);
}

void 
xvm_processor_set_error (struct XVM_Processor *processor, const gchar *errorstr)
{
    xvm_processor_set_result_string (processor, errorstr);
}

