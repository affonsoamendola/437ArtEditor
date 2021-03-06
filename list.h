#ifndef LIST_H
#define LIST_H

typedef struct LIST_ELEMENT_
{
	void * content;
	struct LIST_ELEMENT_ * next_element;
	struct LIST_ELEMENT_ * prev_element; 
	int index;
} 
LIST_ELEMENT;

typedef LIST_ELEMENT LIST;

LIST_ELEMENT * allocate_list_element();

LIST * create_list();

int append_list(LIST * target_list, void * content);

void * get_list_at(LIST * source_list, int index);

void * get_list_last(LIST * source_list);

void remove_list_last(LIST * source_list);

void remove_list_at(LIST * source_list, int index);

int len_list(LIST * source_list);

void destroy_list(LIST * target_list);

#endif
