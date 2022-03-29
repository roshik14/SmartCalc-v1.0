#include <gtk/gtk.h>
#include <cairo.h>
#include <locale.h>
#include "app_common.h"

static GtkWidget *err_msg = NULL;
static double x_range = 0;
static double y_range = 0;

static struct status_struct {
    int bracket;
    int dot;
    gboolean operation;
    gboolean input;
    gboolean clear;
    gboolean x;
    gboolean x_in_str;
    gboolean window;
} status;

static struct strings_struct {
    const gchar *start_text;
    const gchar *error;
    const gchar *click_event;
    const gchar *nan_error;
} string;


typedef struct data_struct {
    GtkWidget *entry_with_x_value;
    GtkWidget *global_entry;
} entry_data_t;

typedef struct range_struct {
    GtkWidget *global_entry;
    GtkWidget *spin_from;
    GtkWidget *spin_to;
} range_t;

static void init_structs() {
    status.bracket = 0;
    status.dot = 0;
    status.operation = false;
    status.input = false;
    status.clear = false;
    status.x = false;
    status.x_in_str = false;
    status.window = false;
    string.start_text = "0";
    string.error = "Error";
    string.click_event = "clicked";
    string.nan_error = "Can't find answer";
}

static void close_window(GtkWidget* window, gpointer user_data) {
    gtk_window_destroy(GTK_WINDOW(window));
    status.window = false;
    status.input = false;
}

static int from_begin(const gchar *entry_text, char symb, int func) {
    return g_str_equal(entry_text, string.start_text)
        && (is_number(symb) || is_unary(symb) || func || is_x(symb));
}

static void input_symbol(const gchar *btn_name, gpointer user_data) {
    GtkEntryBuffer *buffer = get_buffer_from_entry(user_data);
    const gchar *entry_text = get_entry_text(user_data);
    int entry_text_len = strlen(entry_text);
    int name_len = strlen(btn_name);
    int func = is_function_in_expr((char*)btn_name);
    const gchar symb = btn_name[0];
    int insert_pos = 0;
    if (from_begin(entry_text, symb, func)) {
        delete_last_char(buffer, entry_text_len, entry_text_len);
        insert_pos = 0;
    } else {
        insert_pos = entry_text_len + 1;
    }
    gtk_entry_buffer_insert_text(buffer, insert_pos, btn_name, name_len);
    if (func) {
        gtk_entry_buffer_insert_text(buffer, insert_pos + func + 1, "(", 1);
        if (!is_unary(symb))
            status.operation = true;
        status.bracket++;
    }
}

static void input_number_to_entry(const gchar *btn_name, gpointer user_data) {
    if (!status.input) {
        input_symbol(btn_name, user_data);
        status.dot = false;
        status.operation = false;
        status.clear = false;
    }
}

static void input_operation_to_entry(const gchar *btn_name, gpointer user_data) {
    const gchar *entry_text = get_entry_text(user_data);
    int len = strlen(entry_text);
    int unary = status.operation && status.bracket && is_unary(btn_name[0]) && !is_unary(entry_text[len - 1]);
    if ((!status.operation || unary) && !status.input) {
        input_symbol(btn_name, user_data);
        status.operation = true;
        status.clear = false;
        status.dot = true;
        status.x = false;
    }
}

static void input_open_bracket_to_entry(const gchar *btn_name, gpointer user_data) {
    if (status.operation && !status.input) {
        input_symbol(btn_name, user_data);
        status.bracket++;
        status.clear = false;
        status.x = false;
    }
}

static void input_close_bracket_to_entry(const gchar *btn_name, gpointer user_data) {
    if (status.bracket && !status.input) {
        input_symbol(btn_name, user_data);
        status.bracket--;
        status.clear = false;
        status.operation = false;
    }
}

static void input_function_to_entry(const gchar *btn_name, gpointer user_data) {
    const gchar *entry_text = get_entry_text(user_data);
    int func_len = get_func_len(entry_text, strlen(entry_text));
    if (!status.input || (g_str_equal(entry_text, string.start_text) || func_len)) {
        input_symbol(btn_name, user_data);
        status.clear = false;
    }
}

