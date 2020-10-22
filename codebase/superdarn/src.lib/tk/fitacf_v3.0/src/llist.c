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

#define WRITE_LOCK(list, err_code) \
    if ( TRUE == (( _llist * )list)->ismt ) \
    { \
        int rc = pthread_rwlock_wrlock(&( ( _llist * ) list )->llist_lock); \
        if (rc != 0)\
        {\
            return err_code;\
        }\
    }

#define READ_LOCK(list, err_code) \
    if ( TRUE == (( _llist * )list)->ismt ) \
    { \
        int rc = pthread_rwlock_rdlock(&( ( _llist * ) list )->llist_lock); \
        if (rc != 0)\
        {\
            return err_code;\
        }\
    }

#define UNLOCK(list, err_code) \
    if ( TRUE == (( _llist * )list)->ismt ) \
    { \
        int rc = pthread_rwlock_unlock(&( ( _llist * ) list )->llist_lock); \
        if (rc != 0)\
        {\
            return err_code;\
        }\
    }

typedef struct __list_node
{
    llist_node node;
    struct __list_node *next;
} _list_node;

typedef struct
{
    unsigned int count;
    comperator comp_func;
    equal equal_func;
    _list_node *head;
    _list_node *tail;
    _list_node *iter;

    /*multi-threading support*/
    unsigned char ismt;
    pthread_rwlockattr_t llist_lock_attr;
    pthread_rwlock_t llist_lock;

} _llist;

/* Helper functions - not to be exported */
static _list_node *listsort ( _list_node *list, _list_node ** updated_tail, comperator cmp, int flags );

/**
 * @brief Create a list
 * @param[in] compare_func a function used to compare elements in the list
 * @param[in] equal_func a function used to check if two elements are equal
 * @param[in] flags used to identify whether we create a thread safe linked-list
 * @return new list if success, NULL on error
 */
