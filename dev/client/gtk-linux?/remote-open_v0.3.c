#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static gchar *file_path = NULL; // free it from my ass
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
GtkWidget *home_label;

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
                snprintf(text, sizeof(text), "%.2f%% ", value);
                gtk_label_set_text(GTK_LABEL(humidity_percentage_label), text);
            }
            else if (strstr(token, "Noise") != NULL) {  // Assumes "Noise" is the key
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(noise_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "%.2fdB ", value);
                gtk_label_set_text(GTK_LABEL(noise_percentage_label), text);
            }
            else if (strstr(token, "Fire") != NULL) {  // Assumes "Fire" is the key
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(fire_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "%.2f%% ", value);
                gtk_label_set_text(GTK_LABEL(fire_percentage_label), text);
            }
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
}

void open_default_csv() {
    const char *default_file_path = "default_data.csv"; // Adjust the path to your default CSV file
     printf("Opening file: %s\n", default_file_path); // for debug **** delete it from final ver

    if (file_path) {
        g_free(file_path); // Free old file path if it exists
    }
    file_path = g_strdup(default_file_path);
    update_progress_bars(file_path);
}

void on_advance_button_clicked(GtkWidget *widget, gpointer data) {
    gboolean visible = gtk_widget_get_visible(advance_box);
    gtk_widget_set_visible(advance_box, !visible);
    gtk_button_set_label(GTK_BUTTON(widget), visible ? "Advance?" : "Hide Advance");
    gtk_widget_set_visible(file_label, !visible && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(arg_file_input_test)));
}

void on_advance_check_button_toggled(GtkToggleButton *toggle_button, GtkWidget *button) {
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(button, active);
}

void open_file_dialog(GtkWidget *widget, gpointer data) {
    if (gtk_dialog_run(GTK_DIALOG(file_dialog)) == GTK_RESPONSE_ACCEPT) {
        if (file_path) {
            g_free(file_path); // Free old file path if it exists
        }
        file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_dialog));
        gtk_label_set_text(GTK_LABEL(file_label), file_path);
        update_progress_bars(file_path);
    }
    gtk_widget_hide(file_dialog);
}

gboolean update_file_periodically(gpointer user_data) { //just a fun to call 'update_progress_bars' //// its every time the data file is changed
    if (file_path) {
        update_progress_bars(file_path);
    }
    return TRUE; // Continue calling this function
}

GtkWidget* create_progress_bar_with_label(const gchar *label_text, GtkWidget **progress_label, GtkWidget **progress_bar) {
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create the label with a fixed width for alignment
    GtkWidget *label = gtk_label_new(label_text);
    gtk_widget_set_size_request(label, 75, -1); // Adjust the width as necessary
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    // Create the progress bar and set it to expand
    *progress_bar = gtk_progress_bar_new();
    gtk_widget_set_hexpand(*progress_bar, TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), *progress_bar, TRUE, TRUE, 0);

    // Create the percentage label and align it to the end
    *progress_label = gtk_label_new("null");
    gtk_widget_set_size_request(*progress_label, 50, -1); // Adjust the width as necessary
    gtk_widget_set_halign(*progress_label, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(hbox), *progress_label, FALSE, FALSE, 0);

    return hbox;
}



int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
//for tabs
    GtkWidget *notebook;
    GtkWidget *home_tab;
    GtkWidget *lib_tab;
    GtkWidget *lib_label;
    GtkWidget *sets_label;
    GtkWidget *sets_tab;
    GtkWidget *arg_dark_mode;
    GtkWidget *open_url_button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MorustestGUi-v0.3-linux");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // Create Home Tab
    home_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    home_label = gtk_label_new("Here You Can See Your Plant's Staus:");
    gtk_box_pack_start(GTK_BOX(home_tab), home_label, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), home_tab, gtk_label_new("Home"));

    // Create Library Tab
    lib_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    lib_label = gtk_label_new("just a test :)");
    gtk_box_pack_start(GTK_BOX(lib_tab), lib_label, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), lib_tab, gtk_label_new("Library"));

    // Create Settings Tab
    sets_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    sets_label = gtk_label_new("here you can change stuff");
    gtk_box_pack_start(GTK_BOX(sets_tab), sets_label, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sets_tab, gtk_label_new("Settings"));

    GtkWidget *humidity_box = create_progress_bar_with_label("Humidity", &humidity_percentage_label, &humidity_progress_bar);
    gtk_box_pack_start(GTK_BOX(home_tab), humidity_box, FALSE, FALSE, 0);

    GtkWidget *noise_box = create_progress_bar_with_label("Noise", &noise_percentage_label, &noise_progress_bar);
    gtk_box_pack_start(GTK_BOX(home_tab), noise_box, FALSE, FALSE, 0);

    GtkWidget *fire_box = create_progress_bar_with_label("Fire", &fire_percentage_label, &fire_progress_bar);
    gtk_box_pack_start(GTK_BOX(home_tab), fire_box, FALSE, FALSE, 0);

    // Add some padding at the bottom of home_tab
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(spacer, -1, 50);  // Add 20 pixels of vertical padding
    gtk_box_pack_start(GTK_BOX(home_tab), spacer, FALSE, FALSE, 0);


    // settings-tab
    arg_dark_mode = gtk_check_button_new_with_label("Let In The Dark? (a work in progress)");
    gtk_box_pack_start(GTK_BOX(sets_tab), arg_dark_mode, TRUE, TRUE, 0);

    open_url_button = gtk_button_new_with_label("Open The Source Code");
    gtk_box_pack_start(GTK_BOX(sets_tab), open_url_button, FALSE, TRUE, 0);


    // Connect the button click event to the callback function
    // g_signal_connect(open_url_button, "clicked", G_CALLBACK(on_open_url_clicked), "https://github.com/TamirRothschild/Morus");

    advance_button = gtk_button_new_with_label("Wanna Get Advance?");
    g_signal_connect(advance_button, "clicked", G_CALLBACK(on_advance_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(sets_tab), advance_button, FALSE, FALSE, 0);

    advance_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start(GTK_BOX(sets_tab), advance_box, FALSE, FALSE, 0);

    arg_file_input_test = gtk_check_button_new_with_label("Manual Plant Status");
    gtk_box_pack_start(GTK_BOX(advance_box), arg_file_input_test, FALSE, FALSE, 0);

    g_signal_connect(arg_file_input_test, "toggled", G_CALLBACK(on_advance_check_button_toggled), NULL);

    file_button = gtk_button_new_with_label("Choose a File");
    gtk_widget_set_sensitive(file_button, FALSE);
    g_signal_connect(file_button, "clicked", G_CALLBACK(open_file_dialog), NULL);
    gtk_box_pack_start(GTK_BOX(advance_box), file_button, FALSE, FALSE, 0);

    file_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(advance_box), file_label, FALSE, FALSE, 0);

    g_signal_connect(arg_file_input_test, "toggled", G_CALLBACK(on_advance_check_button_toggled), file_button);

    // File Dialog
    file_dialog = gtk_file_chooser_dialog_new("Choose a File", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN,
                                              "_Cancel", GTK_RESPONSE_CANCEL,
                                              "_Open", GTK_RESPONSE_ACCEPT,
                                              NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV Files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_dialog), filter);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_timeout_add_seconds(10, update_file_periodically, NULL);

    open_default_csv(); // opens the default file need to be at the buttom otherwise it would call a var when its not in memory

    gtk_widget_show_all(window);
    gtk_widget_set_visible(advance_box, FALSE);
    gtk_widget_set_visible(file_label, FALSE);

    gtk_main();
    return 0;
}
