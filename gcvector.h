#ifndef GCVECTOR_H_
#define GCVECTOR_H_

//  Generic c vector.
//  Based on code Evan Teran's c-vector (https://github.com/eteran/c-vector)

// in case C library malloc() needs extra protection,
// allow these defines to be overridden.

// functions for allocation and deallocation need to correspond to each other, fall back to C library functions if not all are overridden
#if !defined(gcvector_clib_free) || !defined(gcvector_clib_malloc) || !defined(gcvector_clib_calloc) || !defined(gcvector_clib_realloc)
#ifdef gcvector_clib_free
#undef gcvector_clib_free
#endif
#ifdef gcvector_clib_malloc
#undef gcvector_clib_malloc
#endif
#ifdef gcvector_clib_calloc
#undef gcvector_clib_calloc
#endif
#ifdef gcvector_clib_realloc
#undef gcvector_clib_realloc
#endif
#include <stdlib.h>
#define gcvector_clib_free free
#define gcvector_clib_malloc malloc
#define gcvector_clib_calloc calloc
#define gcvector_clib_realloc realloc
#endif
// functions independent of memory allocation
#ifndef gcvector_clib_assert
#include <assert.h> // for assert
#define gcvector_clib_assert assert
#endif
#ifndef gcvector_clib_memcpy
#include <string.h> // for memcpy
#define gcvector_clib_memcpy memcpy
#endif
#ifndef gcvector_clib_memmove
#include <string.h> // for memmove
#define gcvector_clib_memmove memmove
#endif

typedef void (*gcvector_elem_destructor_t)(void* elem_ptr);

struct gcvector_t {
    size_t size;
    size_t item_size;
    size_t capacity;
    gcvector_elem_destructor_t elem_destructor;
    void* data;
};

struct gcvector_iterator_t {
    struct gcvector_t* vector;
    void* point;
};

// @brief gcvector - Syntactic sugar for gcvector
typedef struct gcvector_t gcvector;

// @brief gcvector_iterator - The iterator type used for gcvector
typedef struct gcvector_iterator_t gcvector_iterator;

// @brief gcvector_empty - returns non-zero if the vector is empty
// @param vec - the vector
// @return non-zero if empty, zero if non-empty
#define gcvector_empty(vec) \
    ((vec).size == 0)

// @brief gcvector_reserve - Requests that the vector capacity be at least enough
// to contain n elements. If n is greater than the current vector capacity, the
// function causes the container to reallocate its storage increasing its
// capacity to n (or greater).
// @param vec - the vector
// @param n - Minimum capacity for the vector.
// @return void
#define gcvector_reserve(vec, n) \
    do { \
        if ((vec).capacity < (n)) { \
            gcvector_grow((vec), (n)); \
        } \
    } while (0)

// @brief gcvector_init - Initialize a vector.
// @param vec - the vector
// @param capacity - vector capacity to reserve
// @param item_size - size of item
// @param elem_destructor_fn - element destructor function
// @return void
#define gcvector_init(vec, _capacity, _item_size, _elem_destructor) \
    do { \
        (vec) = (gcvector){ \
            .item_size = _item_size, \
            .data = NULL, \
            .capacity = 0, \
            .size = 0, \
            .elem_destructor = (_elem_destructor) \
        }; \
        gcvector_reserve((vec), (_capacity)); \
    } while (0)

// @brief gcvector_erase - removes the element at index i from the vector
// @param vec - the vector
// @param i - index of element to remove
// @return void
#define gcvector_erase(vec, i) \
    do { \
        if ((i) < (vec).size) { \
            if ((vec).elem_destructor) { \
                (vec).elem_destructor(gcvector_at((vec),(i))); \
            } \
            (vec).size = (vec).size - 1; \
            size_t cv_bytes_ = (i) * (vec).item_size; \
            gcvector_clib_memmove( \
                (vec).data + cv_bytes_, \
                (vec).data + cv_bytes_ + (vec).item_size, \
                (vec).item_size * ((vec).size - (i)) ); \
        } \
    } while (0)

