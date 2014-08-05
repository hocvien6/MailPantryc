#include <stdlib.h>

#include "../include/pantryc-list.h"

void pantryc_list__insert_before(PantrycList *node, void *value);
void pantryc_list__insert_after(PantrycList *node, void *value);

PantrycList* pantryc_list__new(void) {
	PantrycList *list;
	list = (PantrycList*) malloc(sizeof(struct pantrycList));
	list->next = list;
	list->previous = list;
	return list;
}

void pantryc_list__append(list, value)
	PantrycList *list;void *value; {
	pantryc_list__insert_before(list, value);
}

void pantryc_list__prepend(list, value)
	PantrycList *list;void *value; {
	pantryc_list__insert_before(list->next, value);
}

void pantryc_list__delete_node(node)
	PantrycList *node; {
	node->next->previous = node->previous;
	node->previous->next = node->next;
	free(node);
}

int pantryc_list__is_empty(list)
	const PantrycList *list; {
	return (list->next == list);
}

void*pantryc_list__get_value(node)
	const PantrycList *node; {
	return node->value;
}

void pantryc_list__free(list)
	PantrycList *list; {
	while (!pantryc_list__is_empty(list)) {
		pantryc_list__delete_node(PANTRYC_LIST__GET_FIRST(list));
	}
	free(list);
}

/*Private functions */

void pantryc_list__insert_before(node, value)
	PantrycList *node;void *value; {
	PantrycList *newnode;
	newnode = (PantrycList*) malloc(sizeof(struct pantrycList));
	newnode->value = value;
	newnode->next = node;
	newnode->previous = node->previous;
	newnode->next->previous = newnode;
	newnode->previous->next = newnode;
}

void pantryc_list__insert_after(node, value)
	PantrycList *node;void *value; {
	pantryc_list__insert_before(node->next, value);
}
