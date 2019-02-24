#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "list.h"

LIST_ELEMENT * allocate_list_element()
{
	LIST_ELEMENT * new_list_element;

	new_list_element = (LIST_ELEMENT *)malloc(sizeof(LIST_ELEMENT));

	return new_list_element;
}

LIST * create_list()
{	
	LIST * list_head;

	list_head = allocate_list_element();

	list_head->prev_element = list_head;
	list_head->next_element = list_head;
	list_head->content = 0;

	list_head->index = -1;

	return list_head;
}

int append_list(LIST * target_list, void * content)
{
	LIST_ELEMENT * last_list_element;
	LIST_ELEMENT * new_list_element;

	new_list_element = allocate_list_element();

	last_list_element = target_list->prev_element;

	last_list_element->next_element = new_list_element;
	target_list->prev_element = new_list_element;

	new_list_element->prev_element = last_list_element;
	new_list_element->next_element = target_list;

	new_list_element->content = content;

	new_list_element->index = new_list_element->prev_element->index + 1;

	return new_list_element->index;
}

void * get_list_at(LIST * source_list, int index)
{
	int i;

	void * content;

	LIST_ELEMENT * current_element;

	current_element = source_list;

	for(i = -1; i < abs(index); i++)
	{
		if(index >= 0)
		{	
			if(current_element->next_element == source_list)
			{
				break;
			}
	
			current_element = current_element->next_element;
			content = current_element->content;
		}
		else
		{
			content = current_element->content;

			if(current_element->prev_element == source_list)
			{
				break;
			}	

			current_element = current_element->prev_element;
		}
	}

	return content;
}


int get_list_index(LIST * source_list, void * object)
{
	int i;

	int index;

	void * content;

	LIST_ELEMENT * current_element;

	current_element = source_list;

	index = i;

	for(i = -1; i < len_list(source_list); i++)
	{
		if(current_element->content == object)
		{
			index = i;
			break;
		}
	}

	return index;
}


void remove_list_at(LIST * source_list, int index)
{
	int i = -1;
	LIST_ELEMENT * current_element;
	LIST_ELEMENT * removed_element;

	current_element = source_list;

	while(current_element->index != index)
	{
		current_element = current_element->next_element;
		i++;
	}

	current_element->prev_element->next_element = current_element->next_element;
	removed_element = current_element;

	do
	{
		current_element = current_element->next_element;
		current_element->index = current_element->prev_element->index + 1;
		i++;
	}
	while(current_element->next_element != source_list);

	free(removed_element);
}

void remove_list(LIST * source_list, void * object)
{
	remove_list_at(source_list, get_list_index(source_list, object));
}

int len_list(LIST * source_list)
{
	int i = -1;

	LIST_ELEMENT * current_element;

	current_element = source_list;

	while(current_element->next_element != source_list)
	{
		current_element = current_element->next_element;
		i++;
	}

	return i+1;
}

void destroy_list(LIST * target_list)
{
	LIST_ELEMENT * current_element;

	current_element = target_list;

	while(current_element->next_element != target_list)
	{
		current_element = current_element->next_element;
		free(current_element->prev_element);
	}

	free(current_element);
}
