/******************************
 | Author: Matt Scaperoth     |
 | Email: mscapero@gwu.edu    |
 |                            |
 *****************************/

#define BUFFER_LENGTH 16


typedef struct node node_type;
typedef struct node* single_node;

struct node {
    void *value;
    node_type *prev;
    node_type *next;
};


typedef struct ring ring_type;
typedef struct ring* ring_buffer;

struct ring {
    int head;
    int tail;
    single_node array[BUFFER_LENGTH];
};



/*=============================
 * prototypes
===============================*/
static inline void rb_destroy_node(single_node node);

static inline ring_buffer rb_create(void);

static inline void rb_delete(ring_buffer rb);

static inline int rb_isempty(ring_buffer rb);

static inline int rb_isfull(ring_buffer rb);

static inline int rb_enqueue(ring_buffer rb, void * value);

static inline void* rb_dequeue(ring_buffer rb);

static inline void rb_display(ring_buffer rb);

/*=============================*/

/**
 * frees a single node. if node does not exist, returns
 * @param single_node node as a node of a linked_list
 * @return void
 */
static inline void rb_destroy_node(single_node node) {
    if (node == NULL) {
        return;
    }
    kfree(node);
}

/**
 * allocates and returns a pointer to a linked list. if unable to allocate
 * the list, rb_create returns NULL
 * @return struct ring_buffer* as ring_buffer
 */
static inline ring_buffer rb_create() {
    //allocate enough memory for linked list type and cast to list
    ring_buffer rb = (ring_buffer) kmalloc(sizeof (ring_type),GFP_KERNEL);

    //return NULL if memory cannot be allocated
    if (!rb) {
        return NULL;
    }

    rb->head = 0;
    rb->tail = 1;
    return rb;
}

static inline void rb_delete(ring_buffer rb) {
    while (!rb_isempty(rb)) {
        rb_dequeue(rb);
    }

    kfree(rb);
}

static inline int rb_isempty(ring_buffer rb) {
    if (rb->tail == (rb->head + 1) % BUFFER_LENGTH) {
        return -1;
    } else return 0;
}

static inline int rb_isfull(ring_buffer rb) {
    if (rb->tail == rb->head) {
        return -1;
    } else return 0;
}

static inline int rb_enqueue(ring_buffer rb, void * value) {

    if (rb_isfull(rb)) {

        printk("Error: List is full. Cannot enqueue until dequeue.\n");
        return 0;
    }
    //allocate enough memory for the node type and cast to a node
    single_node node = (single_node) kmalloc(sizeof (node_type),GFP_KERNEL);

    int tail = rb->tail;
    //return NULL if memory cannot be allocated
    if (node == NULL) {
        printk("ERROR: Could not allocate memory for new item.\n");
        return 0;
    }

    //set the value
    node->value = (void*) value;

    rb->array[tail] = node;
    tail++;
    rb->tail = tail % BUFFER_LENGTH;
    return -1;
}

/** 
 * removes the value at the tail of the linked list and returns the value
 * if the list is empty, rb_dequeue returns NULL
 * @return void* rvalue 
 */
static inline void* rb_dequeue(ring_buffer rb) {
    int head = rb->head;
    if (rb_isempty(rb)) {

        printk("Error: List is empty. Cannot dequeue until enqueue.\n");
        return 0;
    }
    void* rvalue;
    int i;
    for (i = 0; i < BUFFER_LENGTH; i++) {
        if (rb->array[i]) {
            rvalue = rb->array[i]->value;
            rb_destroy_node(rb->array[i]);
            rb->array[i] = NULL;
            head++;
            head = head % BUFFER_LENGTH;
            rb->head = head;
            return rvalue;
        }
    }
    return NULL;
}

static inline void rb_display(ring_buffer rb) {
    int i;

    printk("%d===%d\n", rb->head, rb->tail);
    printk("List{ ");
    for (i = 0; i < BUFFER_LENGTH; i++) {
        if (rb->array[i]) {
            printk(" %d ", rb->array[i]->value);
        } else {
            printk(" || ");
        }
    }
    printk(" }\n");

    return;
}