// @brief gcvector_clear - erase all of the elements in the vector
// @param vec - the vector
// @return void
#define gcvector_clear(vec) \
    do { \
        if ((vec).elem_destructor) { \
            size_t cv_clear_i__; \
            for (cv_clear_i__ = 0; cv_clear_i__ < (vec).size; ++cv_clear_i__) { \
                (vec).elem_destructor((vec).data + ( (vec).item_size * cv_clear_i__ )); \
            } \
        } \
        (vec).size = 0; \
    } while (0)

// @brief gcvector_begin - returns an iterator to first element of the vector
// @param vec - the vector
// @return a pointer to the first element (or NULL)
#define gcvector_begin(vec) \
    (struct gcvector_iterator_t){.vector = &(vec), .point=(vec).data}

// @brief gcvector_end - returns an iterator to one past the last element of the vector
// @param vec - the vector
// @return a pointer to one past the last element (or NULL)

#define gcvector_end(vec) \
    (struct gcvector_iterator_t){.vector = &(vec), .point=((vec).data + ((vec).size * (vec).item_size))}

// user request to use linear growth algorithm
#ifdef GCVECTOR_LINEAR_GROWTH

// @brief gcvector_compute_next_grow - returns an the computed size in next vector grow
// size is increased by 1
// @param size - current size
// @return size after next vector grow
#define gcvector_compute_next_grow(size) \
    ((size) + 1)

#else

// @brief gcvector_compute_next_grow - returns an the computed size in next vector grow
// size is increased by multiplication of 2
// @param size - current size
// @return size after next vector grow
#define gcvector_compute_next_grow(size) \
    ((size) ? ((size) << 1) : 1)

#endif // GCVECTOR_LINEAR_GROWTH

// @brief gcvector_push_back - adds an element to the end of the vector
// @param vec - the vector
// @param value - the array value to add
// @return void
#define gcvector_push_back(vec, value) \
    do { \
        size_t cv_push_back_cap__ = (vec).capacity; \
        if (cv_push_back_cap__ <= (vec).size) { \
            gcvector_grow((vec), gcvector_compute_next_grow(cv_push_back_cap__)); \
        } \
        void* cv_to = (vec).data + ((vec).size * (vec).item_size); \
        gcvector_clib_memcpy(cv_to, (value), (vec).item_size); \
        (vec).size += 1; \
    } while (0)

// @brief gcvector_push_back_data - adds `array_size` elements to the end of the vector
// @param vec - the vector
// @param array - the array value to add
// @param array_size - number of elements in array
// @return void
#define gcvector_push_back_data(vec, array, array_size) \
    do { \
        gcvector_clib_assert( array ); \
        gcvector_clib_assert( array_size ); \
 \
        if( (vec).capacity < (vec).size + (array_size) ) { \
            gcvector_reserve((vec), (vec).size + (array_size)); \
        } \
        gcvector_clib_memmove( (vec).data + ((vec).size * (vec).item_size), array, (array_size) * (vec).item_size );\
        (vec).size += (array_size); \
    } while (0)

// @brief gcvector_insert - insert element at position pos to the vector
// @param vec - the vector
// @param pos - position in the vector where the new elements are inserted.
// @param val - value to be copied (or moved) to the inserted elements.
// @return void
#define gcvector_insert(vec, pos, value) \
    do { \
        if ((vec).capacity <= (vec).size) { \
            gcvector_grow((vec), gcvector_compute_next_grow((vec).capacity)); \
        } \
        if ((pos) < (vec).size) { \
            size_t cv_bytes_ = (pos) * (vec).item_size; \
            gcvector_clib_memmove( \
                (vec).data + (cv_bytes_) + (vec).item_size, \
                (vec).data + (cv_bytes_), \
                (vec).item_size * (((vec).size) - (pos))); \
        } \
        gcvector_clib_memcpy((vec).data + ((vec).size * (vec).item_size), (value), (vec).item_size); \
        (vec).size += 1; \
    } while (0)

