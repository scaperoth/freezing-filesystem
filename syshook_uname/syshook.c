#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/unistd.h>
#include <linux/utsname.h>
#include <asm/pgtable.h>
#include <asm/desc.h>

MODULE_LICENSE("GPL");

typedef struct desc_struct gate_desc;
typedef void (*sys_call_ptr_t)(void);
typedef asmlinkage long (*orig_uname_t)(struct new_utsname *);

sys_call_ptr_t *_sys_call_table = NULL;
pte_t *pte;

orig_uname_t orig_uname = NULL;
char *msg = "Scaperoth Rules!";

struct desc_ptr {
        unsigned short size;
        unsigned long address;
} __attribute__((packed)) ;

pte_t *lookup_address(unsigned long address) 
{ 
        pgd_t *pgd = pgd_offset_k(address);
        pud_t *pud;
        pmd_t *pmd;
        if (pgd_none(*pgd))
                return NULL;
        pud = pud_offset(pgd, address);
        if (pud_none(*pud))
                return NULL;
        pmd = pmd_offset(pud, address);
        if (pmd_none(*pmd))
                return NULL;
        if (pmd_large(*pmd))
                return (pte_t *)pmd;
        return pte_offset_kernel(pmd, address);
}

asmlinkage long hooked_uname(struct new_utsname *name) {
    orig_uname(name);
    strncpy(name->sysname, msg, 27);
    return 0;
}

static int _init_module(void ) {    
    struct desc_ptr idtr;
    gate_desc *idt_table;
    gate_desc *system_call_gate;
    unsigned int _system_call_off;
    unsigned char *_system_call_ptr;
    unsigned int i;
    unsigned char *off;

    printk("+ Loading module\n");
    asm ("sidt %0" : "=m" (idtr));
    idt_table = (gate_desc *) idtr.address;
    system_call_gate = &idt_table[0x80];
    _system_call_off = (system_call_gate->a & 0xffff) | (system_call_gate->b & 0xffff0000);
    _system_call_ptr = (unsigned char *) _system_call_off;

    for(i = 0; i < 128; i++) {
        off = _system_call_ptr + i;
        if(*(off) == 0xff && *(off+1) == 0x14 && *(off+2) == 0x85) {
            _sys_call_table = *(sys_call_ptr_t **)(off+3);
            break;
        }
    }

    if(_sys_call_table == NULL) {
        printk("- unable to locate sys_call_table\n");
        return 0;
    }
    printk("+ found sys_call_table at %08x!\n", (unsigned int)_sys_call_table);
    orig_uname = (orig_uname_t) _sys_call_table[__NR_uname];
    pte = lookup_address((unsigned long) _sys_call_table);
////////////////////////////////////////////////
    set_pte_atomic(pte, pte_mkwrite(*pte));
    _sys_call_table[__NR_uname] = (sys_call_ptr_t) hooked_uname;
    set_pte_atomic(pte, pte_wrprotect(*pte));
////////////////////////////////////////////////
    printk("+ uname hooked!\n");
    return 0;
}

static void _cleanup_module(void) {
    if(orig_uname != NULL)
    {
        set_pte_atomic(pte, pte_mkwrite(*pte));
        _sys_call_table[__NR_uname] = (sys_call_ptr_t) orig_uname;
        set_pte_atomic(pte, pte_wrprotect(*pte));
    }

    printk("+ Unloading module\n");
}

module_init(_init_module);
module_exit(_cleanup_module);