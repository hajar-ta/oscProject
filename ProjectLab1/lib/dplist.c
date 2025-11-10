#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {

    //TODO: add your code here
    if (list == NULL ||*list == NULL){
	return;
	}
	dplist_node_t *current =(*list)->head;
	dplist_node_t *next;
   while (current != NULL){
	next = current-> next;
	if(free_element && (*list)->element_free !=NULL){
	  (*list)->element_free(&current->element);
	}
	free(current);
	current = next;
	}
	free(*list);
	*list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    //TODO: add your code here
    if (list == NULL) return NULL;
    dplist_node_t *ref_at_index, *list_node;
    list_node = malloc(sizeof(dplist_node_t));
    assert(list_node != NULL);
    list_node->element = insert_copy ? list->element_copy(element) : element;
    
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;

}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    //TODO: add your code here
    if (list == NULL) { return NULL;}
    dplist_node_t *to_remove = dpl_get_reference_at_index(list, index);
    if(to_remove == NULL) { return list; }
    
  
    if(free_element && list->element_free != NULL) {
        list->element_free(&to_remove->element);
        }
        
    if(to_remove->prev !=NULL) {
		to_remove -> prev->next = to_remove->next;
		}
    else {
		list->head = to_remove->next; }
    if (to_remove ->next != NULL) {
		to_remove->next->prev = to_remove ->prev;
		}

   	//	free(to_remove->element);
		free(to_remove);
		return list;

}

int dpl_size(dplist_t *list) {

    //TODO: add your code here
    if(list == NULL) {
	return -1;}
    dplist_node_t  *current= list ->head;
    int size = 0;
    while (current != NULL) {
	current = current->next;
	size +=1; }
	return size;

}

void *dpl_get_element_at_index(dplist_t *list, int index) {

    //TODO: add your code here
    if(list == NULL || list->head == NULL) {
	return NULL; }

   dplist_node_t *node = dpl_get_reference_at_index(list, index);
   if(node == NULL) {
	return NULL; }
    return  node->element;
}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    //TODO: add your code here
     if(list == NULL || list->head == NULL) { return -1; } 
     int index = 0; 
     dplist_node_t *current = list->head; 
     while(current != NULL) { 
            if(list->element_compare(current ->element, element)==0) { 
            return index; } 
            index +=1; 
            current = current->next; } 
     return -1;
      
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {

    //TODO: add your code here

    if (list == NULL || list->head == NULL) {
	return NULL;}
    int count = 0 ;
    dplist_node_t *dummy = list ->head;
    
    if (index <=0) {
	return dummy;
        }

     while (count < index && dummy->next != NULL) {
		dummy = dummy-> next;
		count +=1; }
		return dummy;

}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {

    //TODO: add your code here
    if (list == NULL || reference == NULL) return NULL;
    return reference->element;

}