// @brief gcvector_insert_data - insert elements at position pos to the vector
// @param vec - the vector
// @param pos - position in the vector where the new elements are inserted.
// @param data - pointer from be copied data to the inserted elements.
// @param count - count of data elements
// @return void
#define gcvector_insert_data(vec, pos, _data, count) \
    do { \
        if( (vec).capacity < ( (vec).size + (count) ) ) { \
            gcvector_reserve((vec), (vec).size + (count)); \
        } \
        memmove( \
            (vec).data + (( (pos) + (count) ) * (vec).item_size), \
            (vec).data + ((pos) * (vec).item_size), \
            (count) * (vec).item_size \
        ); \
        memmove( \
            (vec).data + ((pos) * (vec).item_size), \
            (_data), \
            (count) * (vec).item_size \
        ); \
        (vec).size += (count); \
    } while (0)

// @brief gcvector_pop_back - removes the last element from the vector
// @param vec - the vector
// @return void
#define gcvector_pop_back(vec) \
    do { \
        if ( (vec).size ) { \
            if ((vec).elem_destructor) { \
                (vec).elem_destructor(gcvector_back(vec)); \
            } \
            (vec).size -= 1; \
        } \
    } while (0)

// @brief gcvector_copy - copy a gcvector to another gcvector
// @param from - the original vector
// @param to - destination to which the function copy to
// @return void
#define gcvector_copy(from, to) \
    do { \
        gcvector_clib_assert((from).item_size == (to).item_size ); \
        gcvector_grow(to, (from).size); \
        (to).size = (from).size; \
        gcvector_clib_memcpy((to).data, (from).data, (from).size * (from).item_size ); \
    } while (0)

// @brief gcvector_swap - exchanges the content of the vector by the content of another vector of the same type
// @param vec - the original vector
// @param other - the other vector to swap content with
// @return void
#define gcvector_swap(vec, other) \
    do { \
        gcvector cv_swap__ = vec; \
        vec             = other; \
        other           = cv_swap__; \
    } while (0)

// @brief gcvector_grow - For internal use, ensures that the vector is at least `count` elements big
// @param vec - the vector
// @param count - the new capacity to set
// @return void
// @internal
#define gcvector_grow(vec, count) \
    do { \
        const size_t cv_grow_sz__ = (count) * (vec).item_size; \
        if ((vec).data) { \
            void *cv_grow_p1__ = (vec).data; \
            void *cv_grow_p2__ = gcvector_clib_realloc(cv_grow_p1__, cv_grow_sz__); \
            gcvector_clib_assert(cv_grow_p2__); \
            (vec).data = (cv_grow_p2__); \
        } else { \
            void *cv_grow_p__ = gcvector_clib_malloc(cv_grow_sz__); \
            gcvector_clib_assert(cv_grow_p__); \
            (vec).data = (cv_grow_p__); \
            (vec).size = 0; \
        } \
        (vec).capacity = (count); \
    } while (0)

// @brief cvector_shrink_to_fit - requests the container to reduce its capacity to fit its size
// @param vec - the vector
// @return void
#define gcvector_shrink_to_fit(vec) \
    do { \
        gcvector_grow(vec, (vec).size); \
    } while (0)

// @brief gcvector_at - returns a reference to the element at position n in the vector.
// @param vec - the vector
// @param n - position of an element in the vector.
// @return the reference of element at the specified position in the vector.
#define gcvector_at(vec, n) \
    ((((int)(n) < 0) || (size_t)(n) >= (vec).size) ? NULL : (unsigned char*)((vec).data + ((n) * (vec).item_size)) )

// @brief gcvector_front - returns a reference to the first element in the vector. Unlike member cvector_begin, which returns an iterator to this same element, this function returns a direct reference.
// @param vec - the vector
// @return a reference to the first element in the vector container.
#define gcvector_front(vec) \
    (((vec).size > 0) ? gcvector_at((vec), 0) : NULL)

