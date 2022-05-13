#include "app_common.h"

GtkEntryBuffer* get_buffer_from_entry(gpointer user_data) {
    return gtk_entry_get_buffer(GTK_ENTRY((GtkWidget*)user_data));
}

const gchar* get_entry_text(gpointer user_data) {
    GtkEntryBuffer *buffer = get_buffer_from_entry(user_data);
    return gtk_entry_buffer_get_text(buffer);
}

const gchar* get_button_name(GtkWidget *button) {
    return gtk_label_get_text(GTK_LABEL(gtk_button_get_child(GTK_BUTTON(button))));
}

GtkButton* get_btn_from_grid(GtkBuilder *builder, int j, int i) {
    const gchar *grid_str = "grid";
    GObject *grid = gtk_builder_get_object(builder, grid_str);
    return GTK_BUTTON(gtk_grid_get_child_at(GTK_GRID(grid), j, i));
}

void delete_last_char(GtkEntryBuffer *buffer, int len, int start) {
    gtk_entry_buffer_delete_text(buffer, len - start, len);
}

int get_func_len(const char *str, int len) {
    char *func = calloc(FUNC_MAX_LEN, sizeof(char));
    if (is_open_bracket(str[len - 1])) {
        int start_index = 0;
        for (int i = len - 2; str[i] >= 'a' && str[i] <= 'z' && str[i] != 'd'; i--) {
            start_index = i;
        }
        int func_ind = 0;
        for (int i = start_index; i < len - 1; i++) {
            func[func_ind++] = str[i];
        }
    }
    int result = is_function_in_expr(func);
    free(func);
    return result;
}

void copy_func_name(char *input_str, char *func_name) {
    snprintf(input_str, FUNC_MAX_LEN, "%s", func_name);
}

int find_index_in_str(char *str, int start_index, char ch) {
    int len = strlen(str);
    int result = -1;
    for (int i = start_index; i < len; i++) {
        if (str[i] == ch) {
            result = i;
            i = len + 1;
        }
    }
    return result;
}
