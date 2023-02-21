#ifndef __H_SHL_LIST_H
#define __H_SHL_LIST_H

struct shl_list_node{
	struct shl_list_node *prev, *next;
};
typedef struct shl_list_node shl_list_node_t;

#define container_of(ptr, type, name) \
	((type*)((char*)ptr-(unsigned long)&(((type*)0)->name)))

#define shl_get_entry(ptr, type, name) \
	container_of(ptr, type, name)

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

static inline void shl_list_init_head(
		struct shl_list_node* obj);

static inline void shl_list_link(
		struct shl_list_node* prev,
		struct shl_list_node* next);

/* list_insert vs list_insert_safe 
 * 
 * list_insert_safe requires right node init (with shl_list_init_node)
 * 	Can prevent double insertion bug
 * 	
 * list_insert requires nothing. (Default linux-styled double linked list)
 *  Nodes can not be added more than one time.
 *  You can only move them using list_move.
*/
static inline void shl_list_insert(
		struct shl_list_node* list,
		struct shl_list_node* node);

static inline int shl_list_insert_safe(
		struct shl_list_node* list,
		struct shl_list_node* node);

static inline int shl_list_is_last(
		struct shl_list_node* list,
		struct shl_list_node* node);

static inline int shl_list_is_head(
		struct shl_list_node* list,
		struct shl_list_node* node);

static inline int shl_list_is_empty(
		struct shl_list_node* list);

static inline int shl_list_is_node_inserted(
			struct shl_list_node* node);

static inline struct shl_list_node* shl_list_find(
		struct shl_list_node* list,
		const void* data,
		int (*predicate)(
			struct shl_list_node* node,
			const void* data,
			void* user_data
		),
		void* user_data);

static inline void shl_list_init_head(
		struct shl_list_node* obj)
{
	obj->next = obj;
	obj->prev = obj;
}

static inline void shl_list_init_node(
		struct shl_list_node* obj)
{
	obj->next = obj->prev = NULL;
}

static inline void shl_list_link(
		struct shl_list_node* prev,
		struct shl_list_node* next)
{
	prev->next = next;
	next->prev = prev;
}

static inline void __shl_list_add(
		struct shl_list_node* new,
		struct shl_list_node* prev,
		struct shl_list_node* next)
{
	shl_list_link(prev, new);
	shl_list_link(new, next);
}

static inline void shl_list_insert(
		struct shl_list_node* list,
		struct shl_list_node* node)
{
	__shl_list_add(node, list, list->next);
}

static inline int shl_list_insert_safe(
		struct shl_list_node* list,
		struct shl_list_node* node)
{
	if (!shl_list_is_node_inserted(node)){
		__shl_list_add(node, list, list->next);
		return 0;
	}
	return 1;
}

static inline void shl_list_remove(
		struct shl_list_node* node)
{
	shl_list_link(node->prev, node->next);
	shl_list_init_node(node);
}

static inline void shl_list_move(	
		struct shl_list_node* list,
		struct shl_list_node* node)
{
	shl_list_remove(node);
	shl_list_insert(list, node);
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

static inline int shl_list_is_empty(
		struct shl_list_node* list)
{
	return list->next == list;
}

static inline int shl_list_is_node_inserted(
			struct shl_list_node* node)
{
	return node->next && node->prev;
}

static inline struct shl_list_node* shl_list_find(
		struct shl_list_node* list,
		const void* data,
		int (*predicate)(
			struct shl_list_node* node,
			const void* data,
			void* user_data
		),
		void* user_data)
{
	struct shl_list_node* iter;
	shl_list_for_each(list, iter){
		if (predicate(iter, data, user_data))
			return iter;
	}
	return 0;
}

static inline int shl_list_traverse (
		struct shl_list_node* list,
		int (*trav_func)(
			struct shl_list_node* node,
			void* user_data
		),
		void* user_data)
{
	int ret;
	for (struct shl_list_node* iter = list->next, *next = iter->next;
		!shl_list_is_head(list, iter);
		iter = next)
	{
		next = iter->next;
		if ((ret = trav_func(iter, user_data)) != 0)
			return ret;
	}
	return 0;
}

#endif /* __H_SHL_LIST_H */
