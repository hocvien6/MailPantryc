#ifndef PANTRYC_LIST_H_
#define PANTRYC_LIST_H_

#include <pantryc-global.h>

typedef struct pantrycList {
	struct pantrycList *next;
	struct pantrycList *previous;
	void *value;
} PantrycList;

/**
 * Initialize new pantryc doubly linked list
 * return			new list
 */
PantrycList* pantryc_list__new(void);

/**
 * Insert at the end of the list
 * @list:			list which to append node
 * @value:			value want to insert
 */
void pantryc_list__append(PantrycList *list, void *value);

/**
 * Insert at the beginning of the list
 * @list:			list which to prepend node
 * @value:			value want to insert
 */
void pantryc_list__prepend(PantrycList *list, void *value);

/**
 * Delete an arbitrary item
 * @node			node to delete
 */
void pantryc_list__delete_node(PantrycList *node);

/**
 * Check whether list is empty
 * return			list is empty or not
 */
pBOOL pantryc_list__is_empty(const PantrycList *list);

/**
 * Get value from node
 * @node			node to get value from
 * return			value
 */
void* pantryc_list__get_value(const PantrycList *node);

/**
 * Free list
 * @list			list to free
 */
void pantryc_list__free(PantrycList *list);

#define PANTRYC_LIST__GET_FIRST(list) (list->next)
#define PANTRYC_LIST__NEXT(node) (node->next)
#define PANTRYC_LIST__GET_LAST(list) (list->previous)
#define PANTRYC_LIST__PREVIOUS(node) (node->previous)
#define PANTRYC_LIST__IS_NULL(node) (!node)

#define PANTRYC_LIST__TRAVERSE(seeker, list) \
  for (seeker = list->next; seeker != list; seeker = seeker->next)
#define PANTRYC_LIST__REVERSE(seeker, list) \
  for (seeker = list->previous; seeker != list; seeker = seeker->previous)

#endif /* PANTRYC_LIST_H_ */
