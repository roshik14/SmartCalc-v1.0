#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#include <gtk/gtk.h>
#include "checks.h"


#define FUNC_MAX_LEN 10
#define DOUBLE_MAX 100
#ifdef __APPLE__
    #define BACK_KEYCODE 51
#elif __linux__
    #define BACK_KEYCODE 22
#endif
#define ENTER_KEYCODE 36

struct status_struct;
struct string_struct;

GtkEntryBuffer* get_buffer_from_entry(gpointer user_data);
const gchar* get_entry_text(gpointer user_data);
const gchar* get_button_name(GtkWidget *button);
GtkButton* get_btn_from_grid(GtkBuilder *builder, int j, int i);
void delete_last_char(GtkEntryBuffer *buffer, int len, int start);
int get_func_len(const char *str, int len);
void copy_func_name(char *input_str, char *func_name);
int find_index_in_str(char*, int, char);
#endif  // SRC_S21_APP_COMMON_H_
