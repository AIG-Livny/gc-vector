

#include "gcvector.h"
#include "utest.h/utest.h"
#include <stdarg.h>
#include <stdlib.h>

int destruct_counter;
static void destructor(void *ptr) {
    if (ptr) {
        destruct_counter += 1;
    }
}

UTEST(test, gcvector_back) {
    gcvector v;
    gcvector_init(v,10,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,0};

    gcvector_push_back(v, arr1);
    gcvector_push_back(v, arr2);

    ASSERT_TRUE(gcvector_size(v) == 2);

    char* back = gcvector_back(v);
    ASSERT_TRUE(back[0] == 2);
    ASSERT_TRUE(back[1] == 0);

    gcvector_deinit(v);
}

UTEST(test, cvector_front) {
    gcvector v;
    gcvector_init(v,10,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,0};

    gcvector_push_back(v, arr1);
    gcvector_push_back(v, arr2);

    ASSERT_TRUE(gcvector_size(v) == 2);

    char* front = gcvector_front(v);
    ASSERT_TRUE(front[0] == 0);
    ASSERT_TRUE(front[1] == 1);

    gcvector_deinit(v);
}

UTEST(test, vector_at) {
    gcvector v;
    gcvector_init(v,10,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,3};
    char arr3[] = {4,5};
    char arr4[] = {6,7};
    char arr5[] = {8,9};

    gcvector_push_back(v, arr1);
    gcvector_push_back(v, arr2);
    gcvector_push_back(v, arr3);
    gcvector_push_back(v, arr4);
    gcvector_push_back(v, arr5);

    ASSERT_TRUE(gcvector_size(v) == 5);

    int counter = 0;
    int i = 0;
    for (; i < (int)gcvector_size(v); i++) {
        int o = 0;
        for (; o < gcvector_item_size(v); o++) {
            ASSERT_TRUE(gcvector_at(v, i)[o] == counter++);
        }
    }

    //
    int pos_non_exists = 999;
    ASSERT_TRUE(gcvector_at(v, pos_non_exists) == NULL);

    //
    gcvector_pop_back(v);
    ASSERT_TRUE(gcvector_at(v, 4) == NULL);
    ASSERT_TRUE(gcvector_size(v) == 4);

    gcvector_deinit(v);
}

UTEST(test, vector_empty) {
    gcvector v;
    gcvector_init(v,0,2,NULL);
    ASSERT_TRUE(gcvector_capacity(v) == 0);
    ASSERT_TRUE(gcvector_size(v) == 0);
    ASSERT_TRUE(gcvector_iterator_cmp(gcvector_begin(v) ,==, gcvector_end(v)));
    ASSERT_TRUE(gcvector_empty(v));

    gcvector_deinit(v);
}

UTEST(test, vector_push_pop) {
    gcvector v;
    gcvector_init(v,0,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,3};
    char arr3[] = {4,5};

    // add some elements to the back
    gcvector_push_back(v, arr1);
    gcvector_push_back(v, arr2);
    gcvector_push_back(v, arr3);

    // and remove one too
    gcvector_pop_back(v);

    ASSERT_TRUE(gcvector_capacity(v) == 4);
    ASSERT_TRUE(gcvector_size(v) == 2);

    gcvector_deinit(v);
}

UTEST(test, vector_iterator) {
    gcvector v;
    gcvector_init(v,0,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,3};
    char arr3[] = {4,5};

    // add some elements to the back
    gcvector_push_back(v, arr1);
    gcvector_push_back(v, arr2);
    gcvector_push_back(v, arr3);

    // and remove one too
    gcvector_pop_back(v);

    ASSERT_TRUE(gcvector_capacity(v) == 4);
    ASSERT_TRUE(gcvector_size(v) == 2);

    // iterator over the vector using "iterator" style
    gcvector_iterator it = gcvector_begin(v);

    int i = 0;
    for (it = gcvector_begin(v); gcvector_iterator_cmp(it, != ,gcvector_end(v)); gcvector_iterator_next(it)) {
        switch (i) {
        case 0:
            ASSERT_TRUE(gcvector_iterator_deref(it)[0] == 0);
            break;
        case 1:
            ASSERT_TRUE(gcvector_iterator_deref(it)[0] == 2);
        }
        ++i;
    }
    gcvector_deinit(v);
}

