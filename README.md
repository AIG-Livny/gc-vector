Implementation of generic vector or typeless vector. It contains runtime-calculated-size arrays.
Code based on Evan Teran's c-vector (eteran/c-vector) and adapted to use in this generic vector implementation.

Example:
```c
    gcvector v;
    gcvector_init(v,10,2,NULL);

    char arr1[] = {0,1};

    gcvector_push_back(v, arr1);

    gcvector_iterator it1 = gcvector_begin(v);
    gcvector_for_each_in(it1, v) {
        //do something
    }

    gcvector_iterator it2 = gcvector_end(v);
    if ( gcvector_iterator_cmp(it1, == ,it2) ) {
        //
    }

    gcvector_for_each(a,swap_bytes);

    gcvector_shrink_to_fit(a);

    gcvector_deinit(v);
```