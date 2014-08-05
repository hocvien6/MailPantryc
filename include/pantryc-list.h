#ifndef PANTRYC_LIST_H_
#define PANTRYC_LIST_H_

typedef struct pantrycList {
	struct pantrycList *next;
	struct pantrycList *previous;
	void *value;
}*PantrycList;

PantrycList pantryc_list__new();

/**
 * Insert at the end of the list
 * @list:			list which to append node
 * @value:			value want to insert
 */
void pantryc_list__append(PantrycList list, void *value);

/**
 * Insert at the beginning of the list
 * @list:			list which to prepend node
 * @value:			value want to insert
 */
void pantryc_list__prepend(PantrycList list, void *value);

/**
 * Delete an arbitrary item
 * @node			node to delete
 */
void pantryc_list__delete_node(PantrycList node);

/**
 * Check whether list is empty
 * return			list is empty or not
 */
int pantryc_list__is_empty(PantrycList list);

/**
 * Get value from node
 * @node			node to get value from
 * return			value
 */
void* pantryc_list__get_value(PantrycList node);

/**
 * Free list
 * @list			list to free
 */
void pantryc_list__free(PantrycList list);

#define PANTRYC_LIST__GET_FIRST(d) ((d)->next)
#define PANTRYC_LIST__NEXT(d) ((d)->next)
#define PANTRYC_LIST__GET_LAST(d) ((d)->previous)
#define PANTRYC_LIST__PREVIOUS(d) ((d)->previous)
#define PANTRYC_LIST__IS_NULL(d) (d)

#define PANTRYC_LIST__TRAVERSE(ptr, list) \
  for (ptr = list->next; ptr != list; ptr = ptr->next)
#define PANTRYC_LIST__REVERSE(ptr, list) \
  for (ptr = list->previous; ptr != list; ptr = ptr->previous)

#endif /* PANTRYC_LIST_H_ */
