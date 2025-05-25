#include "editor_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Functions about ll_text...
// append a line: add to the end of the linked list
ll_text *append_text(ll_text *list, char *text) {
  ll_text *node = malloc(sizeof(ll_text));
  if (!node) return NULL;
  node->text = strdup(text);
  node->next = NULL;
  if (!list) return node;
  ll_text *cur = list;
  while (cur->next) cur = cur->next;
  cur->next = node;
  return list;
}

void save_to_file(ll_text *list, const char *filename) {
  FILE *outfile = fopen(filename, "w");
  for (ll_text *here = list; here; here = here->next) {
    fprintf(outfile, "%s\n", here->text);
  }
  fclose(outfile);
}

int ll_text_length(ll_text *list) {
  int count = 0;
  for (; list; list = list->next) count++;
  return count;
}

// insert a specific line: add to specific place in the linked list; this is the
// new entry at the specified line
ll_text *insert_text(ll_text *list, char *text, int position) {
  ll_text *node = malloc(sizeof(ll_text));
  if (!node) return NULL;
  node->text = strdup(text);
  if (position == 0) {
    node->next = list;
    return node;
  }
  ll_text *cur = list;
  for (int i = 0; i < position - 1; i++) cur = cur->next;
  node->next = cur->next;
  cur->next = node;
  return list;
}

// delete a specific line: delete the ith entry in the linked list and return
// the new front of the linked list.
ll_text *delete_text(ll_text *list, int position) {
  if (!list) return NULL;
  if (position == 0) {
    ll_text *next = list->next;
    free(list->text);
    free(list);
    return next;
  }
  ll_text *cur = list;
  for (int i = 0; i < position - 1; i++) cur = cur->next;
  ll_text *to_del = cur->next;
  cur->next = to_del->next;
  free(to_del->text);
  free(to_del);
  return list;
}

// replace a line: go to the specific entry, free the text that's there, replace
// it with a copy of the specified text.
ll_text *replace_text(ll_text *list, char *text, int position) {
  ll_text *cur = list;
  for (int i = 0; i < position; i++) cur = cur->next;
  free(cur->text);
  cur->text = strdup(text);
  return list;
}

// duplicate a list. Returns a copy of the list, including newly copied versions
// of all of the strings.
ll_text *duplicate_ll_text(ll_text *list) {
  ll_text *new_list = NULL;
  for (ll_text *cur = list; cur; cur = cur->next) {
    new_list = append_text(new_list, cur->text);
  }
  return new_list;
}

// functions about the state stack...
ll_text_stack *load_from_file(const char *filename) {
  ll_text *lines = NULL;
  ll_text_stack *stack = calloc(1, sizeof(ll_text_stack));
  char buf[1024];
  FILE *infile = fopen(filename, "r");
  while (fgets(buf, 1024, infile)) {
    chomp(buf);
    lines = append_text(lines, buf);
  }
  stack->lines = lines;
  fclose(infile);
  return stack;
}

ll_text_stack *new_stack_empty_text(void) {
  ll_text_stack *stack = calloc(1, sizeof(ll_text_stack));
  stack->lines = NULL;
  return stack;
}

// Push a new state onto the stack that's an exact copy of the state currently
// at the top.
ll_text_stack *push_duplicate(ll_text_stack *stack) {
  ll_text_stack *s = malloc(sizeof(ll_text_stack));
  if (!s) return NULL;
  s->lines = duplicate_ll_text(stack->lines);
  s->next = stack;
  return s;
}

// Push a new entry onto the stack that has an empty ll_text at the top.
ll_text_stack *push_empty(ll_text_stack *stack) {
  ll_text_stack *s = malloc(sizeof(ll_text_stack));
  if (!s) return NULL;
  s->lines = NULL;
  s->next = stack;
  return s;
}

// undo, removing the most recent state
// this has to free all the memory associated with that state
ll_text_stack *pop_stack(ll_text_stack *stack) {
  if (!stack) return NULL;
  ll_text *cur = stack->lines;
  while (cur) {
    ll_text *next = cur->next;
    free(cur->text);
    free(cur);
    cur = next;
  }
  ll_text_stack *next_stack = stack->next;
  free(stack);
  return next_stack;
}

// Find the first newline in the input and replace it with \0
void chomp(char *s) {
  for (int i = 0; s[i]; i++) {
    if (s[i] == '\n' || s[i] == '\r') {
      // Check for \r in case we're on Windows or a very old Mac??
      // Look, I won't stop you from doing CSE 13s on a Mac from 2001 or prior.
      s[i] = '\0';
      break;
    }
  }
}
