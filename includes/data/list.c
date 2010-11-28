/*
 * list.c
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#include <stdlib.h>
#include "list.h"

LIST* list_new() {
	LIST* list = (LIST*) calloc(1, sizeof(LIST));
	return list;
}

void list_add_item(LIST* list, LIST_ITEM* item) {
	if(list->tail) {
		list->tail->next = item;
		item->prev = list->tail;
		list->tail = item;
	} else {
		list->tail = item;
		list->head = item;
	}
	list->size++;
}

void list_remove(LIST* list, void* data, uint8_t (*equal)(void*,void*)) {
	list_remove_item(list, list_item_for(list, data, equal));
}

void list_remove_item(LIST* list, LIST_ITEM* item) {
	if(item->prev) {
		item->prev->next = item->next;
	}
	if(item->next) {
		item->next->prev = item->prev;
	}
	if(item == list->tail) {
		list->tail = item->prev;
	}
	if(item == list->head) {
		list->head = item->next;
	}
	item->next = NULL;
	item->prev = NULL;
}

uint8_t list_add(LIST* list, void* data) {
	LIST_ITEM* item = (LIST_ITEM*) calloc(1, sizeof(LIST_ITEM));
	if(!item) {
		return 0;
	}
	list_add_item(list, item);
	return 1;
}

/*uint8_t list_contains_helper(LIST_ITEM* item, void* data) {
	if(item) {
		if(item->element == data) {
			return 1;
		}
		return list_contains_helper(item->next, data);
	} else {
		return 0;
	}
}

uint8_t list_contains(LIST* list, void* data) {
	return list_contains_helper(list->head, data);
}*/

uint8_t list_contains_data(LIST* list, void* data, uint8_t (*equal)(void*,void*)) {
	return (list_item_for(list, data, equal) != NULL);
}

LIST_ITEM* list_item_for_helper(LIST_ITEM* item, void* data, uint8_t (*equal)(void*,void*)) {
	if(item) {
		if((*equal)(item->element, data)) {
			return item;
		}
		return list_item_for_helper(item->next, data, equal);
	} else {
		return NULL;
	}
}

LIST_ITEM* list_item_for(LIST* list, void* data, uint8_t (*equal)(void*,void*)) {
	return list_item_for_helper(list->head, data, equal);
}