static void input_null_to_entry(const gchar *btn_name, gpointer user_data) {
    const gchar *entry_text = get_entry_text(user_data);
    if (!status.input || !g_str_equal(entry_text, string.start_text)) {
        input_symbol(btn_name, user_data);
        status.operation = false;
        status.clear = false;
        status.dot = false;
        status.x = true;
    }
}

static void input_dot_to_entry(const gchar *btn_name, gpointer user_data) {
    if (!status.input && !status.dot) {
        input_symbol(btn_name, user_data);
        status.operation = true;
        status.clear = false;
        status.dot = true;
        status.x = false;
    }
}

static void input_x_to_entry(const gchar *btn_name, gpointer user_data) {
    if (!status.input && status.operation || (!status.operation && !status.x)) {
        input_symbol(btn_name, user_data);
        status.clear = false;
        status.dot = true;
        status.x = true;
        status.operation = false;
    }
}


static void undo_last_action(GtkEntryBuffer *buffer) {
    const gchar *str = gtk_entry_buffer_get_text(buffer);
    int len = strlen(str);
    int func_len = get_func_len(str, len);
    char last = str[len-1];
    if (func_len && is_open_bracket(last)) {
        delete_last_char(buffer, len, func_len + 1);
        status.bracket--;
    } else if (is_number(last)) {
        delete_last_char(buffer, len, 1);
        status.operation = true;
    } else if (is_binary_operator(last)) {
        delete_last_char(buffer, len, 1);
        status.operation = false;
    } else if (is_open_bracket(last)) {
        delete_last_char(buffer, len, 1);
        if (status.bracket > 0)
            status.bracket--;
    } else if (is_close_bracket(last)) {
        delete_last_char(buffer, len, 1);
        status.bracket++;
    } else if (len >= 3 && is_mod((char*)&str[len -3])) {
        delete_last_char(buffer, len, 3);
        status.operation = false;
    } else if (is_dot(last)) {
        delete_last_char(buffer, len, 1);
        status.dot = false;
    } else {
        delete_last_char(buffer, len, 1);
        status.x = false;
    }
    int emtpy = len - 1 == 0 || (func_len && (len - (func_len + 1)) == 0);
    if (emtpy) {
        gtk_entry_buffer_delete_text(buffer, 0, -1);
        gtk_entry_buffer_set_text(buffer, string.start_text, 1);
    }
}

static void update_after_clear() {
    status.input = false;
    status.operation = false;
    status.x = false;
    status.bracket = 0;
}

static void clear_input(gpointer user_data) {
    GtkWidget *entry = (GtkWidget*)user_data;
    GtkEntryBuffer *buffer = get_buffer_from_entry(entry);
    if (!status.window) {
        if (status.clear) {
            gtk_entry_buffer_delete_text(buffer, 0, -1);
            gtk_entry_buffer_set_text(buffer, string.start_text, 1);
            update_after_clear();
            gtk_widget_set_visible(err_msg, false);
        } else {
            undo_last_action(buffer);
        }
    }
}

static void clear_input_from_button(GtkWidget *button, gpointer user_data) {
    clear_input(user_data);
}

static void show_answer(GtkEntryBuffer *buffer, char *result) {
    if (!result) {
        gtk_widget_set_visible(err_msg, true);
        status.input = true;
        status.clear = true;
    } else if (g_str_equal(result, "nan") || g_str_equal(result, "-nan")) {
        gtk_entry_buffer_delete_text(buffer, 0, -1);
        gtk_entry_buffer_set_text(buffer, string.nan_error, strlen(string.nan_error));
        g_free(result);
        status.input = true;
    } else {
        gtk_entry_buffer_delete_text(buffer, 0, -1);
        gtk_entry_buffer_set_text(buffer, result, strlen(result));
        g_free(result);
    }
}

static void check_entry_text(gpointer user_data) {
    const gchar *str = get_entry_text(user_data);
    if (is_invalid_expr(str) || status.bracket > 0) {
        gtk_widget_set_visible(err_msg, true);
        status.clear = true;
        status.input = true;
    }
}

static void print_answer_to_entry(gpointer user_data) {
    const gchar *str = get_entry_text(user_data);
    GtkEntryBuffer *buffer = get_buffer_from_entry(user_data);
    check_entry_text(user_data);
    if (!g_str_equal(string.error, str) && status.bracket <= 0) {
        char *answer = NULL;
        if (check_for_x(str)) {
            status.x_in_str = true;
        } else {
            status.x_in_str = false;
            answer = calculate((gchar*)str);
            show_answer(buffer, answer);
        }
        status.clear = true;
    }
}

