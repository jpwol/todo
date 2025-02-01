#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Summary: Change program structure to work based off one group, made at time
 * of file creation. This simplifies the usage of the program, as well as
 * speeding up it's usage for the user.
 *
 * TODO: - Get rid of add and delete group functions.
 *       - Change arguments to not use '-' and just be the letter instead
 *       - Update help printout
 *
 *       - FINISH add_task REWRITE!
 *         - Functionality is done, add safety checks and fix indexing
 *
 * DONE: Added functionality to makefile function to add group name.
 */

int check_file_status();

char* read_file();
cJSON* read_json_data();

void handle_args(int num_args, char** args);

void make_file(char* name);
void delete_file();
void save_file(cJSON* json);

void add_task(char* task_string);
void delete_task(char* group_name, char* task_index);

void add_task_group(char* group_name);
void delete_task_group(char* group_name);

void print_json_string();
void print_help_message();
void print_no_file_message();

FILE* todo;
char* task_name = NULL;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    if (check_file_status()) {
      print_json_string();
    } else {
      print_no_file_message();
    }
    return 0;
  } else if (argc >= 2) {
    handle_args(argc, argv);
  }
  return 0;
}

// Check if the todo file exists.
int check_file_status() {
  todo = fopen("todo.json", "r");
  if (todo) {
    fclose(todo);
    return 1;
  } else {
    return 0;
  }
}

void handle_args(int num_args, char** args) {
  int file_exists = check_file_status();

  if (!strcmp(args[1], "m")) {
    make_file(args[2]);
  } else if (!strcmp(args[1], "d")) {
    if (num_args == 2)
      delete_file();
    else if (num_args == 3)
      delete_task_group(args[2]);
    else if (num_args == 4)
      delete_task(args[2], args[3]);
    else
      print_help_message();
  } else if (!strcmp(args[1], "a")) {
    if (num_args == 3)
      add_task(args[2]);
    else
      print_help_message();
  } else if (!strcmp(args[1], "-g")) {
    if (num_args == 3)
      add_task_group(args[2]);
    else
      print_help_message();
  } else if (!strcmp(args[1], "-h")) {
    print_help_message();
  }

  return;
}

void print_json_string() {
  cJSON* json = read_json_data();
  if (json == NULL) {
    fprintf(stderr, "Failed to read json data.\n");
    return;
  }

  cJSON* task_array = cJSON_GetObjectItem(json, "tasks");
  if (!cJSON_IsArray(task_array)) {
    fprintf(stderr, "Error: \"tasks\" is not an array.\n");
    return;
  }
  cJSON* group = cJSON_GetArrayItem(task_array, 0);
  cJSON* group_array = group->child;
  printf("%s:\n", group_array->string);
  int arr_len = cJSON_GetArraySize(group_array);
  for (int i = 0; i < arr_len; i++) {
    cJSON* task = cJSON_GetArrayItem(group_array, i)->child;
    printf("   %s: %s\n", task->string, task->valuestring);
  }
}

void print_help_message() {
  char* help_message = {
      "Usage: todo [option] [<group>] [task]\n"
      "Make and view todo lists in the command line\n\n"
      "If todo file exists, use 'todo' to print out todo list. Else:\n\n"
      "Options:\n"
      "   -h, --help\tprint this help message\n"
      "   -m\t\tmake a todo file\n"
      "   -d\t\tdelete a todo file/group/task\n"
      "   \t\tuse todo -d [<group>] [task]\n"
      "   -g\t\tcreate a todo group\n"
      "   \t\tuse todo -g <group>\n"
      "   -a\t\tadd a task to a group\n"
      "   \t\tuse todo -a <group> <task>\n"};

  printf("%s", help_message);
}

void print_no_file_message() {
  printf("todo: no todo file exists.\nTry todo -h for help.\n");
}

void add_task_group(char* group_name) {
  if (check_file_status()) {
    cJSON* json = read_json_data();
    cJSON* tasks_array = cJSON_GetObjectItem(json, "tasks");

    int array_size = cJSON_GetArraySize(tasks_array);
    for (int i = 0; i < array_size; i++) {
      cJSON* array_item = cJSON_GetArrayItem(tasks_array, i);
      cJSON* idk = cJSON_GetObjectItem(array_item, group_name);
      if (idk != NULL) {
        printf("group already exists.\n");
        return;
      }
    }

    cJSON* group_array = cJSON_CreateObject();
    cJSON_AddArrayToObject(group_array, group_name);
    cJSON_AddItemToArray(tasks_array, group_array);
    save_file(json);
  } else {
    print_no_file_message();
  }
}

void add_task(char* task_string) {
  if (check_file_status()) {
    cJSON* json = read_json_data();

    cJSON* tasks_array = cJSON_GetObjectItem(json, "tasks");
    cJSON* group = cJSON_GetArrayItem(tasks_array, 0);
    cJSON* group_array = group->child;

    cJSON* task = cJSON_CreateObject();
    int arr_len = cJSON_GetArraySize(group_array);
    char* buf = (char*)malloc(12);
    snprintf(buf, sizeof(buf), "%d", arr_len + 1);
    cJSON_AddStringToObject(task, buf, task_string);
    cJSON_AddItemToArray(group_array, task);
    save_file(json);
    // int array_size = cJSON_GetArraySize(tasks_array);

    //   for (int i = 0; i < array_size; i++) {
    //     cJSON* task_group = cJSON_GetArrayItem(tasks_array, i);
    //     cJSON* task_group_object = cJSON_GetObjectItem(task_group,
    //     group_name); int inner_arr_size =
    //     cJSON_GetArraySize(task_group_object);
    //
    //     if (task_group_object != NULL) {
    //       printf("todo: group found. Adding task as element %d.\n",
    //              ++inner_arr_size);
    //
    //       cJSON* json_task = cJSON_CreateObject();
    //       char* buf = (char*)malloc(12);
    //
    //       snprintf(buf, sizeof(buf), "%d", inner_arr_size);
    //       cJSON_AddStringToObject(json_task, buf, task_string);
    //       free(buf);
    //
    //       cJSON_AddItemToArray(task_group_object, json_task);
    //     }
    //   }
    //   save_file(json);
    // } else {
    //   print_no_file_message();
    // }
  }
}