UTEST(test, vector_insert_delete) {
    gcvector a;
    gcvector_init(a,0,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,3};
    char arr3[] = {4,5};
    char arr4[] = {6,7};
    char arr5[] = {8,9};

    gcvector_push_back(a, arr1);
    gcvector_push_back(a, arr2);
    gcvector_push_back(a, arr3);

    gcvector_pop_back(a); // delete arr3
    gcvector_push_back(a, arr4);

    ASSERT_TRUE(((char*)a.data)[0] == 0);
    ASSERT_TRUE(((char*)a.data)[1] == 1);
    ASSERT_TRUE(((char*)a.data)[2] == 2);
    ASSERT_TRUE(((char*)a.data)[3] == 3);
    ASSERT_TRUE(((char*)a.data)[4] == 6);
    ASSERT_TRUE(((char*)a.data)[5] == 7);

    gcvector_erase(a, 1); // delete arr2

    ASSERT_TRUE(((char*)a.data)[0] == 0);
    ASSERT_TRUE(((char*)a.data)[1] == 1);
    ASSERT_TRUE(((char*)a.data)[2] == 6);
    ASSERT_TRUE(((char*)a.data)[3] == 7);

    gcvector_erase(a, 0);
    gcvector_insert(a, 0, arr5);

    ASSERT_TRUE(((char*)a.data)[0] == 6);
    ASSERT_TRUE(((char*)a.data)[1] == 7);
    ASSERT_TRUE(((char*)a.data)[2] == 8);
    ASSERT_TRUE(((char*)a.data)[3] == 9);

    ASSERT_TRUE(gcvector_capacity(a) == 4);
    ASSERT_TRUE(gcvector_size(a) == 2);

    gcvector_deinit(a);
}


UTEST(test, vector_copy) {
    gcvector a;
    gcvector b;

    gcvector_init(a,0,2,NULL);
    gcvector_init(b,0,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,3};
    char arr3[] = {4,5};
    char arr4[] = {6,7};
    char arr5[] = {8,9};

    gcvector_push_back(a, arr1);
    gcvector_push_back(a, arr5);
    gcvector_push_back(a, arr4);
    gcvector_pop_back(a); // delete 4
    gcvector_push_back(a, arr5);
    gcvector_erase(a, 1); // delete 5
    gcvector_erase(a, 0);
    gcvector_insert(a, 0, arr3);

    ASSERT_TRUE(gcvector_capacity(a) == 4);
    ASSERT_TRUE(gcvector_size(a) == 2);

    size_t i;
    gcvector_copy(a, b);
    ASSERT_TRUE(gcvector_size(a) == gcvector_size(b));
    for (i = 0; i < gcvector_size(b) * 2; ++i) {
        ASSERT_TRUE(((char*)a.data)[i] == ((char*)b.data)[i]);
    }

    gcvector_deinit(a);
    gcvector_deinit(b);
}

UTEST(test, vector_swap) {
    gcvector a;
    gcvector b;

    gcvector_init(a,0,2,NULL);
    gcvector_init(b,0,2,NULL);

    char arr1[] = {0,1};
    char arr2[] = {2,3};
    char arr3[] = {4,5};
    char arr4[] = {6,7};
    char arr5[] = {8,9};
    char arr6[] = {10,11};
    char arr7[] = {12,13};

    gcvector_push_back(a, arr1);
    gcvector_push_back(a, arr2);
    gcvector_push_back(a, arr3);

    gcvector_push_back(b, arr4);
    gcvector_push_back(b, arr5);
    gcvector_push_back(b, arr6);
    gcvector_push_back(b, arr7);

    ASSERT_EQ(gcvector_size(a), (size_t)3);
    ASSERT_EQ(gcvector_size(b), (size_t)4);

    gcvector_swap(a, b);

    ASSERT_EQ(gcvector_size(a), (size_t)4);
    ASSERT_EQ(gcvector_size(b), (size_t)3);

    ASSERT_EQ(((char*)a.data)[0], 6);
    ASSERT_EQ(((char*)a.data)[1], 7);
    ASSERT_EQ(((char*)b.data)[0], 0);
    ASSERT_EQ(((char*)b.data)[1], 1);

    gcvector_deinit(a);
    gcvector_deinit(b);
}

