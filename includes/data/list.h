/*
 * list.h
 *
 *  Created on: 28.11.2010
 *      Author: sven
 */

#ifndef LIST_H_
#define LIST_H_

#include "../../conf/settings.h"

#ifdef USE_DOUBLE_LINKED_LISTS
#include <avr/io.h>

struct list_item;
typedef struct list_item {
	void* element;
	struct list_item* next;
	struct list_item* prev;
} LIST_ITEM;

typedef struct list {
	uint8_t size;
	LIST_ITEM* head;
	LIST_ITEM* tail;
} LIST;

LIST* list_new();
LIST_ITEM* list_item_for(LIST* list, void* data, uint8_t (*equal)(void*,void*));
void list_add_item(LIST* list, LIST_ITEM* item);
void list_remove_item(LIST* list, LIST_ITEM* item);
void list_remove(LIST* list, void* data, uint8_t (*equal)(void*,void*));
//uint8_t list_contains(LIST* list, void* data);
uint8_t list_contains_data(LIST* list, void* data, uint8_t (*equal)(void*,void*));
uint8_t list_add(LIST* list, void* data);

#endif
#endif /* LIST_H_ */