static void on_dialog_pressed_ok(gpointer user_data) {
    entry_data_t *data = user_data;
    GtkEntryBuffer *buffer = get_buffer_from_entry(data->global_entry);
    const gchar *value_for_x = get_entry_text(data->entry_with_x_value);
    const gchar *global_entry_text = get_entry_text(data->global_entry);
    guint x_str_len = strlen(value_for_x);
    guint global_text_len = strlen(global_entry_text);
    for (guint i = 0; i < global_text_len; i++) {
        int x_pos = find_index_in_str((char*)global_entry_text, i, 'x');
        if (x_pos != -1) {
            gtk_entry_buffer_delete_text(buffer, x_pos, 1);
            gtk_entry_buffer_insert_text(buffer, x_pos, value_for_x, x_str_len);
        }
    }
    print_answer_to_entry(data->global_entry);
}

static void on_dialog_response(GtkDialog *dialog, int response, gpointer user_data) {
    if (response == GTK_RESPONSE_OK) {
        on_dialog_pressed_ok(user_data);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void on_dialog_key_pressed(
        GtkEventController *controller,
        guint keyval,
        guint keycode,
        GdkModifierType state,
        gpointer user_data) {
    entry_data_t *data = user_data;
    char key_str[2] = {0};
    snprintf(key_str, sizeof(key_str), "%c", keyval);
    if (is_number(keyval)) {
        input_number_to_entry(key_str, data->entry_with_x_value);
    } else if (is_dot(keyval)) {
        input_dot_to_entry(key_str, data->entry_with_x_value);
    } else if (keycode == BACK_KEYCODE) {
        clear_input(data->entry_with_x_value);
    }
}

static void show_dialog(gpointer user_data) {
    GtkWidget *window = gtk_window_new();
    GtkBuilder *dialog_builder = gtk_builder_new_from_file("ui/dialog_builder.ui");
    GObject *dialog_win = gtk_builder_get_object(dialog_builder, "dialog1");
    gtk_window_set_transient_for(GTK_WINDOW(dialog_win), GTK_WINDOW(window));

    GObject *entry = gtk_builder_get_object(dialog_builder, "dialog_entry");
    GObject *controller = gtk_builder_get_object(dialog_builder, "dialog_controller");

    entry_data_t *data = g_new(entry_data_t, 1);
    data->entry_with_x_value = GTK_WIDGET(entry);
    data->global_entry = GTK_WIDGET(user_data);

    g_signal_connect(GTK_EVENT_CONTROLLER(controller),
            "key-pressed",
            G_CALLBACK(on_dialog_key_pressed),
            data);

    g_signal_connect_data(dialog_win, "response",
            G_CALLBACK(on_dialog_response),
            data,
            (GClosureNotify) g_free,
            0);

    gtk_widget_show(GTK_WIDGET(dialog_win));
    g_object_unref(dialog_builder);
}

static void print_answer_from_button(GtkWidget *button, gpointer user_data) {
    check_entry_text(user_data);
    if (!status.input) {
        const gchar *str = get_entry_text(user_data);
        if (check_for_x(str) && !status.window)
            show_dialog(user_data);
        else
            print_answer_to_entry(user_data);
    }
}

static void input_key_to_entry(gchar *input_key, guint keyval, gpointer user_data) {
    if (is_number(keyval) && keyval != '0')
        input_number_to_entry(input_key, user_data);
    else if (is_binary_operator(keyval) || keyval == 'm')
        input_operation_to_entry(input_key, user_data);
    else if (is_open_bracket(keyval))
        input_open_bracket_to_entry(input_key, user_data);
    else if (is_close_bracket(keyval))
        input_close_bracket_to_entry(input_key, user_data);
    else if (is_dot(keyval))
        input_dot_to_entry(input_key, user_data);
    else if (is_x(keyval))
        input_x_to_entry(input_key, user_data);
    else if (keyval == '0')
        input_null_to_entry(input_key, user_data);
    else if (is_function_key(keyval))
        input_function_to_entry(input_key, user_data);
}

static void show_key(
        GtkEventController *controller,
        guint keyval, guint keycode,
        GdkModifierType state,
        gpointer user_data) {
    gchar input_key[10] = {0};
    if ((!is_function_key(keyval) && keyval != 'm') || is_x(keyval)) {
        snprintf(input_key, sizeof(input_key), "%c", keyval);
    } else if (keyval == 'm') {
        snprintf(input_key, sizeof(input_key), "%s", "mod");
    } else if (is_function_key(keyval)) {
        char *func_name = get_function_name(keyval);
        copy_func_name(input_key, func_name);
        g_free(func_name);
    }
    input_key_to_entry(input_key, keyval, user_data);
}

static char* get_replaced_on_x(const gchar *entry_text, char *x_str) {
    int len = strlen(entry_text);
    int x_len = strlen(x_str);
    guint x_count = 0;
    for (guint i = 0; i < len; i++) {
        int x_pos = find_index_in_str((char*)entry_text, i, 'x');
        if (x_pos != -1) {
            i+= x_pos;
            x_count++;
        }
    }
    int replaced_str_len = len*x_count*x_len;
    char *replaced_str = calloc(len*x_count*strlen(x_str), sizeof(char));
    for (int i = 0, g_i = 0; i < replaced_str_len; i++, g_i++) {
        if (is_x(entry_text[g_i])) {
            for (int j = i, k = 0; k < x_len; k++) {
                replaced_str[j++] = x_str[k];
            }
            i+= x_len - 1;
        } else {
            replaced_str[i] = entry_text[g_i];
        }
    }
    return replaced_str;
}

static double get_y_value(const gchar *global_text, gdouble x) {
    setlocale(LC_ALL, "en_US.utf8");
    double y = 0;
    int x_max = 50;
    char *x_str = calloc(x_max, sizeof(char));
    g_snprintf(x_str, x_max, "(%.1lf)", x);
    char *replaced = get_replaced_on_x(global_text, x_str);
    char *y_str = calculate(replaced);
    if (y_str == NULL)
        y = NAN;
    else
        sscanf(y_str, "%lf", &y);
    g_free(replaced);
    g_free(x_str);
    g_free(y_str);
    return y;
}

static void draw_coord(cairo_t * cr, double x1, double y1, double x2, double y2, const gchar *text) {
    cairo_move_to(cr, x1, y1);
    if (text == NULL) {
        cairo_line_to(cr, x2, y2);
    } else {
        cairo_show_text(cr, text);
    }
    cairo_stroke(cr);
}

static void change_color(cairo_t *cr, const gchar *color, GdkRGBA *var) {
    gdk_rgba_parse(var, color);
    gdk_cairo_set_source_rgba(cr, var);
}

static double get_steps_col() {
    double max = 10000;
    double first_n = x_range;
    if (x_range == 10) {
        first_n = 1;
    } else {
        while (first_n > 10)
            first_n /= 10;
    }
    double steps_col = x_range < 100.0 ? 100.0 * first_n : max * first_n;
    return steps_col;
}

static void draw_graph(
        GtkDrawingArea *drawing_area,
        cairo_t        *cr,
        int             width,
        int             height,
        gpointer        user_data) {
    GdkRGBA color;

    change_color(cr, "black", &color);
    double w_center = width / 2;
    double h_center = height / 2;
    cairo_set_line_width(cr, 1);
    cairo_set_font_size(cr, 15);
    draw_coord(cr, 0, h_center, width, h_center, NULL);
    draw_coord(cr, w_center, 0, w_center, height, NULL);
    draw_coord(cr, w_center+5, h_center-5, 0, 0, "0");
    draw_coord(cr, width-15, h_center-20, 0, 0, "x");
    draw_coord(cr, w_center+10, 20, 0, 0, "y");

    change_color(cr, "blue", &color);
    double line_width = x_range * 0.002 + y_range * 0.002;
    double steps_col = get_steps_col();
    double step = (x_range) / (steps_col);
    double x_scale = width/(x_range *2);
    double y_scale = (height/(y_range*2))*-1;
    cairo_translate(cr, w_center, h_center);
    cairo_set_line_width(cr, line_width);
    cairo_scale(cr, x_scale, y_scale);
    const gchar *entry_text = get_entry_text(user_data);
    for (double x = x_range * -1; x <= x_range; x += step) {
        double y = get_y_value(entry_text, x);
        int not_draw = fabs(fabs(atan(y)) - M_PI_2) > 0.1;
        if (not_draw && !isnan(y))
            cairo_line_to(cr, x, y);
        else
            cairo_new_sub_path(cr);
    }
    cairo_stroke(cr);
}

static void show_graph_window(gpointer user_data) {
    GtkBuilder *graph_builder = gtk_builder_new_from_file("ui/graph_builder.ui");

    GObject *graph_window = gtk_builder_get_object(graph_builder, "graph_window");
    GObject *draw_area = gtk_builder_get_object(graph_builder, "draw_area");

    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(draw_area), draw_graph, user_data, NULL);
    g_signal_connect(GTK_WIDGET(graph_window), "destroy", G_CALLBACK(close_window), NULL);

    g_object_unref(graph_builder);
    gtk_widget_show(GTK_WIDGET(graph_window));
}

static void equal_and_back_on_key(
        GtkEventController *controller,
        guint keyval,
        guint keycode,
        GdkModifierType state,
        gpointer user_data) {
    const gchar *str = get_entry_text(user_data);
    if (keycode == ENTER_KEYCODE) {
        check_entry_text(user_data);
        if (!status.input) {
            if (check_for_x(str))
                show_dialog(user_data);
            else
                print_answer_to_entry(user_data);
        }
    } else if (keycode == BACK_KEYCODE) {
        clear_input(user_data);
    }
}

static void on_graph_dialog_response(GtkDialog *dialog, int response, gpointer user_data) {
    if (response == GTK_RESPONSE_OK) {
        range_t *data = user_data;
        x_range = gtk_spin_button_get_value(GTK_SPIN_BUTTON(data->spin_from));
        y_range = gtk_spin_button_get_value(GTK_SPIN_BUTTON(data->spin_to));
        show_graph_window(data->global_entry);
    } else {
        status.input = false;
        status.window = false;
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void input_range(gpointer user_data) {
    GtkWidget *window = gtk_window_new();
    GtkBuilder *range_builder = gtk_builder_new_from_file("ui/range_builder.ui");
    GObject *dialog_win = gtk_builder_get_object(range_builder, "dialog1");
    gtk_window_set_transient_for(GTK_WINDOW(dialog_win), GTK_WINDOW(window));

    GObject *spin_btn_1 = gtk_builder_get_object(range_builder, "spin_btn_1");
    GObject *spin_btn_2 = gtk_builder_get_object(range_builder, "spin_btn_2");
    range_t *data = g_new(range_t, 1);
    data->global_entry = GTK_WIDGET(user_data);
    data->spin_from = GTK_WIDGET(spin_btn_1);
    data->spin_to = GTK_WIDGET(spin_btn_2);
    g_signal_connect_data(
            dialog_win,
            "response",
            G_CALLBACK(on_graph_dialog_response),
            data,
            (GClosureNotify) g_free,
            0);

    gtk_widget_show(GTK_WIDGET(dialog_win));
    g_object_unref(range_builder);
}

static void show_graph_on_button(GtkWidget *button, gpointer user_data) {
    const gchar *str = get_entry_text(user_data);
    check_entry_text(user_data);
    if (check_for_x(str))
        status.x_in_str = true;
    if (!status.window && status.x_in_str && !status.input) {
        input_range(user_data);
        status.window = true;
        status.input = true;
    }
}

static void input_number_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_number_to_entry(btn_name, user_data);
}

static void input_operation_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_operation_to_entry(btn_name, user_data);
}

static void input_open_bracket_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_open_bracket_to_entry(btn_name, user_data);
}

