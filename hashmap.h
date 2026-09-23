
// GENERIC HASHMAP

#include <stdbool.h>
#include "linked_list.h"


#define HASHMAP_SUCCESS 0              // successful operation
#define HASHMAP_ERROR_NULL -1          // NULL pointer passed in as argument
#define HASHMAP_ERROR_KEY -2           // the key is NULL or invalid
#define HASHMAP_ERROR_MEMORY -3        // memory allocation error
#define HASHMAP_ERROR_NOT_FOUND -4     // the key is not in the table

typedef struct _hashmap* hashmap;


/*
 * Creates a new hashmap with a specific capacity (number of buckets).
 * @param _capacity Maximum nuber of buckets.
 * @return Pointer to the newly created table or NULL if there was an error.
 */
hashmap hashmap_create(int _capacity);

/*
 * Destroys a hashmap and frees its associated memory.
 * @param _map Pointer to the hashmap to destroy, it will be set to NULL.
 */
void hashmap_destroy(hashmap* _map);

/*
 * Inserts or updates a key-value pair;
 * @param _map Map to operate on.
 * @param _key String key.
 * @param _value Value to pair.
 * @return HASHMAP_SUCCESS if successful,
 *         HASHMAP_ERROR_NULL if _map is NULL,
 *         HASHMAP_ERROR_KEY if _key is NULL,
 *         HASHMAP_ERROR_MEMORY if there is a memory error.
 */
int hashmap_set(hashmap _map, char* _key, void* _value);

/*
 * Reads the value associated with a key.
 * @param _map Map to operate on.
 * @param _key Key to find.
 * @param _value_out Pointer to store the value.
 * @return HASHMAP_SUCCESS if found,
 *         HASHMAP_ERROR_NULL if _map or _value_out is NULL,
 *         HASHMAP_ERROR_KEY if _key is NULL,
 *         HASHMAP_ERROR_NOT_FOUND if the key does not exist.
 */
int hashmap_get(hashmap _map, char* _key, void** _value_out);

/*
 * Checks if a specific key is in the table.
 * @param _map Map to operate on.
 * @param _key Key to find.
 * @return true if found, false if not found or if _map is NULL.
 */
bool hashmap_has_key(hashmap _map, char* _key);

/*
 * Returns a list with all the keys.
 * @param _map Map to operate on.
 * @return List with all the keys or NULL if _map is NULL.
 */
list hashmap_get_keys(hashmap _map);

/*
 * Returns the number of elements in the hashmap.
 * @param _map Map to operate on.
 * @return Number of key-value pairs in the hashmap,
 *         or HASHMAP_ERROR_NULL if _map is NULL.
 */
int hashmap_size(hashmap _map);
