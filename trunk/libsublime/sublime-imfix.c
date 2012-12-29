#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdlib.h>
#include <string.h>
static GdkFilterReturn event_filter (GdkXEvent *xevent, GdkEvent *event, gpointer data)
{

    GdkWindow *window = g_object_get_data(G_OBJECT(data),"window");
    int w_width, w_height;
    gdk_drawable_get_size(window, &w_width, &w_height);

    gchar *conf = g_build_filename(g_get_home_dir(), "/.sublime_im_position", NULL);

    XEvent *xev = (XEvent *)xevent;
    if(xev->type == KeyRelease) {
        GKeyFile *keyfile = g_key_file_new();
        GError *error = NULL;
        g_key_file_load_from_file(keyfile, conf, G_KEY_FILE_NONE, &error);

        int id, x, y;
            
        x= g_key_file_get_double(keyfile,"pos","x", &error);
        error = NULL;
        y=g_key_file_get_double(keyfile,"pos","y", &error);
        error = NULL;
        id=g_key_file_get_integer(keyfile,"pos","id", &error);
       
        GdkRectangle rect;
        if(id == 3) { //editor window
            rect.x = x+60;
            rect.y = y;
            rect.width = 0;
            rect.height = 70;
        } 
        else if(id == 2) //bottom
        {
            rect.x = x+30;
            rect.y = ABS(w_height-100);
            rect.width = 0;
            rect.height = 70;
        }else if(id == 1) { //top center
            rect.x = x + (w_width-428)/2;
            rect.y = y + 10;
            rect.width = 0;
            rect.height = 70;
        }
        
        gtk_im_context_set_cursor_location(data, &rect);
    }
    return GDK_FILTER_CONTINUE;
}

void gtk_im_context_set_client_window (GtkIMContext *context,
          GdkWindow    *window)
{
  GtkIMContextClass *klass;
  g_return_if_fail (GTK_IS_IM_CONTEXT (context));
  klass = GTK_IM_CONTEXT_GET_CLASS (context);
  if (klass->set_client_window)
    klass->set_client_window (context, window);

  g_object_set_data(G_OBJECT(context),"window",window);

  if(!GDK_IS_WINDOW (window))
    return;
  int width = gdk_window_get_width(window);
  int height = gdk_window_get_height(window);
  if(width != 0 && height !=0)
    gtk_im_context_focus_in(context);
  gdk_window_add_filter (window, event_filter, context);
}

