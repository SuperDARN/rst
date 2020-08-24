/*
 *    Copyright [2013] [Ramon Fried] <ramon.fried at gmail dot com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
NOTICE:

Modified for SuperDARN uses and purposes
Keith Kotyk
2015
 */

#include "rtypes.h"
#include "llist.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define LOG_FUNC_ENTRANCE() printf("%lu: In %s\n", time(NULL), __PRETTY_FUNCTION__);

/* Helper functions - not to be exported */
static list_node *listsort ( list_node *list, list_node ** updated_tail, comperator cmp, int flags );

/**
 * @brief Create a list
 * @param[in] compare_func a function used to compare elements in the list
 * @param[in] equal_func a function used to check if two elements are equal
 * @param[in] flags used to identify whether we create a thread safe linked-list
 * @return new list if success, NULL on error
 */
llist llist_create ( comperator compare_func, equal equal_func, unsigned flags)
{
    list *new_list;
    new_list = malloc ( sizeof ( list ) );

    if ( new_list == NULL )
    {
        return NULL;
    }

/* These can be NULL, I don't care...*/
    new_list->equal_func = equal_func;
    new_list->comp_func = compare_func;

/* Reset the list */
    new_list->count = 0;
    new_list->head = NULL;
    new_list->tail = NULL;
    new_list->iter = new_list->head;

    return new_list;
}

/**
 * @brief Destroys a list
 * @warning Call this function only if the list was created with llist_create
 *          Static initializer created list cannot be destroyed using this function
 * @param[in] list The list to destroy
 * @param[in] destroy_nodes true if the nodes should be destroyed, false if not
 * @param[in] destructor alternative destructor, if the previous param is true,
 *            if NULL is provided standard library c free() will be used
 */
void llist_destroy ( llist data_list, bool destroy_nodes, node_func destructor )
{
    list_node *iterator;
    list_node *next;

    if ( data_list == NULL )
    {
        return;
    }
    /* Delete the data contained in the nodes*/
    iterator = ( ( list * ) data_list )->head;

    while ( iterator != NULL )
    {

        if ( destroy_nodes )
        {

            if ( destructor )
            {
                destructor ( iterator->node );
            }
            else
            {
                free ( iterator->node );
            }
        }

        next = iterator->next;

        free ( iterator ); /* Delete's the container*/

        iterator = next;
    }

    /*release the list*/
    free ( data_list );

    return;

}

/**
 * @brief return the number of elements in the list
 * @param[in] list the list to operate on
 * @return int  number of elements in the list or -1 if error
 */
int llist_size ( llist data_list )
{
    unsigned int retval = 0;
    if ( data_list == NULL )
    {
        return 0;
    }

    {
        /*read only critical section*/
        retval = ((struct list *) data_list )->count;
    }

    return retval;
}

/**
 * @brief      Moves the iterator down the list by one.
 *
 * @param[in]  list  The list to operate on.
 *
 * @return     LLIST_SUCCESS if successful.
 */
