#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Summary: Change program structure to work based off one group, made at time
 * of file creation. This simplifies the usage of the program, as well as
 * speeding up it's usage for the user.
 *
 * TODO: DONE: Get rid of add and delete group functions.
 *       DONE: Change arguments to not use '-' and just be the letter instead
 *       DONE: Update help printout
 *       - Add safety checks throughout functions
 *       DONE: For the love of God, add comments for documentation purposes. I'm
 *             so tired of opening this file and being lost.
 *       DONE: Add functionality to makefile function to add group name.
 *
 *       DONE: FINISH add_task REWRITE!
 *
 *       EDIT: I LOVE MEMORY LEAKS! RAM IS NOT REAL!
 */

int check_file_status();

char* read_file();
cJSON* read_json_data();

void handle_args(int num_args, char** args);

void make_file(char* name);
void delete_file();
void save_file(cJSON* json);

void add_task(char* task_string);
void delete_task(char* task_index);

void print_json_string();
void print_help_message();
void print_no_file_message();

FILE* todo;
char* task_name = NULL;

int main(int argc, char* argv[]) {
  // If args are less than two, that means the user just ran "todo", which
  // should print out the file. Must make sure file exists before trying that.
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
  if (!strcmp(args[1], "-m") || !strcmp(args[1], "--make")) {
    make_file(args[2]);
  } else if (!strcmp(args[1], "-d") || !strcmp(args[1], "--delete")) {
    if (num_args == 2)
      delete_file();
    else if (num_args == 3)
      delete_task(args[2]);
    else
      print_help_message();
  } else if (!strcmp(args[1], "-a") || !strcmp(args[1], "--add")) {
    // if (num_args == 3)
    add_task(args[2]);
    // else
    // print_help_message();
  } else if (!strcmp(args[1], "-h") || !strcmp(args[1], "--help")) {
    print_help_message();
  }
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
      "Usage: todo [option] [<task>]\n"
      "Make and view todo lists in the command line\n\n"
      "If todo file exists, use 'todo' to print out todo list. Else:\n\n"
      "Options:\n"
      "   -h, --help\t\tprint this help message\n"
      "   -m, --make\t\tmake a todo file\n"
      "   \t\t\t- use todo -m <name>\n"
      "   -d, --delete\t\tdelete a todo file/task\n"
      "   \t\t\t- use todo -d [task index]\n"
      "   -a, --add\t\tadd a task\n"
      "   \t\t\t- use todo -a <task>\n"};

  printf("%s", help_message);
}

void print_no_file_message() {
  printf("todo: no todo file exists.\nTry todo -h for help.\n");
}

void add_task(char* task_string) {
  if (check_file_status()) {
    if (task_string == NULL) {
      fprintf(stderr, "Usage: todo -a <task>\nTry todo -h for help.\n");
    } else {
      cJSON* json = read_json_data();

      // Could consolidate group and group_array to be cJSON* group =
      // cJSON_GetArrayItem(tasks_array, 0)->child but I'm tired.
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
    }
  } else {
    print_no_file_message();
  }
}

// What an awful function to write honestly.
void delete_task(char* task_index) {
  if (check_file_status()) {
    cJSON* json = read_json_data();
    cJSON* task_array = cJSON_GetObjectItem(json, "tasks");
    cJSON* group_array = cJSON_GetArrayItem(task_array, 0)->child;
    int arr_len = cJSON_GetArraySize(group_array);
    char* endptr;
    // not a fan of strtol
    int num = strtol(task_index, &endptr, 10);

    // simple safety check to make sure the deletion index is within the array
    if (num < 1 || num > arr_len) {
      printf("Requested action is out of bounds\n");
      return;
    }
    // What happens here is we're creating a copy of the existing group_array,
    // but without the task that the user wants to delete. I've tried doing this
    // about 200 other ways and it's just not worth it for an implementation
    // like this. I can't see anyone having a todo list 200 items long but who
    // knows.
    //
    // May revisit in the future.
    //
    // Here is the new empty array
    cJSON* new_group_array = cJSON_CreateArray();

    // if (i == num - 1) continue; just skips over the item that the user wants
    // to delete
    for (int i = 0; i < arr_len; i++) {
      if (i == num - 1) continue;

      cJSON* task = cJSON_GetArrayItem(group_array, i)->child;
      char* task_string = task->valuestring;
      char buf[12];
      snprintf(buf, sizeof(buf), "%d", cJSON_GetArraySize(new_group_array) + 1);

      cJSON* new_index_task = cJSON_CreateObject();
      cJSON_AddStringToObject(new_index_task, buf, task_string);
      cJSON_AddItemToArray(new_group_array, new_index_task);
    }

    // Recreating the object within "tasks" that holds the array, but with our
    // new fancy array.
    const char* group_name = group_array->string;
    cJSON* new_group_obj = cJSON_CreateObject();
    cJSON_AddItemToObject(new_group_obj, group_name, new_group_array);
    cJSON_ReplaceItemInArray(task_array, 0, new_group_obj);

    save_file(json);
  } else {
    print_no_file_message();
  }
}

void make_file(char* name) {
  if (check_file_status()) {
    printf("todo already exists.\nUse todo -h for help\n");
  } else if (name == NULL) {
    fprintf(stderr, "Usage: todo -m <task name>\nUse todo -h for help\n");
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
    char answer;
    fprintf(stdout, "Really delete todo file? (y/n): ");
    fscanf(stdin, "%c", &answer);
    if (answer == 'y') {
      fprintf(stdout, "Deleting file...\n");
      remove("todo.json");
    } else {
      fprintf(stdout, "Aborting deletion...\n");
    }
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
