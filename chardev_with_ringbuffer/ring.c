#include "ring.h"

int main(void) {
    ring_buffer rb = rb_create();
    rb_display(rb);
    rb_enqueue(rb,1);
    rb_display(rb);
    rb_enqueue(rb,2);
    rb_display(rb);
    rb_enqueue(rb,3);
    rb_display(rb);
    
    rb_dequeue(rb);
    rb_display(rb);
    rb_enqueue(rb,3);
    rb_display(rb);
    rb_enqueue(rb,3);
    
    rb_display(rb);
    
    rb_enqueue(rb,4);
    rb_display(rb);
    
    
    rb_delete(rb);
    return 0;
}
