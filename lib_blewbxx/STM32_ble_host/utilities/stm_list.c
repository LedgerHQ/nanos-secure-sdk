/**
 ******************************************************************************
  * @file    stm_list.c
  * @author  MCD Application Team
  * @brief   TCircular Linked List Implementation.
  ******************************************************************************
  * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */


/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "utilities_common.h"

#include "stm_list.h"

/******************************************************************************
 * Function Definitions 
 ******************************************************************************/
void LST_init_head (tListNode * listHead)
{
  listHead->next = listHead;
  listHead->prev = listHead;
}

uint8_t LST_is_empty (tListNode * listHead)
{
  
  if(listHead->next == listHead)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void LST_insert_head (tListNode * listHead, tListNode * node)
{

  node->next = listHead->next;
  node->prev = listHead;
  listHead->next = node;
  (node->next)->prev = node;

}


void LST_insert_tail (tListNode * listHead, tListNode * node)
{

  node->next = listHead;
  node->prev = listHead->prev;
  listHead->prev = node;
  (node->prev)->next = node;

}


void LST_remove_node (tListNode * node)
{

  (node->prev)->next = node->next;
  (node->next)->prev = node->prev;

}


void LST_remove_head (tListNode * listHead, tListNode ** node )
{

  *node = listHead->next;
  LST_remove_node (listHead->next);

}


void LST_remove_tail (tListNode * listHead, tListNode ** node )
{

  *node = listHead->prev;
  LST_remove_node (listHead->prev);
}


void LST_insert_node_after (tListNode * node, tListNode * ref_node)
{

  node->next = ref_node->next;
  node->prev = ref_node;
  ref_node->next = node;
  (node->next)->prev = node;
}


void LST_insert_node_before (tListNode * node, tListNode * ref_node)
{

  node->next = ref_node;
  node->prev = ref_node->prev;
  ref_node->prev = node;
  (node->prev)->next = node;
}


int LST_get_size (tListNode * listHead)
{
  int size = 0;
  tListNode * temp;

  temp = listHead->next;
  while (temp != listHead)
  {
    size++;
    temp = temp->next;
  }

  return (size);
}

void LST_get_next_node (tListNode * ref_node, tListNode ** node)
{

  *node = ref_node->next;
}


void LST_get_prev_node (tListNode * ref_node, tListNode ** node)
{
  //uint32_t primask_bit;

  *node = ref_node->prev;
}

