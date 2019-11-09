#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <linux/fdtable.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("1712273 - 1712304");

/*MY sys_call_table address*/

static unsigned long* system_call_table_addr;
/*my custom syscall that takes process name*/
asmlinkage int (*custom_syscall) (const char* path, int flag, mode_t mode);
/*hook*/
asmlinkage int hook_open(const char *pathname, int flags, mode_t mode)
{
 // Print calling process name
 printk(KERN_INFO "Calling process: %p \n",current->comm);
 // Print openning file
 char a[255];
 if(pathname!=NULL){
 	strncpy_from_user(a,pathname, 255);
 }
 printk(KERN_INFO "Openning file: %s \n",a);
 return custom_syscall(pathname, flags,mode);
}

/*Make page writeable*/
int make_rw(unsigned long address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    if(pte->pte &~_PAGE_RW){
        pte->pte |=_PAGE_RW;
    }
    return 0;
}
/* Make the page write protected */
int make_ro(unsigned long address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    pte->pte = pte->pte &~_PAGE_RW;
    return 0;
}

static int __init entry_point(void){
    printk(KERN_INFO "Module load successfully");	
    system_call_table_addr = (void *)kallsyms_lookup_name("sys_call_table");
    if (!system_call_table_addr) {
	printk(KERN_INFO "sch: Couldn't look up sys_call_table\n");
	return -1;
    }
    printk(KERN_INFO "sch: sys_call_table [%p]\n", system_call_table_addr);
    make_rw((unsigned long)system_call_table_addr);
    custom_syscall = system_call_table_addr[__NR_open];    
        
    system_call_table_addr[__NR_open] = hook_open;
    return 0;
}
static void __exit exit_point(void){   
    printk(KERN_INFO "Module unload successfully"); 
    system_call_table_addr[__NR_open] = custom_syscall;    
    make_ro((unsigned long)system_call_table_addr);
    return;
}
module_init(entry_point);
module_exit(exit_point);