static void input_close_bracket_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_close_bracket_to_entry(btn_name, user_data);
}

static void input_function_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_function_to_entry(btn_name, user_data);
}

static void input_null_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_null_to_entry(btn_name, user_data);
}

static void input_dot_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_dot_to_entry(btn_name, user_data);
}

static void input_x_from_button(GtkWidget *button, gpointer user_data) {
    const gchar *btn_name = get_button_name(button);
    input_x_to_entry(btn_name, user_data);
}

static void activate_buttons_number(GtkBuilder *builder, GtkEntry *entry) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
                GtkButton *curr_btn = get_btn_from_grid(builder, j, i);
                g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_number_from_button), entry);
        }
    }
}

static void activate_buttons_operation(GtkBuilder *builder, GtkEntry *entry) {
    for (int i = 0; i < 2; i++) {
        for (int j = 3; j < 7; j++) {
            gboolean skip = false;
            if (i == 1 && (j == 5 || j == 6))
                skip = true;
            if (!skip) {
                GtkButton *curr_btn = get_btn_from_grid(builder, j, i);
                g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_operation_from_button), entry);
            }
        }
    }
}

static void activate_buttons_function(GtkBuilder *builder, GtkEntry *entry) {
    for (int i = 2; i < 5; i++) {
        for (int j = 3; j < 6; j++) {
                GtkButton *curr_btn = get_btn_from_grid(builder, j, i);
                g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_function_from_button), entry);
        }
    }
}

