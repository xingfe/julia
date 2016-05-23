#include "map.h"
#include "connection.h"
#include "request.h"
#include "string.h"

#include <stddef.h>



static hash_t string_hash(string_t* str);

map_slot_t* map_get(map_t* map, map_key_t key)
{
    hash_t hash = string_hash(&key);
    map_slot_t* slot = &map->data[hash % map->size];
    if (slot->key.begin == NULL)
        return NULL;
    while (slot && !string_eq(&slot->key, &key)) {
        slot = slot->next;
    }
    return slot;
}

void map_put(map_t* map, string_t key, map_val_t val)
{
    hash_t hash = string_hash(&key);
    map_slot_t* slot = &map->data[hash % map->size];
    if (slot->key.begin == NULL) {
        slot->key = key;
        slot->val = val;
        return;
    }
    map_slot_t* new_slot = map->cur++;
    new_slot->key = key;
    new_slot->val = val;
    new_slot->next = slot->next;
    slot->next = new_slot;
}

static hash_t string_hash(string_t* str)
{
    hash_t hash = 0;
    for (char* p = str->begin; p < str->end; p++)
        hash = (hash * 31) + *p;
    return hash;
}