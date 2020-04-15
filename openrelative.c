/*
 * plugin.c
 *
 * Copyright 2020 Makin <makin.public@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <geanyplugin.h>

GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
static GtkWidget *main_menu_item = NULL;
static const char * PLUGIN_NAME= "Open Relative";
static const int    WINDOW_SIZE= 500;

#define DS "/"

int launch_widget(const int window_size);

PLUGIN_VERSION_CHECK(1)
PLUGIN_SET_INFO(PLUGIN_NAME, "Open Relative (Key Bindings) ","1.0", "Muhammad Izzulmakin <makin.public@gmail.com>");

enum
{
    KEYB_OPEN_RELATIVE,
    KEYB_COUNT
};
enum
{
    COL_NAME = 0,
    COL_IDX,
    NUM_COLS
} ;
struct open_relative_plugin
{
    GtkTreeModel        *file_list;
    GtkTreeModel        *file_list_filter ;
    GtkTreeSelection    *tree_selection;
    GtkWidget           *window;
    GtkWidget           *view;
    GtkWidget           *box;
    GtkWidget           *text_entry;

} open_relative_plugin;

static void item_activate_cb(GtkMenuItem *menuitem, gpointer gdata)
{
    launch_widget(WINDOW_SIZE);
}
static void kb_activate(G_GNUC_UNUSED guint key_id)
{
    launch_widget(WINDOW_SIZE);
}
static void openrelative_p_launch(G_GNUC_UNUSED guint key_id)
{

    launch_widget(WINDOW_SIZE);
}
void plugin_init(GeanyData *data)
{
    main_menu_item = gtk_menu_item_new_with_mnemonic(PLUGIN_NAME);
    gtk_widget_show(main_menu_item);
    gtk_container_add(GTK_CONTAINER(geany->main_widgets->tools_menu), main_menu_item);
    GeanyKeyGroup *key_group = plugin_set_key_group(geany_plugin,"open_relative_plugin", KEYB_COUNT, NULL);
    keybindings_set_item(key_group, KEYB_OPEN_RELATIVE, kb_activate, 0,0,"open_relative_plugin", PLUGIN_NAME, NULL);
    g_signal_connect(main_menu_item, "activate",G_CALLBACK(item_activate_cb), geany);
}
void plugin_cleanup(void)
{
    gtk_widget_destroy(main_menu_item);
}


static gboolean callback_button_press (GtkWidget *widget, GdkEventButton *event, gpointer   data)
{
    struct open_relative_plugin * openrelative_p = data;
    GtkTreeModel     *model;
    GtkTreeIter       iter;
    if (event->button == 1) {
        if (gtk_tree_selection_get_selected(openrelative_p->tree_selection, &model, &iter))
        {

            gchar *  name = NULL;
            gtk_tree_model_get (model, &iter, COL_NAME, &name, -1);
            GeanyDocument * current_doc = document_find_by_real_path(name);
            document_open_file(current_doc->file_name, FALSE, NULL, NULL);
        }
        gtk_widget_destroy(openrelative_p->window);
        g_free(openrelative_p);
    }

    return FALSE;
}

static gboolean callback_key_press (GtkWidget *widget, GdkEventKey  *event, gpointer   data)
{
    struct open_relative_plugin * openrelative_p = data;
    GtkTreeModel     *model;
    GtkTreeIter       iter;
    switch(event->keyval)
    {
    case 65307://escape
        gtk_widget_destroy(openrelative_p->window);
        g_free(openrelative_p);
        break;
    case 65293://enter

        //~ if (gtk_tree_selection_get_selected(openrelative_p->tree_selection, &model, &iter))
        //~ {

            //~ gchar *  name = NULL;
            //~ gtk_tree_model_get (model, &iter, COL_NAME, &name, -1);
            //~ GeanyDocument * current_doc = document_find_by_real_path(name);
            //~ document_open_file(current_doc->file_name, FALSE, NULL, NULL);
        //~ }
        if (1) {
            const gchar * name = gtk_entry_get_text(GTK_ENTRY(openrelative_p->text_entry));
            
            const gchar * basepath = geany_data->app->project->base_path;
            const gchar * fullname = g_strconcat(basepath, DS, name, NULL);
            printf("open relative: opening %s\n",fullname);
            document_open_file(fullname, FALSE, NULL, NULL);
            
            gtk_widget_destroy(openrelative_p->window);
            g_free(fullname);
            g_free(openrelative_p);
        }
        break;
    default:
        return FALSE;

    }

    return FALSE;
}

static gboolean callback_display_element (GtkTreeModel *model,GtkTreeIter  *iter, gpointer data)
{
    gchar *str;
    struct open_relative_plugin * openrelative_p = data;
    gboolean visible = FALSE;
    gtk_tree_model_get (model, iter, COL_NAME, &str, -1);
    const gchar * text_value = gtk_entry_get_text(GTK_ENTRY(openrelative_p->text_entry));

    if (!text_value || g_strcmp0(text_value,"")==0 || (str && g_str_match_string (text_value, str, TRUE)))
    {
        visible = TRUE;
    }
    g_free (str);

    return visible;
}


static int callback_update_visibilty_elements(GtkWidget *widget, gpointer   data )
{
    struct open_relative_plugin * openrelative_p = data;
    gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(openrelative_p->file_list_filter));
    return 0;
}


static void create_and_fill_model (struct open_relative_plugin *openrelative_p)
{
    GtkListStore  *store;
    GtkTreeIter    iter;
    store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

    int i;
    for ( i= 0; i < geany->documents_array->len; i++)
    {
        GeanyDocument *current_doc = g_ptr_array_index (geany->documents_array, i);

       if(current_doc->file_name){
                gtk_list_store_append (store, &iter);
                gtk_list_store_set (store, &iter, COL_NAME, current_doc->file_name, -1);
        }

    }

    openrelative_p->file_list = GTK_TREE_MODEL (store);
}
static void  create_view_and_model (struct open_relative_plugin *openrelative_p)
{
    GtkCellRenderer     *renderer;
    openrelative_p->view = gtk_tree_view_new ();
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (openrelative_p->view), -1, "file_name", renderer, "text", COL_NAME, NULL);
    create_and_fill_model (openrelative_p);
    openrelative_p->file_list_filter = gtk_tree_model_filter_new (openrelative_p->file_list,NULL);
    gtk_tree_view_set_model (GTK_TREE_VIEW (openrelative_p->view), openrelative_p->file_list_filter);
    gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(openrelative_p->file_list_filter),callback_display_element,openrelative_p,NULL);
    g_object_unref (openrelative_p->file_list_filter);

}

static gboolean quit_openrelative_p (GtkWidget *widget,GdkEvent  *event,gpointer   data)
{
    return FALSE;
}

int launch_widget(const int window_size)
{

    struct open_relative_plugin * openrelative_p =  g_malloc(sizeof(open_relative_plugin));
    openrelative_p->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    openrelative_p->box = gtk_vbox_new(0,0);
    openrelative_p->text_entry = gtk_entry_new();
    create_view_and_model (openrelative_p);
    g_signal_connect (openrelative_p->window, "delete_event", G_CALLBACK(quit_openrelative_p), openrelative_p);
    g_signal_connect (openrelative_p->window, "key-press-event", G_CALLBACK(callback_key_press), openrelative_p);
    //~ g_signal_connect (openrelative_p->window, "button-press-event", G_CALLBACK(callback_button_press), openrelative_p);
    openrelative_p->tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(openrelative_p->view));
    //~ g_signal_connect (openrelative_p->text_entry, "changed", G_CALLBACK(callback_update_visibilty_elements), openrelative_p);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(openrelative_p->view),FALSE);
    GtkWidget * fixed = gtk_fixed_new ();
    GtkWidget * scrolled_win = gtk_scrolled_window_new(NULL,NULL);
    gtk_fixed_put(GTK_FIXED(fixed),openrelative_p->text_entry,0,0);
    gtk_window_set_resizable  (GTK_WINDOW(openrelative_p->window),FALSE);
    gtk_widget_set_size_request (scrolled_win,window_size,window_size);
    gtk_widget_set_size_request (openrelative_p->text_entry,window_size,25);
    gtk_widget_set_size_request (openrelative_p->window,window_size,window_size);
    gtk_container_add(GTK_CONTAINER(scrolled_win), openrelative_p->view);
    gtk_fixed_put(GTK_FIXED(fixed),scrolled_win,0,25);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled_win), GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(openrelative_p->window), fixed);
    gtk_widget_show_all (openrelative_p->window);

    return 0;
}