void delete_task(char* group_name, char* task_index) {
  if (check_file_status()) {
    cJSON* json = read_json_data();
    cJSON* tasks_array = cJSON_GetObjectItem(json, "tasks");
    int array_size = cJSON_GetArraySize(tasks_array);

    cJSON* task_group_array;
    int inner_arr_size;
    char* endptr;
    int task_index_integer = (int)strtol(task_index, &endptr, 10);

    for (int i = 0; i < array_size; i++) {
      cJSON* task_group = cJSON_GetArrayItem(tasks_array, i);
      task_group_array = cJSON_GetObjectItem(task_group, group_name);

      if (task_group_array != NULL) {
        printf("todo: group match found\n");
        inner_arr_size = cJSON_GetArraySize(task_group_array);

        if (*endptr == '\0' && task_index_integer <= inner_arr_size) {
          cJSON_DeleteItemFromArray(task_group_array, task_index_integer - 1);
          printf("Deleting task from array.\n");

        } else
          printf("unknown input for index.\n");
      }
    }

    int index_reassign = task_index_integer;
    for (int j = task_index_integer; j < inner_arr_size; j++) {
      cJSON* array_item = cJSON_GetArrayItem(task_group_array, j - 1);
      if (array_item == NULL) {
        printf("no array item at %d\n", j);
        continue;
      }
      printf("found string at location %d.\n", j);

      char old_key[12];
      char new_key[12];
      snprintf(old_key, sizeof(old_key), "%d", j + 1);
      snprintf(new_key, sizeof(new_key), "%d", index_reassign++);
      cJSON* array_item_obj = cJSON_GetObjectItem(array_item, old_key);
      if (cJSON_IsString(array_item_obj)) {
        char* task = strdup(array_item_obj->valuestring);
        printf("%s\n", task);
        cJSON_DeleteItemFromObject(array_item, old_key);
        cJSON_AddStringToObject(array_item, new_key, task);
      }
    }

    save_file(json);
  } else {
    print_no_file_message();
  }
}

void delete_task_group(char* group_name) {
  if (check_file_status()) {
    cJSON* json = read_json_data();
    cJSON* tasks_array = cJSON_GetObjectItem(json, "tasks");
    int array_size = cJSON_GetArraySize(tasks_array);

    int group_item_found = 0;
    int group_item_index;

    for (int i = 0; i < array_size; i++) {
      cJSON* task_group = cJSON_GetArrayItem(tasks_array, i);
      cJSON* idk = cJSON_GetObjectItem(task_group, group_name);
      if (idk != NULL) {
        printf("todo: match found. Removing %s\n", idk->string);
        group_item_found = 1;
        group_item_index = i;
      }
    }

    if (group_item_found) {
      cJSON_DeleteItemFromArray(tasks_array, group_item_index);
    }

    save_file(json);
  } else {
    print_no_file_message();
  }
}

void make_file(char* name) {
  if (check_file_status()) {
    printf("todo already exists.\nUse todo -h for help\n");
  } else if (name == NULL) {
    fprintf(stderr, "Usage: todo m <task name>\nUse todo -h for help\n");
  } else {
    todo = fopen("todo.json", "w");
    fclose(todo);
    if (todo) {
      printf("todo file created\n");
      cJSON* json = read_json_data();
      json = cJSON_CreateObject();
      cJSON_AddArrayToObject(json, "tasks");
      cJSON* task_array = cJSON_GetObjectItem(json, "tasks");
      cJSON* task = cJSON_CreateObject();
      cJSON_AddArrayToObject(task, name);
      cJSON_AddItemToArray(task_array, task);
      save_file(json);
    } else {
      printf("Error creating todo file\n");
    }
  }
  return;
}

void delete_file() {
  if (check_file_status()) {
    remove("todo.json");
  } else {
    print_no_file_message();
  }
  return;
}

char* read_file() {
  int file_exists = check_file_status();
  if (!file_exists) {
    return NULL;
  }

  todo = fopen("todo.json", "rb");

  fseek(todo, 0, SEEK_END);
  long length = ftell(todo);
  fseek(todo, 0, SEEK_SET);

  char* buf = (char*)malloc(length + 1);

  fread(buf, 1, length, todo);
  buf[length] = '\0';
  fclose(todo);

  return buf;
}

void save_file(cJSON* json) {
  char* json_string = cJSON_Print(json);

  todo = fopen("todo.json", "w");

  fwrite(json_string, 1, strlen(json_string), todo);
  fclose(todo);
  free(json_string);
}

cJSON* read_json_data() {
  char* json_data = read_file();
  cJSON* json = NULL;

  if (json_data) {
    json = cJSON_Parse(json_data);
    free(json_data);
  }

  return json;
}
