#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for the gtk window and file management
GtkWidget *file_dialog;
GtkWidget *file_entry;
GtkWidget *arg_file_input_test;
GtkWidget *advance_box;
GtkWidget *advance_button;
GtkWidget *file_button;
GtkWidget *file_label;

// for the progress bars
GtkWidget *humidity_progress_bar;
GtkWidget *noise_progress_bar;
GtkWidget *fire_progress_bar;
GtkWidget *humidity_percentage_label;
GtkWidget *noise_percentage_label;
GtkWidget *fire_percentage_label;

void update_progress_bars(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", file_path);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        char *token = strtok(buffer, ",");
        while (token != NULL) {
            if (strstr(token, "Humidity") != NULL) {  // Assumes "Humidity" is the key
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(humidity_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "Humidity level is: %.2f%%", value);
                gtk_label_set_text(GTK_LABEL(humidity_percentage_label), text);
            }
            else if (strstr(token, "Noise") != NULL) {  // Assumes "Noise" is the key
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(noise_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "Noise level is: %.2f%%", value);
                gtk_label_set_text(GTK_LABEL(noise_percentage_label), text);
            }
            else if (strstr(token, "Fire") != NULL) {  // Assumes "Fire" is the key
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(fire_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "Fire level is: %.2f%%", value);
                gtk_label_set_text(GTK_LABEL(fire_percentage_label), text);
            }
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
}

void on_advance_button_clicked(GtkWidget *widget, gpointer data) {
    gboolean visible = gtk_widget_get_visible(advance_box);
    gtk_widget_set_visible(advance_box, !visible);
    gtk_button_set_label(GTK_BUTTON(widget), visible ? "Advanced?" : "Hide Advanced");
    gtk_widget_set_visible(file_label, !visible && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(arg_file_input_test)));
}

void on_advance_check_button_toggled(GtkToggleButton *toggle_button, GtkWidget *button) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(button, active);
}

void open_file_dialog(GtkWidget *widget, gpointer data) {
    if (gtk_dialog_run(GTK_DIALOG(file_dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
        gtk_label_set_text(GTK_LABEL(file_label), file_path);
        update_progress_bars(file_path);
        g_free(file_path);
    }
    gtk_widget_hide(file_dialog);
}

void resize_advance_button(GtkWidget *widget, gpointer data) {
    GtkWidget *button = GTK_WIDGET(data);
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    int new_height = allocation.height * 0.25; // 25% of parent's height
    gtk_widget_set_size_request(button, -1, new_height); // Set height and let width adjust automatically
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MorustestGUi-v0.1-linux");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 10);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);



    humidity_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), humidity_progress_bar, FALSE, FALSE, 0);

    humidity_percentage_label = gtk_label_new("Humidity: 0.00%");
    gtk_box_pack_start(GTK_BOX(vbox), humidity_percentage_label, FALSE, FALSE, 0);

    noise_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), noise_progress_bar, FALSE, FALSE, 0);

    noise_percentage_label = gtk_label_new("Noise: 0.00%");
    gtk_box_pack_start(GTK_BOX(vbox), noise_percentage_label, FALSE, FALSE, 0);

    fire_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), fire_progress_bar, FALSE, FALSE, 0);

    fire_percentage_label = gtk_label_new("Fire: 0.00%");
    gtk_box_pack_start(GTK_BOX(vbox), fire_percentage_label, FALSE, FALSE, 0);


    advance_button = gtk_button_new_with_label("Advanced?");
    g_signal_connect(advance_button, "clicked", G_CALLBACK(on_advance_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), advance_button, TRUE, TRUE, 0);
    gtk_widget_set_size_request(advance_button, 30, 30); // Adjust size as needed

    advance_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start(GTK_BOX(vbox), advance_box, TRUE, TRUE, 0);

    GtkWidget *hbox_file_input = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    arg_file_input_test = gtk_check_button_new_with_label("Manual Plant Status");
    file_button = gtk_button_new_with_label("Choose a Data file (.csv)");
    g_signal_connect(file_button, "clicked", G_CALLBACK(open_file_dialog), NULL);
    gtk_widget_set_sensitive(file_button, FALSE);
    g_signal_connect(arg_file_input_test, "toggled", G_CALLBACK(on_advance_check_button_toggled), file_button);
    gtk_box_pack_start(GTK_BOX(hbox_file_input), arg_file_input_test, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_file_input), file_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(advance_box), hbox_file_input, TRUE, TRUE, 0);

    file_label = gtk_label_new(NULL);
    gtk_box_pack_start(GTK_BOX(vbox), file_label, FALSE, FALSE, 0);
    gtk_widget_set_visible(file_label, FALSE);

    file_dialog = gtk_file_chooser_dialog_new("Select CSV File",
                                              GTK_WINDOW(window),
                                              GTK_FILE_CHOOSER_ACTION_OPEN,
                                              "_Cancel", GTK_RESPONSE_CANCEL,
                                              "_Open", GTK_RESPONSE_ACCEPT,
                                              NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog), filter);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

   // g_signal_connect(window, "size-allocate", G_CALLBACK(resize_advance_button), advance_button);

    gtk_widget_show_all(window);
    gtk_widget_show_all(window);
    gtk_widget_set_visible(advance_box, FALSE); // Ensure advance_box is hidden initially
    gtk_main();

    return 0;
}