llist llist_create ( comperator compare_func, equal equal_func, unsigned flags)
{
    _llist *new_list;
    int rc = 0;
    new_list = malloc ( sizeof ( _llist ) );

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

    new_list->ismt = FALSE;
    if ( flags & MT_SUPPORT_TRUE)
    {
        new_list->ismt = TRUE;
        rc = pthread_rwlockattr_setpshared( &new_list->llist_lock_attr,  PTHREAD_PROCESS_PRIVATE );
        if ( 0 != rc)
        {
            free(new_list);
            return NULL;
        }
        rc = pthread_rwlock_init( &new_list->llist_lock, &new_list->llist_lock_attr );
        if ( 0 != rc)
        {
            pthread_rwlockattr_destroy(&new_list->llist_lock_attr);
            free(new_list);
            return NULL;
        }
    }

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
void llist_destroy ( llist list, bool destroy_nodes, node_func destructor )
{
    _list_node *iterator;
    _list_node *next;

    if ( list == NULL )
    {
        return;
    }
    /* Delete the data contained in the nodes*/
    iterator = ( ( _llist * ) list )->head;

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



    if ( TRUE == (( _llist * )list)->ismt )
    {
        /*release any thread related resource, just try to destroy no use checking return code*/
        pthread_rwlockattr_destroy(&( ( _llist * ) list )->llist_lock_attr);
        pthread_rwlock_destroy( &( ( _llist * ) list )->llist_lock);
    }
    /*release the list*/
    free ( list );

    return;

}

/**
 * @brief return the number of elements in the list
 * @param[in] list the list to operate on
 * @return int  number of elements in the list or -1 if error
 */
int llist_size ( llist list )
{
    unsigned int retval = 0;
    if ( list == NULL )
    {
        return 0;
    }
    READ_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {
        /*read only critical section*/
        retval = ( ( _llist * ) list )->count;
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return retval;
}

/**
 * @brief      Moves the iterator down the list by one.
 *
 * @param[in]  list  The list to operate on.
 *
 * @return     LLIST_SUCCESS if successful.
 */
int llist_go_next(llist list)
{

    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    if(( ( _llist * ) list )->iter == NULL){
        return LLIST_END_OF_LIST;
    }

    if(( ( _llist * ) list )->iter->next == NULL){
        return LLIST_END_OF_LIST;
    }

    ( ( _llist * ) list )->iter = ( ( _llist * ) list )->iter->next;

    return LLIST_SUCCESS;
}

/*int llist_iter_not_at_end(llist list)
{
    if(( ( _llist * ) list )->iter == NULL){
        return LLIST_END_OF_LIST;
    }
    else if(( ( _llist * ) list )->iter->next == NULL){
        return LLIST_END_OF_LIST;
    }
    else{
        return LLIST_SUCCESS;
    }
}
*/
/**
 * @brief      Resets iterator to head of the list
 *
 * @param[in]  list  The list to operate on.
 *
 * @return     LLIST_SUCCESS if successful.
 */
int llist_reset_iter(llist list){

    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    ( ( _llist * ) list )->iter = ( ( _llist * ) list )->head;

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
int llist_get_iter(llist list,void** item){

    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    if(( ( _llist * ) list )->iter == NULL){
        *item = NULL;
        return LLIST_NODE_NOT_FOUND;
    }

    *item = ( ( _llist * ) list )->iter->node;
    return LLIST_SUCCESS;

}

/**
 * @brief Add a node to a list
 * @param[in] list the list to operator upon
 * @param[in] node the node to add
 * @param[in] flags flags
 * @return int LLIST_SUCCESS if success
 */
int llist_add_node ( llist list, llist_node node, int flags )
{
    _list_node *node_wrapper = NULL;

    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {   /*write critical section*/
        node_wrapper = malloc ( sizeof ( _list_node ) );

        if ( node_wrapper == NULL )
        {
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
            return LLIST_ERROR;
        }

        node_wrapper->node = node;

        ( ( _llist * ) list )->count++;

        if ( ( ( _llist * ) list )->head == NULL ) /* Adding the first node, update head and tail to point to that node*/
        {
            node_wrapper->next = NULL;
            ( ( _llist * ) list )->head = ( ( _llist * ) list )->tail = node_wrapper;
        }
        else if ( flags & ADD_NODE_FRONT )
        {
            node_wrapper->next = ( ( _llist * ) list )->head;
            ( ( _llist * ) list )->head = node_wrapper;
        }
        else   /* add node in the rear*/
        {
            node_wrapper->next = NULL;
            ( ( _llist * ) list )->tail->next = node_wrapper;
            ( ( _llist * ) list )->tail = node_wrapper;
        }
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

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
int llist_delete_node ( llist list, llist_node node,
		bool destroy_node, node_func destructor )
{
    _list_node *iterator;
    _list_node *temp;
    equal actual_equal;

    if ( ( list == NULL ) || ( node == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    actual_equal = ( ( _llist * ) list )->equal_func;

	if ( actual_equal == NULL )
    {
        return LLIST_EQUAL_MISSING;
    }

    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {

        iterator = ( ( _llist * ) list )->head;

        if ( NULL == iterator)
        {
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
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


            ( ( _llist * ) list )->head = ( ( _llist * ) list )->head->next;
            if (( ( _llist * ) list )->iter == iterator){
                ( ( _llist * ) list )->iter = ( ( _llist * ) list )->head;
            }

            free ( iterator );
            ( ( _llist * ) list )->count--;

            if ( ( ( _llist * ) list )->count == 0 )
            {
                /*
                 *  if we deleted the last node, we need to reset the tail also
                 *  There's no need to check it somewhere else, because the last node must be the head (and tail)
                 */
                ( ( _llist * ) list )->tail = NULL;
            }
            /*assert ( ( ( _llist * ) list )->count >= 0 );*/
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
            return LLIST_SUCCESS;
        }
        else
        {
            while ( iterator->next != NULL )
            {
                if ( actual_equal ( iterator->next->node, node ) )
                {
                    /* found it */
                    if((( ( _llist * ) list )->iter == iterator->next)){
                        ( ( _llist * ) list )->iter = iterator;
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

                    ( ( _llist * ) list )->count--;
                    /*assert ( ( ( _llist * ) list )->count >= 0 );*/

                    UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
                    return LLIST_SUCCESS;
                }

                iterator = iterator->next;
            }
        }

        if ( iterator->next == NULL )
        {
            UNLOCK( list,LLIST_MULTITHREAD_ISSUE );
            return LLIST_NODE_NOT_FOUND;
        }

    }

    /*assert ( 1 == 2 );*/
    /* this assert always failed. we assume that the function never gets here...*/
    UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
    return LLIST_ERROR;
}

/**
 * @brief operate on each element of the list
 * @param[in] list the list to operator upon
 * @param[in] func the function to perform
 * @return int LLIST_SUCCESS if success
 */
int llist_for_each ( llist list, node_func func )
{
    _list_node *iterator;

    if ( ( list == NULL ) || ( func == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    iterator = ( ( _llist * ) list )->head;

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
int llist_for_each_arg ( llist list, node_func_arg func, void * arg1, void * arg2 )
{
    _list_node *iterator;

    if ( ( list == NULL ) || ( func == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    READ_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {

        iterator = ( ( _llist * ) list )->head;

        while ( iterator != NULL )
        {
            func ( iterator->node , arg1, arg2);
            iterator = iterator->next;
        }
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}

/**
 * @brief Insert a node at a specific location
 * @param[in] list the list to operator upon
 * @param[in] new_node the node to add
 * @param[in] pos_node a position reference node
 * @param[in] flags flags
 * @return int LLIST_SUCCESS if success
 */
int llist_insert_node ( llist list, llist_node new_node, llist_node pos_node,
                        int flags )
{
    _list_node *iterator;
    _list_node *node_wrapper = NULL;

    if ( ( list == NULL ) || ( new_node == NULL ) || ( pos_node == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {
        node_wrapper = malloc ( sizeof ( _list_node ) );

        if ( node_wrapper == NULL )
        {
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
            return LLIST_MALLOC_ERROR;
        }

        node_wrapper->node = new_node;

        ( ( _llist * ) list )->count++;

        iterator = ( ( _llist * ) list )->head;

        if ( iterator->node == pos_node )
        {
            /* it's the first node*/

            if ( flags & ADD_NODE_BEFORE )
            {
                node_wrapper->next = iterator;
                ( ( _llist * ) list )->head = node_wrapper;
            }
            else
            {
                node_wrapper->next = iterator->next;
                iterator->next = node_wrapper;
            }
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
            return LLIST_SUCCESS;
        }

        while ( iterator->next != NULL )
        {
            if ( iterator->next->node == pos_node )
            {
                if ( flags & ADD_NODE_BEFORE )
                {
                    node_wrapper->next = iterator->next;
                    iterator->next = node_wrapper;
                }
                else
                {
                    iterator = iterator->next;
                    /* now we stand on the pos node*/
                    node_wrapper->next = iterator->next;
                    iterator->next = node_wrapper;
                }
                UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
                return LLIST_SUCCESS;
            }

            iterator = iterator->next;
        }

    }
    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    assert ( 1 == 2 );
    /* this assert will always fail. we assume that the function never gets here...*/
    return LLIST_ERROR;

}

/**
 * @brief Finds a node in a list
 * @param[in]  list the list to operator upon
 * @param[in]  data the data to find
 * @param[out] found a pointer for found node.
 *              this pointer can be used only if llist_find_node returned LLIST_SUCCESS
 * @return LLIST_SUCCESS if success
 */
int llist_find_node ( llist list, void *data, llist_node *found)
{
    _list_node *iterator;
    equal actual_equal;
    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    actual_equal = ( ( _llist * ) list )->equal_func;

    if ( actual_equal == NULL )
    {
        return LLIST_EQUAL_MISSING;
    }

    READ_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {
        iterator = ( ( _llist * ) list )->head;

        while ( iterator != NULL )
        {
            if ( actual_equal ( iterator->node, data ) )
            {
                if(found != NULL){
                    *found = iterator->node;
                }
                UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
                return LLIST_SUCCESS;
            }

            iterator = iterator->next;
        }
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    /* Didn't find the node*/
    return LLIST_NODE_NOT_FOUND;

}

/**
 * @brief Returns the head node of the list
 * @param[in] list the list to operate on
 * @return the head node, NULL on error
 */
llist_node llist_get_head ( llist list )
{
    READ_LOCK( list, NULL )

    {
        if ( list != NULL )
        {
            if ( ( ( _llist * ) list )->head ) /* there's at least one node*/
            {
                UNLOCK( list, NULL )
                return ( ( _llist * ) list )->head->node;
            }
        }
    }

    UNLOCK( list, NULL )

    return NULL;
}

/**
 * @brief Returns the tail node of the list
 * @param[in] list the list to operate on
 * @return the tail node, NULL on error
 */
llist_node llist_get_tail ( llist list )
{
    READ_LOCK( list, NULL )

    {
        if ( list != NULL )
        {
            if ( ( ( _llist * ) list )->tail ) /* there's at least one node*/
            {
                UNLOCK(list, NULL)
                return ( ( _llist * ) list )->tail->node;
            }
        }
     }

    UNLOCK(list, NULL)

    return NULL;
}

/**
 * @brief push a node to the head of the list
 * @param[in] list the list to operate on
 * @param[in] node the node to push
 * @return int LLIST_SUCCESS if success
 */
int llist_push ( llist list, llist_node node )
{
    return llist_add_node ( list, node, ADD_NODE_FRONT );
}

/**
 * @brief peek at the head of the list
 * @param[in] list the list to operate on
 * @return llist_node the head node
 */
llist_node llist_peek ( llist list )
{
    return llist_get_head ( list );
}

/**
 * @brief pop the head of the list
 * @param[in] list the list to operate on
 * @return llist_node the head node
 */
llist_node llist_pop ( llist list )
{
    llist_node tempnode = NULL;
    _list_node *tempwrapper;

    WRITE_LOCK( list, NULL )

    {
        if ( ( ( _llist * ) list )->count ) /* There exists at least one node*/
        {
            tempwrapper = ( ( _llist * ) list )->head;
            tempnode = tempwrapper->node;
            ( ( _llist * ) list )->head = ( ( _llist * ) list )->head->next;
            ( ( _llist * ) list )->count--;
            free ( tempwrapper );

            if ( ( ( _llist * ) list )->count == 0 ) /* We've deleted the last node*/
            {
                ( ( _llist * ) list )->tail = NULL;
            }
        }
    }

    UNLOCK(list, NULL)

    return tempnode;
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
    _list_node *end_node;

    if ( ( first == NULL ) || ( second == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    WRITE_LOCK( first, LLIST_MULTITHREAD_ISSUE )
    WRITE_LOCK( second, LLIST_MULTITHREAD_ISSUE )

    {

        end_node = ( ( _llist * ) first )->tail;

        ( ( _llist * ) first )->count += ( ( _llist * ) second )->count;

        if ( end_node != NULL ) /* if the first list is not empty*/
        {
            end_node->next = ( ( _llist * ) second )->head;
        }
        else     /* It's empty */
        {
            ( ( _llist * ) first )->head = ( ( _llist * ) first )->tail =
                    ( ( _llist * ) second )->head;
        }

        /* Delete the nodes from the second list. (not really deletes them, only loses their reference.*/
        ( ( _llist * ) second )->count = 0;
        ( ( _llist * ) second )->head = ( ( _llist * ) second )->tail = NULL;
    }

    UNLOCK( first, LLIST_MULTITHREAD_ISSUE )
    UNLOCK( second, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}

/**
 * @brief Reverse a list
 * @param[in] list the list to operate upon
 * @return int LLIST_SUCCESS if success
 */
int llist_reverse ( llist list )
{
    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }


    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {

        _list_node *iterator = ( ( _llist * ) list )->head;
        _list_node *nextnode = NULL;
        _list_node *temp = NULL;

        /*
         * Swap our Head & Tail pointers
         */
        ( ( _llist * ) list )->head = ( ( _llist * ) list )->tail;
        ( ( _llist * ) list )->tail = iterator;

        /*
         * Swap the internals
         */
        while ( iterator )
        {
            nextnode = iterator->next;
            iterator->next = temp;
            temp = iterator;
            iterator = nextnode;
        }

    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}

/**
 * @brief sort a lists
 * @param[in] list the list to operator upon
 * @param[in] flags
 * @return int LLIST_SUCCESS if success
 */
int llist_sort ( llist list, int flags )
{

    comperator cmp;
    _llist *thelist = ( _llist * ) list;
    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }



    cmp =  thelist->comp_func;

 	if ( cmp == NULL )
    {
        return LLIST_COMPERATOR_MISSING;
    }
    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )
    thelist->head = listsort ( thelist->head, &thelist->tail, cmp, flags);
    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
    /*
     * TODO: update list tail.
     */
    return LLIST_SUCCESS;
}

/*Helper function for sorting*/
static _list_node *listsort ( _list_node *list, _list_node ** updated_tail, comperator cmp , int flags)
{
    _list_node *p, *q, *e, *tail;
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
static int llist_get_min_max(llist list, llist_node * output, bool max)
{
    comperator cmp;
    _list_node *iterator = ( ( _llist * ) list )->head;

	if ( list == NULL )
	{
		return LLIST_NULL_ARGUMENT;
	}

    cmp =  ( ( _llist * ) list )->comp_func;

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
 * @brief get the maximum node in a given list
 * @param[in] list the list to operate upon
 * @param[out] maximum node
 * @return int LLIST_SUCCESS if success
 */
int llist_get_max(llist list, llist_node * max)
{
	return llist_get_min_max(list,max,true);
}

/**
 * @brief get the minimum node in a given list
 * @param[in] list the list to operate upon
 * @param[out] minumum node
 * @return int LLIST_SUCCESS if success
 */
int llist_get_min(llist list, llist_node * min)
{
	return llist_get_min_max(list,min,false);
}

/**
 * @brief check if list is empty
 * @param[in] list the list to operate upon
 * @return bool True if list is empty
 */
bool llist_is_empty(llist list)
{
    return ( ! llist_size ( list ) ) ;
}