UTEST(test, vector_reserve) {
    int i;
    gcvector c;

    gcvector_init(c,0,2,NULL);

    char arr1[] = {0,1};

    gcvector_reserve(c, 100);
    ASSERT_TRUE(gcvector_capacity(c) == 100);
    ASSERT_TRUE(gcvector_size(c) == 0);

    gcvector_push_back(c, arr1);
    ASSERT_TRUE(gcvector_capacity(c) == 100);
    ASSERT_TRUE(gcvector_size(c) == 1);

    gcvector_reserve(c, 10);
    ASSERT_TRUE(gcvector_capacity(c) == 100);

    for (i = 0; i < 100; ++i) {
        gcvector_push_back(c, arr1);
    }

    ASSERT_TRUE(gcvector_capacity(c) == 200);
    gcvector_deinit(c);
}

UTEST(test, vector_free_all) {
    int i;

    destruct_counter = 0;

    gcvector v;
    gcvector_init(v,0,6,destructor);

    for (i = 0; i < 10; ++i) {
        char arr[] = {1,2,3,4,5,6};
        gcvector_push_back(v, arr);
    }

    ASSERT_TRUE(gcvector_size(v) == 10);
    ASSERT_TRUE(gcvector_capacity(v) >= 10);

    gcvector_clear(v);

    ASSERT_TRUE(destruct_counter == 10);

    gcvector_deinit(v);
}

UTEST(test, vector_for_each_int) {
    gcvector v;
    gcvector_init(v,0,2,NULL);

    gcvector_iterator it = gcvector_begin(v);

    int i;
    for (i = 0; i < 10; ++i) {
        char arr[] = {1,2};
        gcvector_push_back(v, arr);
    }

    ASSERT_TRUE(gcvector_size(v) == 10);
    ASSERT_TRUE(gcvector_capacity(v) >= 10);

    gcvector_for_each_in(it, v) {
        ASSERT_TRUE( gcvector_iterator_deref(it)[1] == 2);
    }

    gcvector_deinit(v);
}

UTEST(test, vector_shrink_to_fit) {
    gcvector a;
    gcvector_init(a,0,2,NULL);

    char arr1[] = {1,2};
    char arr2[] = {3,4};
    char arr3[] = {5,6};

    gcvector_push_back(a, arr1);
    gcvector_push_back(a, arr2);
    gcvector_push_back(a, arr3);

    gcvector_reserve(a, 50);
    ASSERT_EQ(gcvector_capacity(a), (size_t)50);

    gcvector_shrink_to_fit(a);
    ASSERT_EQ(gcvector_capacity(a), (size_t)3);

    gcvector_deinit(a);
}

UTEST(test, vector_resize) {
    gcvector a;
    gcvector_init(a,0,2,NULL);

    char arr1[] = {1,2};
    char arr2[] = {3,4};
    char arr3[] = {5,6};
    char arr4[] = {7,8};

    gcvector_push_back(a, arr1);
    gcvector_push_back(a, arr2);
    gcvector_push_back(a, arr3);

    gcvector_resize(a, 50, arr4);
    ASSERT_EQ(gcvector_size(a), (size_t)50);
    ASSERT_EQ(((char*)a.data)[1], 2);
    ASSERT_EQ(((char*)a.data)[30], 7);
    ASSERT_EQ(((char*)a.data)[31], 8);

    gcvector_resize(a, 10, arr4);
    ASSERT_EQ(gcvector_size(a), (size_t)10);
    ASSERT_EQ(((char*)a.data)[2], 3);

    gcvector_resize(a, 0, arr4);
    ASSERT_EQ(gcvector_size(a), (size_t)0);

    gcvector_deinit(a);
}

