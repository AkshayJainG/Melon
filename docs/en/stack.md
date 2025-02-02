## Stack



### Header file

```c
#include "mln_stack.h"
```



### Functions/Macros



#### 	mln_stack_init

```c
mln_stack_t *mln_stack_init(struct mln_stack_attr *attr);

struct mln_stack_attr {
    stack_free               free_handler;//stack node data release function
    stack_copy               copy_handler;//Stack node data copy function
    mln_u32_t                cache:1;//Whether to cache the unusedstack node structure
};

typedef void (*stack_free)(void *);
typedef void *(*stack_copy)(void *, void *);
```

Description:

Initialize the stack structure.

`free_handler`: It is the release function of the data on the stack. Since the data on the stack may be a custom data structure, if you need to release it, you can set it, otherwise set it to `NULL`.

`copy_handler`: Copy stack node data.

`cache`: Whether to cache **all** stack node structure memory to improve efficiency (non-user data).

The parameter of `stack_free` is the data structure pointer of user-defined data.

The parameters of `stack_copy` are: the data structure pointer of the copied stack node data and the second parameter of the `mln_stack_dup` function (ie user-defined data), this callback function is only called in the `mln_stack_dup` function.

Return value: return stack pointer on success, otherwise `NULL`



#### mln_stack_destroy

```c
void mln_stack_destroy(mln_stack_t *st);
```

Description: Destroy the stack structure and release the data resources in the stack node.

Return value: none



#### mln_stack_push

```c
int mln_stack_push(mln_stack_t *st, void *data);
```

Description: Push data `data` onto stack `st`.

Return value: return `0` on success, otherwise return `-1`



#### mln_stack_pop

```c
void *mln_stack_pop(mln_stack_t *st);
```

Description: Pop the top element data of stack `st`.

Return value: `NULL` if there is no element in the stack, otherwise it is the data pointer in the stack node



#### mln_stack_empty

```c
mln_stack_empty(s)
```

Description: Check if the stack is empty.

Return value: null is `non-0`, otherwise `0`



#### mln_stack_top

```c
mln_stack_top(st)
```

Description: Get the top element data of the stack.

Return value: Returns `NULL` if stack `st` is empty, otherwise it is the data pointer in the top node of the stack



#### mln_stack_dup

```c
mln_stack_t *mln_stack_dup(mln_stack_t *st, void *udata);
```

Description: Completely duplicate stack `st`. `udata` provides additional data for the user.

Return value: if successful, return the new stack pointer, otherwise return `NULL`



#### mln_stack_scan_all

```c
int mln_stack_scan_all(mln_stack_t *st, stack_scan scanner, void *data);

typedef int (*stack_scan)(void *, void *);
```

Description:

Traverse the data of each element in the stack `st` from the top of the stack to the bottom of the stack. `scanner` is the data access function, `data` is additional user data when traversing.

`stack_scan` has two parameters: the data pointer in the stack node and the `data` parameter.

return value:

- `mln_stack_scan_all`: return `0` after all traversal, otherwise return `-1`
- `stack_scan`: If you want to interrupt the traversal, return the value of `less than 0`, otherwise the return value of `greater than or equal to 0`

