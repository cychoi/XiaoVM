/**
 * XiaoVM
 * 
 * Copyright (c) 2013 Mike Manilone
 * All rights reserved
 */
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include "processor.h"
#include "functions.h"

gchar *
_load_contents (const gchar *filename, gsize *length)
{
    gchar *contents = NULL;
    GError *error = NULL;
    GFile *file = g_file_new_for_path (filename);
    g_file_load_contents (file, NULL, &contents, length, NULL, &error);
    g_object_unref (file);
    if (error)
    {
        g_error ("%s", error->message);
        g_error_free (error);
    }
    return contents;
}

gint 
main (gint   argc, 
      gchar *argv[])
{
    setlocale (LC_ALL, "");
    for (int i = 1; argv[i] != NULL; i ++)
    {
        gchar *errmsg = NULL;
        gsize filelength;
        gchar *contents = _load_contents (argv[i], &filelength);
        struct XVM_Processor *p = xvm_processor_new (contents, filelength);
        xvm_builtins_load_io (p);
        xvm_processor_run (p, &errmsg);
        xvm_processor_destroy (p);
        g_free (contents);
        if (errmsg)
        {
            g_error ("%s", errmsg);
            g_free (errmsg);
        }
    }
    return 0;
}