// @brief gcvector_back - returns a reference to the last element in the vector. Unlike member cvector_end, which returns an iterator just past this element, this function returns a direct reference.
// @param vec - the vector
// @return a reference to the last element in the vector.
#define gcvector_back(vec) \
    (((vec).size > 0) ? gcvector_at((vec), (vec).size - 1) : NULL)

// @brief gcvector_resize - resizes the container to contain count elements.
// @param vec - the vector
// @param count - new size of the vector
// @param value - the value to initialize new elements with
// @return void
#define gcvector_resize(vec, count, value) \
    do { \
        size_t cv_resize_count__ = (size_t)(count); \
        size_t cv_resize_sz__    = (vec).size; \
        if (cv_resize_count__ > cv_resize_sz__) { \
            gcvector_reserve((vec), cv_resize_count__); \
            (vec).size = cv_resize_count__; \
            do { \
                void* cv_dst = (vec).data + (cv_resize_sz__ * (vec).item_size); \
                gcvector_clib_memcpy(cv_dst, (value), (vec).item_size); \
                cv_resize_sz__ += 1; \
            } while (cv_resize_sz__ < cv_resize_count__); \
        } else { \
            while (cv_resize_count__ < cv_resize_sz__--) { \
                gcvector_pop_back(vec); \
            } \
        } \
    } while (0)

// @brief gcvector_free_data - frees all data memory of the vector (not vector itself)
// @param vec - the vector
// @return void
#define gcvector_deinit(vec) \
    do { \
        gcvector_clear(vec); \
        gcvector_clib_free((vec).data); \
        (vec).data = NULL; \
    } while (0)

// @brief gcvector_capacity - gets the current capacity of the vector
// @param vec - the vector
// @return the capacity as a size_t
#define gcvector_capacity(vec) \
    (vec).capacity

// @brief gcvector_item_size - gets the current element size of the vector
// @param vec - the vector
// @return the item size as a size_t
#define gcvector_item_size(vec) \
    (vec).item_size

// @brief gcvector_size - gets the current size of the vector
// @param vec - the vector
// @return the size as a size_t
#define gcvector_size(vec) \
    (vec).size

// @brief gcvector_iterator_cmp - compare iterators
// @param it1 - iterator 1
// @param operator - compare operator [>,<,==,!=,>=,<=]
// @param it2 - iterator 2
// @return non-zero (true) if expression right
#define gcvector_iterator_cmp(it1, operator , it2) \
    (((it1).vector == (it2).vector) && ((it1).point operator (it2).point))

// @brief gcvector_iterator_less - compare iterators
// @param it1 - iterator 1
// @param it2 - iterator 2
// @return non-zero if it1 less than it2, zero if equal or greater
#define gcvector_iterator_less(it1, it2) \
    (((it1).vector == (it2).vector) && ((it1).point < (it2).point))

// @brief gcvector_iterator_next - move iterator next
// @param it - the iterator
// @return void
#define gcvector_iterator_next(it) \
    (it).point += (it).vector->item_size

// @brief gcvector_iterator_prev - move iterator previous
// @param it - the iterator
// @return void
#define gcvector_iterator_prev(it) \
    (it).point -= (it).vector->item_size

// @brief gcvector_iterator_deref - dereference the iterator
// @param it - the iterator
// @return void*
#define gcvector_iterator_deref(it) \
    ((unsigned char*)(it).point)

//  @brief gcvector_for_each_in - for header to iterate over vector each element's address
//  @param it - iterator of type pointer to vector element
//  @param vec - the vector
//  @return void
#define gcvector_for_each_in(it, vec) \
    for (it = gcvector_begin(vec); gcvector_iterator_less(it,gcvector_end(vec)); gcvector_iterator_next(it))

//  @brief gcvector_for_each - call function func on each element of the vector
//  @param vec - the vector
//  @param func - function to be called on each element that takes each element as argument
//  @return void
#define gcvector_for_each(vec, func) \
    do { \
        gcvector_clib_assert(func); \
        size_t i; \
        for (i = 0; i < ((vec).size * (vec.item_size)); i += (vec).item_size) { \
            func((vec).data + i); \
        } \
    } while (0)


#endif // GCVECTOR_H_