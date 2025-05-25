#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "editor_lib.h"

// Extra credit globals
static bool show_numbers = true;
static bool dirty = false;

void print_help(void) {
  printf("valid commands are...\n");
  printf("- help: display this message\n");
  printf("- list: display current text in memory with line numbers (toggle with 'num')\n");
  printf("- num: toggle line numbering on/off\n");
  printf("- undo: revert to the previous state (undoing cannot be undone)\n");
  printf("- append: add a new line to the end of the text\n");
  printf("- insert: add a new line to the text at an existing line number\n");
  printf("- replace: change the text at an existing line number\n");
  printf("- delete: remove an existing line\n");
  printf("- clear: make the buffer be empty (this can be undone)\n");
  printf("- save: write the file to disk\n");
  printf("- quit: exit the program (warns if unsaved changes)\n");
}

// Prompt the user for a line number; keep asking until we get a useful answer.
int prompt_for_line_number(char *msg, int length) {
  bool good_number = false;
  int linenum;
  char buf[1024];
  while (!good_number) {
    printf("%s\n", msg);
    if (!fgets(buf, sizeof buf, stdin)) exit(0);
    chomp(buf);
    linenum = strtol(buf, NULL, 10) - 1;
    if (linenum >= 0 && linenum < length) {
      good_number = true;
    } else {
      printf("invalid line number.\n");
    }
  }
  return linenum;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "USAGE: editor <filename>\n");
    return 0;
  }
  const char *filename = argv[1];

  printf("**************************************\n");
  printf("* welcome to the cse13s text editor! *\n");
  printf("**************************************\n");
  printf("editing %s\n", filename);

  const int BUFSIZE = 1024;
  char buf[BUFSIZE];
  char *result;

  ll_text_stack *stack = NULL;
  if (access(filename, F_OK) == 0) {
    stack = load_from_file(filename);
    printf("loaded %s from disk\n", filename);
  } else {
    stack = new_stack_empty_text();
  }
  while (true) {
    printf("command: ");
    result = fgets(buf, BUFSIZE, stdin);
    if (!result) {
      break;
    }
    chomp(buf);

    if (!strcmp(buf, "quit") || !strcmp(buf, "q")) {
      if (dirty) {
        printf("Unsaved changes—quit anyway? (y/N) ");
        int c = getchar();
        while (getchar() != '\n' && !feof(stdin));
        if (c == 'y' || c == 'Y') break;
      } else {
        break;
      }

    } else if (!strcmp(buf, "help") || !strcmp(buf, "?")) {
      print_help();

    } else if (!strcmp(buf, "list")) {
      ll_text *here = stack->lines;
      for (int i = 1; here; i++) {
        if (show_numbers)
          printf("%4d|%s\n", i, here->text);
        else
          printf("%s\n", here->text);
        here = here->next;
      }

    } else if (!strcmp(buf, "num")) {
      show_numbers = !show_numbers;
      printf("Line numbers %s\n", show_numbers ? "enabled" : "disabled");

    } else if (!strcmp(buf, "save")) {
      save_to_file(stack->lines, filename);
      printf("saved to %s\n", filename);
      dirty = false;

    } else if (!strcmp(buf, "append")) {
      printf("type the new line to add to the file.\n");
      result = fgets(buf, BUFSIZE, stdin);
      chomp(buf);

      // make a new stack entry so we can undo
      stack = push_duplicate(stack);
      stack->lines = append_text(stack->lines, buf);
      dirty = true;

    } else if (!strcmp(buf, "insert")) {
      int length = ll_text_length(stack->lines);
      if (!length) {
        printf("can't insert; there are currently no lines. use append instead.\n");
        continue;
      }
      int linenum = prompt_for_line_number(
          "at which line number (1-indexed) would you like to insert?", length);
      printf("type the new line to add to the file at that line number.\n");
      result = fgets(buf, BUFSIZE, stdin);
      chomp(buf);

      // make a new stack entry so we can undo
      stack = push_duplicate(stack);
      stack->lines = insert_text(stack->lines, buf, linenum);
      dirty = true;

    } else if (!strcmp(buf, "replace")) {
      int length = ll_text_length(stack->lines);
      if (!length) {
        printf("can't replace; there are currently no lines.\n");
        continue;
      }
      int linenum = prompt_for_line_number(
          "which line number (1-indexed) would you like to replace?", length);
      printf("type the new line to put at that line number.\n");
      result = fgets(buf, BUFSIZE, stdin);
      chomp(buf);

      // make a new stack entry so we can undo
      stack = push_duplicate(stack);
      stack->lines = replace_text(stack->lines, buf, linenum);
      dirty = true;

    } else if (!strcmp(buf, "delete")) {
      int length = ll_text_length(stack->lines);
      if (!length) {
        printf("can't delete; there are currently no lines.\n");
        continue;
      }
      int linenum = prompt_for_line_number(
          "which line number (1-indexed) would you like to delete?", length);

      // make a new stack entry so we can undo
      stack = push_duplicate(stack);
      stack->lines = delete_text(stack->lines, linenum);
      dirty = true;

    } else if (!strcmp(buf, "clear")) {
      // push an empty ll_text onto the stack
      stack = push_empty(stack);
      dirty = true;

    } else if (!strcmp(buf, "undo")) {
      // pop from the stack
      stack = pop_stack(stack);
      if (!stack) {
        stack = new_stack_empty_text();
      }
      dirty = true;

    } else {
      printf("unknown command: %s\n", buf);
    }
  }
  // done with the loop. Clean up by undoing until the stack is empty.
  while (stack) {
    stack = pop_stack(stack);
  }

  printf("bye!\n");
  return 0;
}