void swap_bytes(char* arr){
    char tmp = arr[0];
    arr[0] = arr[1];
    arr[1] = tmp;
}

UTEST(test, vector_for_each) {
    gcvector a;
    gcvector_init(a,0,2,NULL);

    char arr1[] = {1,2};
    char arr2[] = {3,4};

    gcvector_push_back(a, arr1);
    gcvector_push_back(a, arr2);

    gcvector_for_each(a,swap_bytes);

    ASSERT_EQ(((char*)a.data)[0], 2);
    ASSERT_EQ(((char*)a.data)[1], 1);
    ASSERT_EQ(((char*)a.data)[2], 4);
    ASSERT_EQ(((char*)a.data)[3], 3);

    gcvector_deinit(a);
}

UTEST(test, erase_range) {
    gcvector a;
    gcvector_init(a,0,2,NULL);

    int i;
    for (i = 0; i < 10; ++i) {
        char arr[] = {i,i};
        gcvector_push_back(a, arr);
    }
    // 0,0  1,1  (2,2  3,3  4,4)  5,5  6,6  7,7  8,8  9,9
    gcvector_erase_range(a, 2,5);

    // 0,0  1,1  5,5  6,6  7,7  8,8  9,9
    ASSERT_EQ(gcvector_size(a), (size_t)7);
    ASSERT_EQ(gcvector_at(a,1)[0], 1);
    ASSERT_EQ(gcvector_at(a,2)[0], 5);
    ASSERT_EQ(gcvector_at(a,4)[0], 7);

    gcvector_deinit(a);
}

UTEST(test, vector_push_back_data) {
    gcvector a;
    gcvector_init(a,0,2,NULL);

    char arr1[] = {1,2,3,4,5,6};
    char arr2[3][2] = {{11,12},{13,14},{15,16}};

    gcvector_push_back_data(a, arr1, 3);

    ASSERT_EQ(gcvector_size(a), 3);

    gcvector_push_back_data(a, arr2, 3);

    ASSERT_EQ(gcvector_size(a), 6);

    ASSERT_EQ(((char*)a.data)[0], 1);
    ASSERT_EQ(((char*)a.data)[1], 2);
    ASSERT_EQ(((char*)a.data)[2], 3);
    ASSERT_EQ(((char*)a.data)[3], 4);
    ASSERT_EQ(((char*)a.data)[4], 5);
    ASSERT_EQ(((char*)a.data)[5], 6);

    ASSERT_EQ(((char*)a.data)[6], 11);
    ASSERT_EQ(((char*)a.data)[7], 12);
    ASSERT_EQ(((char*)a.data)[8], 13);
    ASSERT_EQ(((char*)a.data)[9], 14);
    ASSERT_EQ(((char*)a.data)[10], 15);
    ASSERT_EQ(((char*)a.data)[11], 16);

    gcvector_deinit(a);
}

UTEST(test, vector_insert_data) {
    gcvector a;
    gcvector_init(a,0,2,NULL);

    char arr[] = {99,99};

    gcvector_push_back(a, arr);
    gcvector_push_back(a, arr);

    char array[] = {1,2,3,4,5,6};

    gcvector_insert_data(a,1,array, 3);

    ASSERT_EQ(gcvector_size(a), 5);
    ASSERT_EQ(gcvector_at(a,0)[0], 99);
    ASSERT_EQ(gcvector_at(a,1)[0], 1);
    ASSERT_EQ(gcvector_at(a,2)[0], 3);
    ASSERT_EQ(gcvector_at(a,4)[0], 99);

    gcvector_deinit(a);
}

UTEST_MAIN();
