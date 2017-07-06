/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : list.c
* Author             : AMS - HEA&RF BU
* Version            : V1.0.0
* Date               : 19-July-2012
* Description        : Circular Linked List Implementation.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <hal_types.h>
#include "list.h"

/******************************************************************************
 * Function Definitions 
******************************************************************************/

void list_init_head (tListNode * listHead)
{
	listHead->next = NULL;
	//listHead->prev = NULL;
#ifdef LIST_INVARIANT_CHECK
        listHead->count = 0;
#endif // LIST_INVARIANT_CHECK
}

uint8_t list_is_empty (tListNode * listHead)
{
	return ((listHead->next == NULL)? TRUE:FALSE);
}

#ifdef LIST_INVARIANT_CHECK
// ensure no invalid node in the list
void list_check_invariant(tListNode* listHead) {
        int size=0;
	tListNode * temp = listHead->next;
	while (temp != NULL)
	{
                size++;
		temp = temp->next;		
	}
	
	if (size != listHead->count) {
          for(;;);   
        }
}

uint8_t list_is_in(tListNode * listHead, tListNode * node) {
    
        tListNode * temp = listHead->next;
	while (temp != NULL)
	{
            if (temp == node) {
              return 1;
            }
            temp = temp->next;
	}
	return 0;
}
#endif // LIST_INVARIANT_CHECK

void list_insert_head (tListNode * listHead, tListNode * node)
{
#ifdef LIST_INVARIANT_CHECK
        list_check_invariant(listHead);
        
        // ensure node not in list already
        if (list_is_in(listHead, node)) {
            for(;;);
        }
	
	if (list_watch_addition == node) {
            for (;;);
        }
#endif // LIST_INVARIANT_CHECK
        
	node->next = listHead->next;
	//node->prev = listHead;
	listHead->next = node;
	//(node->next)->prev = node;
#ifdef LIST_INVARIANT_CHECK
        listHead->count++;
        list_check_invariant(listHead);
#endif // LIST_INVARIANT_CHECK
}


void list_insert_tail (tListNode * listHead, tListNode * node)
{
    list_insert_head(listHead, node);
}



void list_remove_head (tListNode * listHead, tListNode ** node )
{
	*node = listHead->next;
#ifdef LIST_INVARIANT_CHECK
        list_check_invariant(listHead);
        if (listHead->count == 0) {
            for(;;);
        }
        if (*node == NULL) {
            for(;;);   
        }
#endif // LIST_INVARIANT_CHECK        
        listHead->next = listHead->next->next;
	//list_remove_node (listHead->next);
	(*node)->next = NULL;
	//(*node)->prev = NULL;
#ifdef LIST_INVARIANT_CHECK
        listHead->count--;
        list_check_invariant(listHead);
#endif // LIST_INVARIANT_CHECK
}

void list_remove_tail (tListNode * listHead, tListNode ** node )
{
    list_insert_head(listHead, node);
}

