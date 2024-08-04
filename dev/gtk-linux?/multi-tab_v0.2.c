#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static gchar *file_path = NULL; // free it to my ass
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
//GtkWidget *humidity_percentage_label;
//GtkWidget *noise_percentage_label;
//GtkWidget *fire_percentage_label;
//setting-tab
GtkWidget *arg_dark_mode;
GtkWidget *dark_button;

static GtkWidget *humidity_percentage_label;
static GtkWidget *noise_percentage_label;
static GtkWidget *fire_percentage_label;
static GtkWidget *home_label;
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
                snprintf(text, sizeof(text), "Noise level is: %.2fdB", value);
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


//void on_open_url_clicked(GtkWidget *widget, gpointer data) {
 //   GtkWindow *window = GTK_WINDOW(data); // Ensure this is a valid GtkWindow
  //  const gchar *url = "https://github.com/TamirRothschild/Morus";

  //  gboolean result = gtk_show_uri_on_window(window, url, GDK_CURRENT_TIME, NULL);

 //   if (!result) {
  //      g_warning("Failed to open URI: %s", url);
   // }
//}


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

void resize_advance_button(GtkWidget *widget, gpointer data) {
    GtkWidget *button = GTK_WIDGET(data);
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    int new_height = allocation.height * 0.25; // 25% of parent's height
    gtk_widget_set_size_request(button, -1, new_height); // Set height and let width adjust automatically
}

gboolean update_file_periodically(gpointer user_data) { //just a fun to call 'update_progress_bars'
    if (file_path) {
        update_progress_bars(file_path);
    }
    return TRUE; // Continue calling this function
}

static void update_font_size(GtkWidget *widget, GtkAllocation *allocation, gpointer user_data) {
    GtkCssProvider *provider = GTK_CSS_PROVIDER(user_data);
    gchar *css;

    int font_size = allocation->width / 30;

    css = g_strdup_printf("* { font-size: %dpx; }", font_size);

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    g_free(css);

    GtkStyleContext *context;

    context = gtk_widget_get_style_context(humidity_percentage_label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    context = gtk_widget_get_style_context(noise_percentage_label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    context = gtk_widget_get_style_context(fire_percentage_label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    context = gtk_widget_get_style_context(home_label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);


}


//batman-mode rip?
//void set_dark_mode() {
//g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
//}

  //  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fakesigns_ipa))) {
 //       set_dark_mode();
 //   }

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
    GtkWidget *switch_widget;
    GtkCssProvider *provider;
 //set_dark_mode(); should be a setting to force dark mode if the user wants to
 //   GtkSettings *settings = gtk_settings_get_default();
   // g_object_set(settings, "gtk-application-prefer-dark-theme", TRUE, NULL);


    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MorustestGUi-v0.2-linux");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 350);

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
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook),sets_tab, gtk_label_new("Settings"));
/////////////////////////////////////////////
    humidity_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(home_tab), humidity_progress_bar, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(humidity_progress_bar, TRUE);  // Allow horizontal expansion
    gtk_widget_set_vexpand(humidity_progress_bar, TRUE);  // Allow vertical expansion

    humidity_percentage_label = gtk_label_new("Humidity: 0.00%");
    gtk_box_pack_start(GTK_BOX(home_tab), humidity_percentage_label, FALSE, FALSE, 0);


    noise_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(home_tab), noise_progress_bar, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(noise_progress_bar, TRUE);  // Allow horizontal expansion
    gtk_widget_set_vexpand(noise_progress_bar, TRUE);  // Allow vertical expansion

    noise_percentage_label = gtk_label_new("Noise: 0.00 dB");
    gtk_box_pack_start(GTK_BOX(home_tab), noise_percentage_label, FALSE, FALSE, 0);

    fire_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(home_tab), fire_progress_bar, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(fire_progress_bar, TRUE);  // Allow horizontal expansion
    gtk_widget_set_vexpand(fire_progress_bar, TRUE);  // Allow vertical expansion

    fire_percentage_label = gtk_label_new("Fire: 0.00%");
    gtk_box_pack_start(GTK_BOX(home_tab), fire_percentage_label, FALSE, FALSE, 0);

//settings-tab
  //  arg_dark_mode = gtk_check_button_new_with_label("Force Darkness Upon us");
 //   gtk_box_pack_start(GTK_BOX(sets_tab), arg_dark_mode, TRUE, TRUE, 0);

    arg_dark_mode = gtk_check_button_new_with_label("Force Darkness Upon us (a work in progress)");
    gtk_box_pack_start(GTK_BOX(sets_tab), arg_dark_mode, TRUE, TRUE, 0);
  //  g_signal_connect(arg_dark_mode, "toggled", G_CALLBACK(on_dark_mode_toggled), NULL);

    open_url_button = gtk_button_new_with_label("Check The Source Code");
    gtk_box_pack_start(GTK_BOX(sets_tab), open_url_button, TRUE, TRUE, 0);

    // Connect the button click event to the callback function
  // g_signal_connect(open_url_button, "clicked", G_CALLBACK(on_open_url_clicked), "https://github.com/TamirRothschild/Morus");












////////////////////////////////////// too damm advance 4 me
    advance_button = gtk_button_new_with_label("Advance?");
    g_signal_connect(advance_button, "clicked", G_CALLBACK(on_advance_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(sets_tab), advance_button, TRUE, TRUE, 0);
    gtk_widget_set_size_request(advance_button, 30, 30); // Adjust size as needed

    advance_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start(GTK_BOX(sets_tab), advance_box, TRUE, TRUE, 0);

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
    gtk_box_pack_start(GTK_BOX(sets_tab), file_label, FALSE, FALSE, 0);
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



    //oudsdsds_button = gtk_button_new_with_label("Select output file");
   // g_signal_connect(oudsdsds_button, "clicked", G_CALLBACK(open_output_dialog), NULL);
   // gtk_box_pack_start(GTK_BOX(vbox), oudsdsds_button, TRUE, TRUE, 0);

    g_timeout_add_seconds(60, update_file_periodically, NULL);  //set timer call update_file_periodically every 60 seconds


   // provider = gtk_css_provider_new();
   // gtk_widget_set_size_request(window, 400, 400);
   // g_signal_connect(window, "size-allocate", G_CALLBACK(update_font_size), provider);




    gtk_widget_show_all(window);
    gtk_widget_show_all(window);
    gtk_widget_set_visible(advance_box, FALSE); // Ensure advance_box is hidden initially
   // g_signal_connect(window, "size-allocate", G_CALLBACK(update_font_size), provider);
    gtk_main();

    return 0;
}
