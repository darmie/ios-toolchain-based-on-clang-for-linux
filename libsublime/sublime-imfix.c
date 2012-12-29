#include <gtk/gtkimcontext.h>
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
}

