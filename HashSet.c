#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashSet.h"

// Private functions:

/**
 * A function for fixed modulo.
 * Returns mathematical modulo also for negative values.
 * We replace the '%' modulo of C Language in this operator.
 * @param value A numerical value to perform fixed modulo operation.
 * @param capacity The capacity of the Hashset.
 * @return Returns mathematical modulo for negative and positive values.
 */
size_t fixed_mod(size_t value, unsigned int capacity) {
    size_t modulo = (value & (capacity - 1));
    return modulo;
}

/**
 * This function checks if the given node is free or not.
 * Node is free is equivalent to node->data = NULL.
 * @param node A pointer to a node to check if it's free.
 * @return Returns -1 if the node pointer is null. Returns 0 if the node
 * is not free e.g there is another value in the node.
 * Returns 1 if the node is free of data.
 */
int is_node_free(Node *node) {
    if (!node) {
        return -1;
    } else if (node->data == NULL) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * This function is used to rehash the values to the new NodesList
 * after resizing the hashset. This basically the step we insert the old data
 * we kept from the old NodesList back to the resized hashset NodesList with
 * the new indexes.
 * @param hash_set The Hashset.
 * @param tmp_values_list Dynamically allocated Value type array.
 * @param old_size The old size of the hashset before we reset it to 0.
 */
void rehash(HashSet *hash_set, Value const *tmp_values_list, size_t old_size) {
    for (size_t i = 0; i < old_size; ++i) {
        if (tmp_values_list[i]) {
            hash_set_insert(hash_set, tmp_values_list[i]);
        }
    }
}

/**
 * Allocates all nodes to the NodesList at once.
 * @param hash_set The Hashset.
 * @param node_list Dynamically allocated array of nodes.
 */
void node_list_alloc(HashSet *hash_set, Node **node_list) {
    for (size_t i = 0; i < hash_set->capacity; ++i) {
        node_list[i] = node_alloc(hash_set->value_cpy,
                                  hash_set->value_cmp,
                                  hash_set->value_free);
    }
}

/**
 * Frees the dynamically allocated array of type Value values.
 * @param hash_set The Hashset.
 * @param tmp_values_list Dynamically allocated array of Value type values.
 */
void free_copied_values(const HashSet *hash_set, Value *tmp_values_list,
                        size_t old_size) {
    for (size_t i = 0; i < old_size; ++i) {
        if (tmp_values_list[i]) {
            hash_set->value_free(&(tmp_values_list[i]));
            free(tmp_values_list[i]);
            tmp_values_list[i] = NULL;
        }
    }
    free(tmp_values_list);
    tmp_values_list = NULL;
}

/**
 * Copies the values from the nodes in the NodesList to a dynamically allocated
 * array of type Value.
 * @param hashset The hashset.
 * @return Returns a dynamically allocated list of values that were extracted
 * from the nodes the were on the NodesList.
 */
Value *copy_values(HashSet *hash_set) {
    Value *tmp_values = malloc((hash_set->size) * sizeof(Value));
    size_t last_index = 0;
    for (size_t i = 0; i < hash_set->capacity; ++i) {
        if (hash_set->NodesList[i]->data != NULL) {
            Value new_value = hash_set->value_cpy(
                    hash_set->NodesList[i]->data);
            tmp_values[last_index] = new_value;
            last_index++;
        }
        node_free(&(hash_set->NodesList[i]));
    }
    free(hash_set->NodesList);
    hash_set->NodesList = NULL;
    return tmp_values;
}

/**
 * This function resizes the hashset's NodesList
 * according to a new given capacity.
 * Works for both shrinking the hashset and enlarging the hashset.
 * @param hash_set The hashset.
 * @param new_capacity The new capacity of the Hashset.
 * @return Returns a resized hashset with the fixed number of nodes on the
 * NodesList according to the new capacity and hashes the values back to
 * the hashset with fixed indexes after the resize has occurred.
 */
HashSet *resize_hashset(HashSet *hash_set, unsigned int new_capacity) {
    Value *tmp_values_list = copy_values(hash_set);
    Node **new_node_list = malloc(new_capacity * sizeof(Node *));
    hash_set->capacity = new_capacity;
    node_list_alloc(hash_set, new_node_list);
    size_t old_size = hash_set->size;
    hash_set->size = 0;
    hash_set->NodesList = new_node_list;
    rehash(hash_set, tmp_values_list, old_size);
    free_copied_values(hash_set, tmp_values_list, old_size);
    return hash_set;
}

// API supported functions:

/**
 * Allocates dynamically new hash set.
 * @param hash_func a function which "hashes" keys.
 * @param value_cpy a function which copies Values.
 * @param value_cmp a function which compares Values.
 * @param value_free a function which frees Values.
 * @return pointer to dynamically allocated HashSet.
 * @if_fail return NULL or if one of the function pointers is null.
 */
HashSet *hash_set_alloc(
        HashFunc hash_func, HashSetValueCpy value_cpy,
        HashSetValueCmp value_cmp, HashSetValueFree value_free) {
    if (!hash_func || !value_cmp || !value_cpy || !value_free) {
        return NULL;
    }
    HashSet *hash_set = malloc(sizeof(HashSet));
    hash_set->value_cpy = value_cpy;
    hash_set->value_cmp = value_cmp;
    hash_set->value_free = value_free;
    hash_set->hash_func = hash_func;
    hash_set->size = 0;
    hash_set->capacity = HASH_SET_INITIAL_CAP;
    Node **node_list = malloc(
            sizeof(Node *) * HASH_SET_INITIAL_CAP);
    if (!(node_list)) {
        return NULL;
    }
    node_list_alloc(hash_set, node_list);
    hash_set->NodesList = node_list;
    return hash_set;
}

/**
 * Frees the hash set and all its elements.
 * @param p_hash_set pointer to pointer to a hash_set.
 */
void hash_set_free(HashSet **p_hash_set) {
    if (!p_hash_set || !(*p_hash_set) || !(*p_hash_set)->NodesList) {
        return;
    }
    for (size_t i = 0; i < (*p_hash_set)->capacity; ++i) {
        if ((*p_hash_set)->NodesList[i]) {
            node_free(&(*p_hash_set)->NodesList[i]); //free nodes one by one.
        }
    }
    free((*p_hash_set)->NodesList);
    (*p_hash_set)->NodesList = NULL;
    free(*p_hash_set);
    *p_hash_set = NULL;
}

/**
 * Inserts a new Value to the hash set.
 * The function inserts *new*, *copied*, *dynamically allocated* Value,
 * @param hash_set the hash set where we want to insert the new element
 * @param value a Value we would like to add to the hashset
 * @return returns 1 for successful insertion, 0 otherwise.
 */
int hash_set_insert(HashSet *hash_set, Value value) {
    if (!hash_set || !value || !hash_set->value_cpy || !hash_set->value_free ||
        !hash_set->value_cmp || !hash_set->hash_func) {
        return 0;
    }
    Value new_value = hash_set->value_cpy(value);
    if (hash_set_contains_value(hash_set, value) == 1) {
        hash_set->value_free(&new_value);
        return 0;
    }
    for (size_t i = 0; i < hash_set->capacity; ++i) {
        unsigned int idx = fixed_mod(
                hash_set->hash_func(value) + (i + (i * i)) / 2,
                hash_set->capacity);
        int result = is_node_free(hash_set->NodesList[idx]);
        if (result == 1) {
            int data_added = set_node_data(hash_set->NodesList[idx],
                                           new_value);
            if (data_added) {
                if (i == 0) {
                    hash_set->NodesList[idx]->hashCount++;
                }
                hash_set->size++;
                double load_factor = hash_set_get_load_factor(hash_set);
                if (load_factor >= HASH_SET_MAX_LOAD_FACTOR) {
                    unsigned int new_capacity =
                            hash_set->capacity * HASH_SET_GROWTH_FACTOR;
                    resize_hashset(hash_set, new_capacity);
                }
                hash_set->value_free(&new_value);
                return 1;
            }
        } else if (result == 0) {
            if (i == 0) {
                hash_set->NodesList[idx]->hashCount++;
            }
            continue;
        }
    }
    hash_set->value_free(&new_value);
    return 0;
}


/**
 * The function checks if the given value exists in the hash set.
 * @param hash_set a hash set.
 * @param value the value to be checked.
 * @return 1 if the value is in the hash set, 0 otherwise.
 */
int hash_set_contains_value(HashSet *hash_set, Value value) {
    if (!hash_set || !value) {
        return 0;
    }
    unsigned int idx2 = fixed_mod(hash_set->hash_func(value),
                                  hash_set->capacity);
    if (hash_set->NodesList[idx2]->hashCount == 0) {
        return 0;
    }
    for (size_t i = 0; i < hash_set->capacity; ++i) {
        unsigned int idx = fixed_mod(
                hash_set->hash_func(value) + (i + (i * i)) / 2,
                hash_set->capacity);
        int result = check_node(hash_set->NodesList[idx], value);
        if (result == 1) {
            return 1;
        }
    }
    return 0;
}

/**
 * The function erases a Value.
 * @param hash_set a hash set.
 * @param value the value to be erased from the hashtable
 * @return 1 if the erasing was done successfully, 0 otherwise.
 */
int hash_set_erase(HashSet *hash_set, Value value) {
    if (!hash_set || !value || !hash_set->value_cpy || !hash_set->value_cmp ||
        !hash_set->value_free || !hash_set->hash_func) {
        return 0;
    }
    if (!hash_set_contains_value(hash_set, value)) {
        return 0;
    }
    unsigned int first_idx = fixed_mod(hash_set->hash_func(value),
                                       hash_set->capacity);
    for (size_t i = 0; i < hash_set->capacity; ++i) {
        unsigned int idx = fixed_mod(hash_set->hash_func(value) +
                                     (i + (i * i)) / 2, hash_set->capacity);
        int result = check_node(hash_set->NodesList[idx], value);
        if (result == 1) {
            clear_node(hash_set->NodesList[idx]);
            hash_set->size--;
            hash_set->NodesList[first_idx]->hashCount--;
            double load_factor = hash_set_get_load_factor(hash_set);
            if (load_factor <= HASH_SET_MIN_LOAD_FACTOR) {
                unsigned int new_capacity =
                        hash_set->capacity / HASH_SET_GROWTH_FACTOR;
                resize_hashset(hash_set, new_capacity);
            }
            return 1;
        } else if (result == 0) {
            continue;
        }
    }
    return 0;
}

/**
 * This function returns the load factor of the hash set.
 * @param hash_set a hash set.
 * @return the hash set's load factor, -1 if the function failed.
 */
double hash_set_get_load_factor(HashSet *hash_set) {
    if (!hash_set) {
        return -1;
    }
    double load_factor = (double) hash_set->size / hash_set->capacity;
    return load_factor;
}

/**
 * This function deletes all the values in the hash set.
 * @param hash_set a hash set to be cleared.
 */
void hash_set_clear(HashSet *hash_set) {
    if (!hash_set || !hash_set->NodesList || !hash_set->value_free) {
        return;
    }
    for (size_t i = 0; i < hash_set->capacity; ++i) {
        clear_node(hash_set->NodesList[i]);
        hash_set->NodesList[i]->hashCount = 0;
    }
    hash_set->size = 0; // reset size after all values were deleted.
}

/**
 * This function gets the element in the hashset hashed to the indexed value
 * @param hash_set the hashset
 * @param value the index of the node in the hashtable that we would like to
 * have its value
 * @return the element in the hashset hashed to the indexed value
 * NULL in case of an error or an empty node or an unexpected value
 */
Value hash_set_at(HashSet *hash_set, int value)
{
    if (!hash_set || !value || value < 0 ||
        (unsigned int) value >= hash_set->capacity ||
        !hash_set->NodesList[value])
    {
        return NULL;
    }
    Value node_data = hash_set->NodesList[value]->data;
    if (!node_data) {
        return NULL;
    }
    return node_data;
}