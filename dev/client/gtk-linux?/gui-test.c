#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *file_dialog;
GtkWidget *file_entry;
GtkWidget *arg_file_input_test;
GtkWidget *advance_box;
GtkWidget *advance_button;
GtkWidget *file_button;
GtkWidget *file_label;

void on_advance_button_clicked(GtkWidget *widget, gpointer data) {
    gboolean visible = gtk_widget_get_visible(advance_box);
    gtk_widget_set_visible(advance_box, !visible);
    gtk_button_set_label(GTK_BUTTON(widget), visible ? "Advanced?" : "Hide Advanced");
    //gtk_widget_set_visible(file_label, !visible && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(arg_file_input_test)));
}

void on_advance_check_button_toggled(GtkToggleButton *toggle_button, GtkWidget *button) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(button, active);
}

void open_file_dialog(GtkWidget *widget, gpointer data) {
    if (gtk_dialog_run(GTK_DIALOG(file_dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
        gtk_label_set_text(GTK_LABEL(file_label), file_path);
        g_free(file_path);
    }
    gtk_widget_hide(file_dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Morus_pre_testGUi");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    advance_button = gtk_button_new_with_label("Advanced?");
    g_signal_connect(advance_button, "clicked", G_CALLBACK(on_advance_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), advance_button, TRUE, TRUE, 0);

    advance_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start(GTK_BOX(vbox), advance_box, TRUE, TRUE, 0);

    GtkWidget *hbox_file_input = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    arg_file_input_test = gtk_check_button_new_with_label("Manual Plant Status");
    file_button = gtk_button_new_with_label("Chose a Data file (.csv)");
    g_signal_connect(file_button, "clicked", G_CALLBACK(open_file_dialog), NULL);
    gtk_widget_set_sensitive(file_button, FALSE);
    g_signal_connect(arg_file_input_test, "toggled", G_CALLBACK(on_advance_check_button_toggled), file_button);
    gtk_box_pack_start(GTK_BOX(hbox_file_input), arg_file_input_test, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_file_input), file_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(advance_box), hbox_file_input, TRUE, TRUE, 0);

    file_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), file_label, TRUE, TRUE, 0);
    gtk_widget_set_visible(file_label, FALSE);

    file_dialog = gtk_file_chooser_dialog_new("Data file (.csv)",
                                              GTK_WINDOW(window),
                                              GTK_FILE_CHOOSER_ACTION_OPEN,
                                              "_Cancel", GTK_RESPONSE_CANCEL,
                                              "_Open", GTK_RESPONSE_ACCEPT,
                                              NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, ".csv file");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog), filter);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_widget_set_visible(advance_box, FALSE); // Ensure advance_box is hidden initially
    gtk_main();

    return 0;
}