static void activate_button_open_bracket(GtkBuilder *builder, GtkEntry *entry) {
    GtkButton *curr_btn = get_btn_from_grid(builder, 5, 1);
    g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_open_bracket_from_button), entry);
}

static void activate_button_close_bracket(GtkBuilder *builder, GtkEntry *entry) {
    GtkButton *curr_btn = get_btn_from_grid(builder, 6, 1);
    g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_close_bracket_from_button), entry);
}

static void activate_button_null_number(GtkBuilder *builder, GtkEntry *entry) {
    GtkButton *curr_btn = get_btn_from_grid(builder, 0, 3);
    g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_null_from_button), entry);
}

static void activate_button_dot(GtkBuilder *builder, GtkEntry *entry) {
    GtkButton *curr_btn = get_btn_from_grid(builder, 1, 3);
    g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_dot_from_button), entry);
}

static void activate_button_x(GtkBuilder *builder, GtkEntry *entry) {
    GtkButton *curr_btn = get_btn_from_grid(builder, 2, 3);
    g_signal_connect(curr_btn, "clicked", G_CALLBACK(input_x_from_button), entry);
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkBuilder *builder = gtk_builder_new_from_file("ui/main_builder.ui");
    GObject *window = gtk_builder_get_object(builder, "window");
    gtk_window_set_application(GTK_WINDOW(window), app);

    GObject *entry = gtk_builder_get_object(builder, "entry");
    GtkEntryBuffer *buffer = get_buffer_from_entry(entry);
    gtk_entry_buffer_set_text(buffer, "0", 1);

    activate_buttons_number(builder, GTK_ENTRY(entry));
    activate_buttons_operation(builder, GTK_ENTRY(entry));
    activate_buttons_function(builder, GTK_ENTRY(entry));
    activate_button_open_bracket(builder, GTK_ENTRY(entry));
    activate_button_close_bracket(builder, GTK_ENTRY(entry));
    activate_button_null_number(builder, GTK_ENTRY(entry));
    activate_button_dot(builder, GTK_ENTRY(entry));
    activate_button_x(builder, GTK_ENTRY(entry));

    GtkEventController *controller = gtk_event_controller_key_new();
    gtk_event_controller_set_name(controller, "main_controller");
    gtk_widget_add_controller(GTK_WIDGET(window), controller);
    g_signal_connect(controller, "key-pressed", G_CALLBACK(show_key), entry);
    g_signal_connect(controller, "key-pressed", G_CALLBACK(equal_and_back_on_key), entry);

    GObject *button_eq = gtk_builder_get_object(builder, "btn_equal");
    g_signal_connect(button_eq, "clicked", G_CALLBACK(print_answer_from_button), entry);

    GObject *button_graph = gtk_builder_get_object(builder, "btn_graph");
    g_signal_connect(button_graph, "clicked", G_CALLBACK(show_graph_on_button), entry);

    GObject *button_clear = gtk_builder_get_object(builder, "clear");
    g_signal_connect(button_clear, "clicked", G_CALLBACK(clear_input_from_button), entry);

    GObject *infobar = gtk_builder_get_object(builder, "error_msg");
    err_msg = GTK_WIDGET(infobar);

    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    gtk_widget_show(GTK_WIDGET(window));

    g_object_unref(builder);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "en_US.utf8");
    GtkApplication *app = gtk_application_new("s21.smart.calc.app", G_APPLICATION_FLAGS_NONE);
    init_structs();
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return 0;
}
