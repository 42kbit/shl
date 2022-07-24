#ifndef _H_LIST_H
#define _H_LIST_H

struct shl_list_node{
	struct shl_list_node *prev, *next;
};
typedef struct shl_list_node shl_list_node_t;

/* biggest hack ever */
#define shl_get_entry(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

#define shl_list_for_each(list, iter) \
	for(iter = list->next; !shl_list_is_head(list, iter); iter = iter->next)

#define shl_list_for_each_prev(list, iter) \
	for(iter = list->prev; !shl_list_is_head(list, iter); iter = iter->prev)

#define shl_list_for_each_entry(list, iter, ent_name, type, member) \
	for(iter = list->next, ent_name = shl_get_entry(iter, type, member); \
		!shl_list_is_head(list, iter); \
		iter = iter->next, ent_name = shl_get_entry(iter, type, member))

#define shl_list_for_each_entry_prev(list, iter, ent_name, type, member) \
	for(iter = list->prev, ent_name = shl_get_entry(iter, type, member); \
		!shl_list_is_head(list, iter); \
		iter = iter->prev, ent_name = shl_get_entry(iter, type, member))

#define shl_list_for_each_entry_auto(list, iter, ent_name, member) \
	shl_list_for_each_entry(list, iter, ent_name, typeof(*ent_name), member)

#define shl_list_for_each_entry_prev_auto(list, iter, ent_name, member) \
	shl_list_for_each_entry_prev(list, iter, ent_name, typeof(*ent_name), member)

static inline void shl_list_init_node(struct shl_list_node* obj){
	obj->next = obj;
	obj->prev = obj;
}

static inline void shl_list_link(
		struct shl_list_node* prev,
		struct shl_list_node* next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void shl_list_insert(
		struct shl_list_node* new,
		struct shl_list_node* prev,
		struct shl_list_node* next)
{
	prev->next = new;
	new->prev = prev;
	next->prev = new;
	new->next = next;
}

/* DO NOT EVER ADD EXISTING NODE AGAIN 
 * IF YOU WANT TO MOVE IT USE list_move */

static inline void shl_list_add(
		struct shl_list_node* to,
		struct shl_list_node* new)
{
	shl_list_insert(new, to, to->next);
}

static inline void shl_list_add_tail(
		struct shl_list_node* to,
		struct shl_list_node* new)
{
	shl_list_insert(new, to->prev, to);
}

static inline void shl_list_remove(struct shl_list_node* node){
	shl_list_link(node->prev, node->next);
}

static inline void shl_list_move(
		struct shl_list_node* list,
		struct shl_list_node* node)
{
	shl_list_remove(node);
	shl_list_add(list, node);
}

static inline void shl_list_move_tail(
		struct shl_list_node* list,
		struct shl_list_node* node)
{
	shl_list_remove(node);
	shl_list_add_tail(list, node);
}

static inline struct shl_list_node* shl_list_next(struct shl_list_node* node){
	return node->next;
}

static inline int shl_list_is_last(
		struct shl_list_node* list,
		struct shl_list_node* node)
{
	return node->next == list;
}

static inline int shl_list_is_head(
		struct shl_list_node* list,
		struct shl_list_node* node)
{
	return list == node;
}

static inline struct shl_list_node* shl_list_find(
		struct shl_list_node* list,
		void* data,
		int (*predicate)(struct shl_list_node* node, void* data))
{
	struct shl_list_node* iter;
	shl_list_for_each(list, iter){
		if (predicate(iter, data))
			return iter;
	}
	return 0;
}

#endif /*_H_LIST_H */ 
