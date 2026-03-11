Implementation of generic vector or typeless vector. It contains array of one sized arrays in one memory block.
Code based on Evan Teran's c-vector (eteran/c-vector) and adapted to use in generic vector implementation.

Memory organization:
    For 3 bytes structures it will be like:

    |----------One memory block-----------...  |
    +-----------+-----------+-----------+
	| [1][2][3] | [1][2][3] | [1][2][3] |
	+-----------+-----------+-----------+....


Example:
```c
    gcvector v;
    gcvector_init(v,10,2,NULL);

    char arr1[] = {0,1};

    gcvector_push_back(v, arr1);

    gcvector_iterator it = gcvector_begin(v);
    gcvector_for_each_in(it, v) {
        //do something
    }

    gcvector_for_each(a,swap_bytes);

    gcvector_shrink_to_fit(a);

    gcvector_deinit(v);
```