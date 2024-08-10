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
GtkWidget *temp_progress_bar;
GtkWidget *water_progress_bar;

GtkWidget *humidity_percentage_label;
GtkWidget *temp_percentage_label;
GtkWidget *water_percentage_label;
GtkWidget *home_label;
//lib
GtkWidget *plant_list_box;

void update_progress_bar_color(GtkProgressBar *progress_bar, double value) {
    GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(progress_bar));
    GtkCssProvider *css_provider = gtk_css_provider_new();
    char css[256];
    GdkRGBA color;

    // Update color based on progress bar value
    if (value > 0.8) {
        gdk_rgba_parse(&color, "#ff0000"); // Red for low values
    } else if (value < 0.1) {
        gdk_rgba_parse(&color, "#ffa500"); // Orange for medium values
    } else {
        gdk_rgba_parse(&color, "#4caf50"); // Green for high values
    }

    snprintf(css, sizeof(css), "progressbar { background-color: %s; }", gdk_rgba_to_string(&color));

    gtk_css_provider_load_from_data(css_provider, css, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);
}


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
            if (strstr(token, "Humidity") != NULL) {
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(humidity_progress_bar), value / 100.0);
                update_progress_bar_color(GTK_PROGRESS_BAR(humidity_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "%.2f%% ", value);
                gtk_label_set_text(GTK_LABEL(humidity_percentage_label), text);
            }
            else if (strstr(token, "Temperature") != NULL) {
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(temp_progress_bar), value / 100.0);
                update_progress_bar_color(GTK_PROGRESS_BAR(temp_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "%.2fc* ", value);
                gtk_label_set_text(GTK_LABEL(temp_percentage_label), text);
            }
            else if (strstr(token, "Water") != NULL) {
                token = strtok(NULL, ",");
                double value = atof(token);
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(water_progress_bar), value / 100.0);
                update_progress_bar_color(GTK_PROGRESS_BAR(water_progress_bar), value / 100.0);
                char text[50];
                snprintf(text, sizeof(text), "%.2f%% ", value);
                gtk_label_set_text(GTK_LABEL(water_percentage_label), text);
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
    gtk_button_set_label(GTK_BUTTON(widget), visible ? "Show Advance Options" : "Hide Advance Options");
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

// for library
void load_plants_from_csv(const char *plant_file) {
    FILE *file = fopen(plant_file, "r");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", plant_file);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        char *name = strtok(buffer, ",");
        char *description = strtok(NULL, ",");
        char *temperature = strtok(NULL, ",");
        char *humidity = strtok(NULL, ",");
        char *water = strtok(NULL, "\n");

        // Create main row container
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

        // Add plant name
        GtkWidget *label = gtk_label_new(name);
        gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

        // Add expander for details
        GtkWidget *details_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

        // Add description
        GtkWidget *description_label = gtk_label_new(description);
        gtk_box_pack_start(GTK_BOX(details_box), description_label, TRUE, TRUE, 0);

        // Add temperature
        gchar temp_text[100];
        snprintf(temp_text, sizeof(temp_text), "Temperature: %s", temperature);
        GtkWidget *temp_label = gtk_label_new(temp_text);
        gtk_box_pack_start(GTK_BOX(details_box), temp_label, TRUE, TRUE, 0);

        // Add humidity
        gchar humidity_text[100];
        snprintf(humidity_text, sizeof(humidity_text), "Humidity: %s", humidity);
        GtkWidget *humidity_label = gtk_label_new(humidity_text);
        gtk_box_pack_start(GTK_BOX(details_box), humidity_label, TRUE, TRUE, 0);

        // Add water requirements
        gchar water_text[100];
        snprintf(water_text, sizeof(water_text), "Water: %s", water);
        GtkWidget *water_label = gtk_label_new(water_text);
        gtk_box_pack_start(GTK_BOX(details_box), water_label, TRUE, TRUE, 0);

        GtkWidget *expander = gtk_expander_new("Expand");
        gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);
        gtk_container_add(GTK_CONTAINER(expander), details_box);

        gtk_box_pack_start(GTK_BOX(vbox), expander, FALSE, FALSE, 0);
        gtk_list_box_insert(GTK_LIST_BOX(plant_list_box), vbox, -1);
    }

    fclose(file);
}



int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
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
    gtk_window_set_title(GTK_WINDOW(window), "MorustestGUi-v0.5-linux");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    home_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    home_label = gtk_label_new("Here You Can See Your Plant's Status:");
    gtk_box_pack_start(GTK_BOX(home_tab), home_label, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), home_tab, gtk_label_new("Home"));

    lib_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(lib_tab), scroll_window, TRUE, TRUE, 0);


    plant_list_box = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scroll_window), plant_list_box);

    load_plants_from_csv("plants_db.csv");

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), lib_tab, gtk_label_new("Library"));

    sets_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    sets_label = gtk_label_new("here you can change stuff");
    gtk_box_pack_start(GTK_BOX(sets_tab), sets_label, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sets_tab, gtk_label_new("Settings"));

    GtkWidget *humidity_box = create_progress_bar_with_label("Humidity", &humidity_percentage_label, &humidity_progress_bar);
    gtk_box_pack_start(GTK_BOX(home_tab), humidity_box, FALSE, FALSE, 0);

    GtkWidget *temp_box = create_progress_bar_with_label("Temp", &temp_percentage_label, &temp_progress_bar);
    gtk_box_pack_start(GTK_BOX(home_tab), temp_box, FALSE, FALSE, 0);

    GtkWidget *water_box = create_progress_bar_with_label("Water", &water_percentage_label, &water_progress_bar);
    gtk_box_pack_start(GTK_BOX(home_tab), water_box, FALSE, FALSE, 0);

    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(spacer, -1, 50);
    gtk_box_pack_start(GTK_BOX(home_tab), spacer, FALSE, FALSE, 0);

    arg_dark_mode = gtk_check_button_new_with_label("Let In The Dark? (a work in progress)");
    gtk_box_pack_start(GTK_BOX(sets_tab), arg_dark_mode, TRUE, TRUE, 0);

    open_url_button = gtk_button_new_with_label("Open The Source Code");
    gtk_box_pack_start(GTK_BOX(sets_tab), open_url_button, FALSE, TRUE, 0);

    advance_button = gtk_button_new_with_label("Show Advance Options");
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

    open_default_csv();


    gtk_widget_show_all(window);
    gtk_widget_set_visible(advance_box, FALSE);
    gtk_widget_set_visible(file_label, FALSE);

    gtk_main();
    return 0;
}