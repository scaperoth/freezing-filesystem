/******************************
 | Author: Matt Scaperoth     |
 | Email: mscapero@gwu.edu    |
 |                            |
 *****************************/

typedef struct node node_type;
typedef struct node *single_node;

struct node
{
    void *value;
    node_type *prev;
    node_type *next;
};

typedef struct linked_list
{
    int length;
    node_type *head;
    node_type *tail;
} linked_list_type;

static inline void ll_destroy_node(single_node node);

static inline struct linked_list *ll_create(void);

static inline void ll_destroy(struct linked_list *ll);

static inline void ll_add(struct linked_list *ll, void *value);

static inline int ll_length(struct linked_list *ll);

static inline void *ll_remove_first(struct linked_list *ll);

static inline int ll_remove(struct linked_list *ll, void *value);

static inline int ll_contains(struct linked_list *ll, void *value);

static inline void ll_display(struct linked_list *ll);


/**
 *
 */
static inline struct linked_list *ll_create()
{
    struct linked_list *list = kmalloc(sizeof(struct linked_list), GFP_KERNEL);
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

/**
 *
 */
static inline void ll_add(struct linked_list *ll, void *value)
{
    single_node node = kmalloc(sizeof(node_type), GFP_KERNEL);

    if (node == NULL)
    {
        return;
    }

    node->value = value;

    if (ll->tail == NULL)
    {
        ll->head = node;
        ll->tail = node;
        node->prev = NULL;
        node->next = NULL;

    }
    else
    {
        node->prev = ll->tail;
        node->next = NULL;
        ll->tail->next = node;
        ll->tail = node;
    }

    ll->length++;

    return;
}

/**
 *
 */
static inline void ll_destroy(struct linked_list *ll)
{
    int num_nodes = ll->length;

    while (num_nodes)
    {
        ll_remove_first(ll);
        num_nodes--;
    }

    kfree(ll);
    printk("List destroyed\n");
}

/**
 *
 */
static inline void *ll_dequeue(struct linked_list *ll)
{

    void *rvalue;

    if (ll->length == 0)
    {
        return NULL;
    }
    else if (ll->length == 1)
    {
        rvalue = ll->head->value;
        single_node temp = ll->head;
        ll->head = NULL;
        ll->tail = NULL;
        kfree(ll->head);
        ll->length--;
    }
    else
    {
        rvalue = ll->head->value;
        single_node temp = ll->head->next;
        kfree(ll->head);
        ll->head = temp;
        ll->head->prev = NULL;
        ll->length--;
    }
    return rvalue;
}

/**
 *
 */
static inline void *ll_remove_first(struct linked_list *ll)
{

    void *rvalue;

    if (ll->length == 0)
    {
        return NULL;
    }
    else if (ll->length == 1)
    {
        rvalue = ll->tail->value;
        single_node temp = ll->tail;
        ll->tail = NULL;
        kfree(ll->tail);
        ll->length--;
    }
    else
    {
        rvalue = ll->tail->value;
        single_node temp = ll->tail->prev;
        kfree(ll->tail);
        ll->tail = temp;
        ll->tail->next = NULL;
        ll->tail->prev = temp->prev;
        ll->length--;


    }
    return rvalue;
}

/**
 *
 */
static inline int ll_remove(struct linked_list *ll, void *value)
{
    int num_nodes = ll->length;
    single_node next = ll->tail;
    single_node current_node;
    single_node temp;
    if (!num_nodes)
    {
        return -1;
    }
    else
    {
        while (next)
        {
            current_node = next;
            if (current_node->value == value)
            {
                if (current_node == ll->head)
                {
                    current_node->next->prev = NULL;

                }
                else if (current_node == ll->tail)
                {
                    current_node->prev->next = NULL;
                    ll->tail = current_node;
                }
                else
                {
                    current_node->prev->next = current_node->next;
                    current_node->next->prev = current_node->prev;
                }

                ll->length--;
                kfree(current_node);
            }
            next = next->prev;
        }
    }

    return 0;
}

/**
 *
 */
static inline int ll_length(struct linked_list *ll)
{

    node_type *prev = ll->tail;
    int count = 0;
    while (prev != NULL)
    {

        count++;
        node_type *temp = prev;
        prev = temp->prev;
    }

    return count;
}

/**
 *
 */
static inline int ll_contains(struct linked_list *ll, void *value)
{
    single_node next = ll->head;
    while (next)
    {
        single_node node = next;
        if (node->value == value)
            return 1;
        next = node->next;
    }

    return 0;
}

/**
 *
 */
static inline void ll_display(struct linked_list *ll)
{
    single_node next = ll->head;
    printk("List: { ");
    if (ll_length(ll))
    {
        while (next)
        {
            if (next->value)
            {
                printk("%s ", (char*)next->value);
            }
            next = next->next;
        }
    }
    printk("}\n");
    return;
}
