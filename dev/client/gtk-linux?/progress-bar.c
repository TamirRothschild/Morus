#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *file_dialog;
GtkWidget *humidity_progress_bar;
GtkWidget *noise_progress_bar;
GtkWidget *file_label;
GtkWidget *humidity_percentage_label;
GtkWidget *noise_percentage_label;
GtkWidget *fire_percentage_label;
GtkWidget *fire_progress_bar;

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
            } else if (strstr(token, "Noise") != NULL) {  // Assumes "Noise" is the key
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

void open_file_dialog(GtkWidget *widget, gpointer data) {
    if (gtk_dialog_run(GTK_DIALOG(file_dialog)) == GTK_RESPONSE_ACCEPT) {
        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
        gtk_label_set_text(GTK_LABEL(file_label), file_path);
        update_progress_bars(file_path);
        g_free(file_path);
    }
    gtk_widget_hide(file_dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *file_button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "CSV Progress Bars");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    file_button = gtk_button_new_with_label("Open CSV File");
    g_signal_connect(file_button, "clicked", G_CALLBACK(open_file_dialog), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), file_button, FALSE, FALSE, 0);

    file_label = gtk_label_new("No file selected");
    gtk_box_pack_start(GTK_BOX(vbox), file_label, FALSE, FALSE, 0);

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

    fire_percentage_label = gtk_label_new("Noise: 0.00%");
    gtk_box_pack_start(GTK_BOX(vbox), fire_percentage_label, FALSE, FALSE, 0);

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

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
