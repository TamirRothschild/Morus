#include <gtk/gtk.h>

void switch_tab(GtkWidget *widget, GtkWidget *notebook, guint page_num) {
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *home_tab;
    GtkWidget *lib_tab;
    GtkWidget *home_label;
    GtkWidget *lib_label;
    GtkWidget *button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tab Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Create Home Tab
    home_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    home_label = gtk_label_new("This is the Home Tab");
    gtk_box_pack_start(GTK_BOX(home_tab), home_label, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), home_tab, gtk_label_new("Home"));

    // Create Library Tab
    lib_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    lib_label = gtk_label_new("This is the Library Tab");
    gtk_box_pack_start(GTK_BOX(lib_tab), lib_label, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), lib_tab, gtk_label_new("Library"));

    // Button to switch to Library tab
    button = gtk_button_new_with_label("Go to Library Tab");
    g_signal_connect(button, "clicked", G_CALLBACK(switch_tab), notebook);
    gtk_box_pack_start(GTK_BOX(home_tab), button, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

