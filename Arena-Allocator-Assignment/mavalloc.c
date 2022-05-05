// The MIT License (MIT)
//
// Copyright (c) 2022 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

enum TYPE
{
    FREE = 0,
    USED
};

struct Node {
  size_t size;
  enum TYPE type;
  void * arena;
  struct Node * next;
  struct Node * prev;
};

//Global variable I'm using throught the code for my linked list
struct Node *alloc_list;
struct Node *previous_node;
struct Node *temp;


void * arena;

enum ALGORITHM allocation_algorithm = FIRST_FIT;

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  arena = malloc( ALIGN4( size ) );

  allocation_algorithm = algorithm;

  alloc_list = ( struct Node * )malloc( sizeof( struct Node ));

  alloc_list -> arena = arena;
  alloc_list -> size  = ALIGN4(size);
  alloc_list -> type  = FREE;
  alloc_list -> next  = NULL;
  alloc_list -> prev  = NULL;

  previous_node  = alloc_list;

  return 0;
}

void mavalloc_destroy( )
{
  free( arena );

  // iterate over the linked list and free the nodes
  while( alloc_list != NULL)
  {
    temp = alloc_list;
    alloc_list = alloc_list -> next;
    free(temp);
  }
  return;
}

void * mavalloc_alloc( size_t size )
{
  struct Node * node;

  if( allocation_algorithm != NEXT_FIT )
  {
    node = alloc_list;
  }
  else if ( allocation_algorithm == NEXT_FIT )
  {
    node = previous_node;
  }
  else
  {
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  size_t aligned_size = ALIGN4( size );

//Code to run FIRST_FIT algorithm.
  if( allocation_algorithm == FIRST_FIT )
  {
    while( node )
    {
      //Making sure the new block would actually fit
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        //calculating left space node
        int leftover_size = 0;

        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;

          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }

  // Implement Next Fit
  if( allocation_algorithm == NEXT_FIT )
  {
    //Having this on NEXT_FIT because it has to be able to cycle through
    if(previous_node == NULL)
    {
      previous_node = alloc_list;
    }
    node = previous_node;
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;

        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;

          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
      if(node == previous_node)
      {
        break;
      }
      if(node == NULL)
      {
        node = alloc_list;
      }
    }
  }
  // Implement Worst Fit
  //Looking for the most space to put the new block in.
  if( allocation_algorithm == WORST_FIT )
  {
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;

        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;

          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }
  // Implement Best Fit
  //Looking for the best place to put the new block in
  //Algorithm isn't working yet but I know you have to find the smallest
  //block in where the new one will still fit.
  if( allocation_algorithm == BEST_FIT )
  {
    while( node )
    {
      if( node -> size >= aligned_size  && node -> type == FREE )
      {
        int leftover_size = 0;

        node -> type  = USED;
        leftover_size = node -> size - aligned_size;
        node -> size =  aligned_size;

        if( leftover_size > 0 )
        {
          struct Node * previous_next = node -> next;
          struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

          leftover_node -> arena = node -> arena + size;
          leftover_node -> type  = FREE;
          leftover_node -> size  = leftover_size;
          leftover_node -> next  = previous_next;

          node -> next = leftover_node;
        }
        previous_node = node;
        return ( void * ) node -> arena;
      }
      node = node -> next;
    }
  }

  return NULL;
}

//Freeing variables called in main.
//free the block back to my preallocated memory arena
void mavalloc_free( void * ptr )
{
  struct Node * free = alloc_list;
  while( free )
  {
    if( free -> arena == ptr )
    {
      free -> type = FREE;
      break;
    }
    if(free -> type == FREE && free -> next -> type == FREE)
    {
      free -> next = free -> next -> next; 
    }
    free = free -> next;
  }
  return;
}

//Calculating the size of the linked list. 
int mavalloc_size( )
{
  int number_of_nodes = 0;
  struct Node * ptr = alloc_list;

  while( ptr )
  {
    number_of_nodes ++;
    ptr = ptr -> next;
  }

  return number_of_nodes;
}
