/*****************************************************************************

Copyright (c) 2006, 2014, Oracle and/or its affiliates. All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA

*****************************************************************************/

/*******************************************************************//**
@file ut/ut0list.cc
A double-linked list

Created 4/26/2006 Osku Salerma
************************************************************************/

#include "ut0list.h"
#ifdef UNIV_NONINL
#include "ut0list.ic"
#endif

/****************************************************************//**
Create a new list.
@return list */
ib_list_t*
ib_list_create(void)
/*=================*/
{
	ib_list_t*	list;

	list = static_cast<ib_list_t*>(ut_malloc_nokey(sizeof(*list)));

	list->first = NULL;
	list->last = NULL;
	list->is_heap_list = FALSE;

	return(list);
}

/****************************************************************//**
Create a new list using the given heap. ib_list_free MUST NOT BE CALLED for
lists created with this function.
@return list */
ib_list_t*
ib_list_create_heap(
/*================*/
	mem_heap_t*	heap)	/*!< in: memory heap to use */
{
	ib_list_t*	list;

	list = static_cast<ib_list_t*>(mem_heap_alloc(heap, sizeof(*list)));

	list->first = NULL;
	list->last = NULL;
	list->is_heap_list = TRUE;

	return(list);
}

/****************************************************************//**
Free a list. */
void
ib_list_free(
/*=========*/
	ib_list_t*	list)	/*!< in: list */
{
	ut_a(!list->is_heap_list);

	/* We don't check that the list is empty because it's entirely valid
	to e.g. have all the nodes allocated from a single heap that is then
	freed after the list itself is freed. */

	ut_free(list);
}

/****************************************************************//**
Add the data to the start of the list.
@return new list node */
ib_list_node_t*
ib_list_add_first(
/*==============*/
	ib_list_t*	list,	/*!< in: list */
	void*		data,	/*!< in: data */
	mem_heap_t*	heap)	/*!< in: memory heap to use */
{
	return(ib_list_add_after(list, ib_list_get_first(list), data, heap));
}

/****************************************************************//**
Add the data to the end of the list.
@return new list node */
ib_list_node_t*
ib_list_add_last(
/*=============*/
	ib_list_t*	list,	/*!< in: list */
	void*		data,	/*!< in: data */
	mem_heap_t*	heap)	/*!< in: memory heap to use */
{
	return(ib_list_add_after(list, ib_list_get_last(list), data, heap));
}

/****************************************************************//**
Add the data after the indicated node.
@return new list node */
ib_list_node_t*
ib_list_add_after(
/*==============*/
	ib_list_t*	list,		/*!< in: list */
	ib_list_node_t*	prev_node,	/*!< in: node preceding new node (can
					be NULL) */
	void*		data,		/*!< in: data */
	mem_heap_t*	heap)		/*!< in: memory heap to use */
{
	ib_list_node_t*	node;

	node = static_cast<ib_list_node_t*>(
		mem_heap_alloc(heap, sizeof(*node)));

	node->data = data;

	if (!list->first) {
		/* Empty list. */

		ut_a(!prev_node);

		node->prev = NULL;
		node->next = NULL;

		list->first = node;
		list->last = node;
	} else if (!prev_node) {
		/* Start of list. */

		node->prev = NULL;
		node->next = list->first;

		list->first->prev = node;

		list->first = node;
	} else {
		/* Middle or end of list. */

		node->prev = prev_node;
		node->next = prev_node->next;

		prev_node->next = node;

		if (node->next) {
			node->next->prev = node;
		} else {
			list->last = node;
		}
	}

	return(node);
}

/****************************************************************//**
Remove the node from the list. */
void
ib_list_remove(
/*===========*/
	ib_list_t*	list,	/*!< in: list */
	ib_list_node_t*	node)	/*!< in: node to remove */
{
	if (node->prev) {
		node->prev->next = node->next;
	} else {
		/* First item in list. */

		ut_ad(list->first == node);

		list->first = node->next;
	}

	if (node->next) {
		node->next->prev = node->prev;
	} else {
		/* Last item in list. */

		ut_ad(list->last == node);

		list->last = node->prev;
	}

	node->prev = node->next = NULL;
}
 