int llist_go_next(llist data_list)
{

    if ( data_list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    if(( ( list * ) data_list )->iter == NULL){
        return LLIST_END_OF_LIST;
    }

    if(( ( list * ) data_list )->iter->next == NULL){
        return LLIST_END_OF_LIST;
    }

    ( ( list * ) data_list )->iter = ( ( list * ) data_list )->iter->next;

    return LLIST_SUCCESS;
}


/**
 * @brief      Resets iterator to head of the list
 *
 * @param[in]  list  The list to operate on.
 *
 * @return     LLIST_SUCCESS if successful.
 */
int llist_reset_iter(llist data_list){

    if ( data_list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    ( ( list * ) data_list )->iter = ( ( list * ) data_list )->head;

    return LLIST_SUCCESS;
}

/**
 * @brief      Gets a pointer to the item at the iterator in the output parameter.
 *
 * @param[in]  list  The list to operate on.
 * @param[out] item  A pointer to a pointer to put the list iterator into.
 *
 * @return     LLIST_SUCCESS if successful.
 */
int llist_get_iter(llist data_list,void** item){

    if ( data_list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    if(( ( list * ) data_list )->iter == NULL){
        *item = NULL;
        return LLIST_NODE_NOT_FOUND;
    }

    *item = ( ( list * ) data_list )->iter->node;
    return LLIST_SUCCESS;

}

/**
 * @brief Add a node to a list
 * @param[in] list the list to operator upon
 * @param[in] node the node to add
 * @param[in] flags flags
 * @return int LLIST_SUCCESS if success
 */
int llist_add_node ( llist data_list, llist_node node, int flags )
{
    list_node *node_wrapper = NULL;

    if ( data_list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    node_wrapper = malloc ( sizeof ( list_node ) );

    if ( node_wrapper == NULL )
    {
        return LLIST_ERROR;
    }

    node_wrapper->node = node;

    ( ( list * ) data_list )->count++;

    if ( ( ( list * ) data_list )->head == NULL ) /* Adding the first node, update head and tail to point to that node*/
    {
        node_wrapper->next = NULL;
        ( ( list * ) data_list )->head = ( ( list * ) data_list )->tail = node_wrapper;
    }
    else if ( flags & ADD_NODE_FRONT )
    {
        node_wrapper->next = ( ( list * ) data_list )->head;
        ( ( list * ) data_list )->head = node_wrapper;
    }
    else   /* add node in the rear*/
    {
        node_wrapper->next = NULL;
        ( ( list * ) data_list )->tail->next = node_wrapper;
        ( ( list * ) data_list )->tail = node_wrapper;
    }

    return LLIST_SUCCESS;
}

/**
 * @brief Delete a node from a list
 * @param[in] list the list to operator upon
 * @param[in] node the node to delete
 * @param[in] destroy_node Should we run a destructor
 * @param[in] destructor function, if NULL is provided, free() will be used
 * @return int LLIST_SUCCESS if success
 */
int llist_delete_node ( llist data_list, llist_node node,
		bool destroy_node, node_func destructor )
{
    list_node *iterator;
    list_node *temp;
    equal actual_equal;

    if ( ( data_list == NULL ) || ( node == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    actual_equal = ( ( list * ) data_list )->equal_func;

	if ( actual_equal == NULL )
    {
        return LLIST_EQUAL_MISSING;
    }



    iterator = ( ( list * ) data_list )->head;

    if ( NULL == iterator)
    {
        return LLIST_NODE_NOT_FOUND;
    }


    /* is it the first node ?*/
    if ( actual_equal ( iterator->node, node ) )
    {
        if ( destroy_node )
        {
            if ( destructor )
            {
                destructor ( iterator->node);
            }
            else
            {
                free ( iterator->node );
            }

        }


        ( ( list * ) data_list )->head = ( ( list * ) data_list )->head->next;
        if (( ( list * ) data_list )->iter == iterator){
            ( ( list * ) data_list )->iter = ( ( list * ) data_list )->head;
        }

        free ( iterator );
        ( ( list * ) data_list )->count--;

        if ( ( ( list * ) data_list )->count == 0 )
        {
            /*
             *  if we deleted the last node, we need to reset the tail also
             *  There's no need to check it somewhere else, because the last node must be the head (and tail)
             */
            ( ( list * ) data_list )->tail = NULL;
        }
        /*assert ( ( ( list * ) data_list )->count >= 0 );*/
        return LLIST_SUCCESS;
    }
    else
    {
        while ( iterator->next != NULL )
        {
            if ( actual_equal ( iterator->next->node, node ) )
            {
                /* found it */
                if((( ( list * ) data_list )->iter == iterator->next)){
                    ( ( list * ) data_list )->iter = iterator;
                }
                temp = iterator->next;
                iterator->next = temp->next;
                if ( destroy_node )
                {
                    if ( destructor )
                    {
                        destructor ( temp->node);
                    }
                    else
                    {
                        free ( temp->node );
                    }

                }
                free ( temp );

                ( ( list * ) data_list )->count--;
                return LLIST_SUCCESS;
            }

            iterator = iterator->next;
        }
    }

    if ( iterator->next == NULL )
    {
        return LLIST_NODE_NOT_FOUND;
    }


    return LLIST_ERROR;
}

/**
 * @brief operate on each element of the list
 * @param[in] list the list to operator upon
 * @param[in] func the function to perform
 * @return int LLIST_SUCCESS if success
 */
int llist_for_each ( llist data_list, node_func func )
{
    list_node *iterator;

    if ( ( data_list == NULL ) || ( func == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    iterator = ( ( list * ) data_list )->head;

    while ( iterator != NULL )
    {
        func ( iterator->node );
        iterator = iterator->next;
    }

    return LLIST_SUCCESS;
}

/**
 * @brief operate on each element of the list
 * @param[in] list the list to operator upon
 * @param[in] func the function to perform
 * @param[in] arg passed to func
 * @return int LLIST_SUCCESS if success
 */
/*
 * NOTE: if we want variable arguement list then we need to use "..." which is 
 *       what printf uses. This can be complex and hard to understand from 
 *       a none programmers point of view. To keep things simple I implemented 
 *       a variable structure that contains everything. 
 */
int llist_for_each_arg ( llist data_list, node_func_arg func, void * arg1, void * arg2 )
{
    list_node *iterator;

    if ( ( data_list == NULL ) || ( func == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }


    iterator = ( ( list * ) data_list )->head;

    while ( iterator != NULL )
    {
        func ( iterator->node , arg1, arg2);
        iterator = iterator->next;
    }
    
    return LLIST_SUCCESS;
}

/**
 * @brief Finds a node in a list
 * @param[in]  list the list to operator upon
 * @param[in]  data the data to find
 * @param[out] found a pointer for found node.
 *              this pointer can be used only if llist_find_node returned LLIST_SUCCESS
 * @return LLIST_SUCCESS if success
 */
int llist_find_node ( llist data_list, void *data, llist_node *found)
{
    list_node *iterator;
    equal actual_equal;
    if ( data_list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    actual_equal = ( ( list * ) data_list )->equal_func;

    if ( actual_equal == NULL )
    {
        return LLIST_EQUAL_MISSING;
    }


    iterator = ( ( list * ) data_list )->head;

    while ( iterator != NULL )
    {
        if ( actual_equal ( iterator->node, data ) )
        {
            if(found != NULL){
                *found = iterator->node;
            }
            return LLIST_SUCCESS;
        }

        iterator = iterator->next;
    }

    /* Didn't find the node*/
    return LLIST_NODE_NOT_FOUND;

}

/**
 * @brief Returns the head node of the list
 * @param[in] list the list to operate on
 * @return the head node, NULL on error
 */
llist_node llist_get_head ( llist data_list )
{
    if ( data_list != NULL )
        {
            if ( ( ( list * ) data_list )->head ) /* there's at least one node*/
            {
                return ( ( list * ) data_list )->head->node;
            }
        }

    return NULL;
}

/**
 * @brief concatenate the second list to the first list
 * @param[in] first the list to operate on
 * @param[in] second the list to operate on.
 * @warning The nodes from the second list will be deleted and concatenated to the first list
 *          Remember to call llist_destroy() on  the second list (if it was created by llist_create())
 * @return int LLIST_SUCCESS if success
 */
int llist_concat ( llist first, llist second )
{
    list_node *end_node;

    if ( ( first == NULL ) || ( second == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    end_node = ( ( list * ) first )->tail;

    ( ( list * ) first )->count += ( ( list * ) second )->count;

    if ( end_node != NULL ) /* if the first list is not empty*/
    {
        end_node->next = ( ( list * ) second )->head;
    }
    else     /* It's empty */
    {
        ( ( list * ) first )->head = ( ( list * ) first )->tail =
                ( ( list * ) second )->head;
    }

    /* Delete the nodes from the second list. (not really deletes them, only loses their reference.*/
    ( ( list * ) second )->count = 0;
    ( ( list * ) second )->head = ( ( list * ) second )->tail = NULL;
    return LLIST_SUCCESS;
}

/**
 * @brief sort a lists
 * @param[in] list the list to operator upon
 * @param[in] flags
 * @return int LLIST_SUCCESS if success
 */
int llist_sort ( llist data_list, int flags )
{

    comperator cmp;
    list *thelist = ( list * ) data_list;
    if ( data_list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    cmp =  thelist->comp_func;

 	if ( cmp == NULL )
    {
        return LLIST_COMPERATOR_MISSING;
    }
    thelist->head = listsort ( thelist->head, &thelist->tail, cmp, flags);
    /*
     * TODO: update list tail.
     */
    return LLIST_SUCCESS;
}

/*Helper function for sorting*/
static list_node *listsort ( list_node *list, list_node ** updated_tail, comperator cmp , int flags)
{
    list_node *p, *q, *e, *tail;
    int insize, nmerges, psize, qsize, i;
    int direction = ( flags & SORT_LIST_ASCENDING ) ? 1 : -1;
    insize = 1;

    while ( 1 )
    {
        p = list;
        list = NULL;
        tail = NULL;

        nmerges = 0; /* count number of merges we do in this pass */

        while ( p )
        {
            nmerges++; /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for ( i = 0; i < insize; i++ )
            {
                psize++;
                q = q->next;
                if ( !q )
                {
                    break;
                }
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while ( psize > 0 || ( qsize > 0 && q ) )
            {

                /* decide whether next element of merge comes from p or q */
                if ( psize == 0 )
                {
                    /* p is empty; e must come from q. */
                    e = q;
                    q = q->next;
                    qsize--;
                }
                else
                    if ( qsize == 0 || !q )
                    {
                        /* q is empty; e must come from p. */
                        e = p;
                        p = p->next;
                        psize--;
                    }
                    else
                        if ( ( direction * cmp ( p->node, q->node ) ) <= 0 )
                        {
                            /* First element of p is lower (or same);
                             * e must come from p. */
                            e = p;
                            p = p->next;
                            psize--;
                        }
                        else
                        {
                            /* First element of q is lower; e must come from q. */
                            e = q;
                            q = q->next;
                            qsize--;
                        }

                /* add the next element to the merged list */
                if ( tail )
                {
                    tail->next = e;
                }
                else
                {
                    list = e;
                }

                tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }

        tail->next = NULL;

        /* If we have done only one merge, we're finished. */
        if ( nmerges <= 1 ) /* allow for nmerges==0, the empty list case */
        {
            break;
        }
        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }

    *updated_tail = tail;
    return list;
}

/*helper function to return both min or max*/
static int llist_get_min_max(llist data_list, llist_node * output, bool max)
{
    comperator cmp;
    list_node *iterator = ( ( list * ) data_list )->head;

	if ( data_list == NULL )
	{
		return LLIST_NULL_ARGUMENT;
	}

    cmp =  ( ( list * ) data_list )->comp_func;

 	if ( cmp == NULL )
    {
        return LLIST_COMPERATOR_MISSING;
    }


	*output = iterator->node;
	iterator = iterator->next;
	while (iterator)
	{
		if (max) /* Find maximum*/
		{
			if ( cmp(iterator->node, *output) > 0 )
			{
				*output = iterator->node;
			}
		}
		else /* Find minimum*/
		{
			if ( cmp(iterator->node, *output) < 0 )
			{
				*output = iterator->node;
			}
		}
		iterator = iterator->next;
	}

	return LLIST_SUCCESS;
}

/**
 * @brief get the minimum node in a given list
 * @param[in] list the list to operate upon
 * @param[out] minumum node
 * @return int LLIST_SUCCESS if success
 */
int llist_get_min(llist data_list, llist_node * min)
{
	return llist_get_min_max(data_list,min,false);
}
