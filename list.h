#ifndef _H_LIST_H
#define _H_LIST_H

struct list_node{
	struct list_node *prev, *next;
};
typedef struct list_node list_node_t;

/* biggest hack ever */
#define get_entry(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

#define list_for_each(list, iter) \
	for(iter = list->next; !list_is_head(list, iter); iter = iter->next)

#define list_for_each_prev(list, iter) \
	for(iter = list->prev; !list_is_head(list, iter); iter = iter->prev)

#define list_for_each_entry(list, iter, ent_name, type, member) \
	for(iter = list->next, ent_name = get_entry(iter, type, member); \
		!list_is_head(list, iter); \
		iter = iter->next, ent_name = get_entry(iter, type, member))

#define list_for_each_entry_prev(list, iter, ent_name, type, member) \
	for(iter = list->prev, ent_name = get_entry(iter, type, member); \
		!list_is_head(list, iter); \
		iter = iter->prev, ent_name = get_entry(iter, type, member))

#define list_for_each_entry_auto(list, iter, ent_name, member) \
	list_for_each_entry(list, iter, ent_name, typeof(*ent_name), member)

#define list_for_each_entry_prev_auto(list, iter, ent_name, member) \
	list_for_each_entry_prev(list, iter, ent_name, typeof(*ent_name), member)

static inline void init_list_node(struct list_node* obj){
	obj->next = obj;
	obj->prev = obj;
}

static inline void list_link(
		struct list_node* prev,
		struct list_node* next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void list_insert(
		struct list_node* new,
		struct list_node* prev,
		struct list_node* next)
{
	prev->next = new;
	new->prev = prev;
	next->prev = new;
	new->next = next;
}

/* DO NOT EVER ADD EXISTING NODE AGAIN 
 * IF YOU WANT TO MOVE IT USE list_move */

static inline void list_add(
		struct list_node* to,
		struct list_node* new)
{
	list_insert(new, to, to->next);
}

static inline void list_add_tail(
		struct list_node* to,
		struct list_node* new)
{
	list_insert(new, to->prev, to);
}

static inline void list_remove(struct list_node* node){
	list_link(node->prev, node->next);
}

static inline void list_move(
		struct list_node* list,
		struct list_node* node)
{
	list_remove(node);
	list_add(list, node);
}

static inline void list_move_tail(
		struct list_node* list,
		struct list_node* node)
{
	list_remove(node);
	list_add_tail(list, node);
}

static inline struct list_node* list_next(struct list_node* node){
	return node->next;
}

static inline int list_is_last(
		struct list_node* list,
		struct list_node* node)
{
	return node->next == list;
}

static inline int list_is_head(
		struct list_node* list,
		struct list_node* node)
{
	return list == node;
}

static inline struct list_node* list_find(
		struct list_node* list,
		void* data,
		int (*predicate)(struct list_node* node, void* data))
{
	struct list_node* iter;
	list_for_each(list, iter){
		if (predicate(iter, data))
			return iter;
	}
	return 0;
}

#endif /*_H_LIST_H */ 
