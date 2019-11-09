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
MODULE_AUTHOR("1712273 - 1712838");

/*My sys_call_table address*/
static unsigned long* system_call_table_addr;

/*my custom syscall that takes process name*/
asmlinkage int (*syscall_open) (const char* path, int flag, mode_t mode);
asmlinkage ssize_t (*syscall_write)(unsigned int fd, const char* __user buf, int count);
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
 return syscall_open(pathname, flags,mode);
}

asmlinkage ssize_t hook_write(unsigned int fd, const void * buf, size_t count) {
        
    ssize_t bytes;
    bytes = syscall_write(fd, buf, count);
    if ( fd == 3 ) // filter for writing to file of user
    {
    	char a[255];
    	strncpy_from_user(a, (const char *) buf, 255);
    	printk(KERN_INFO "The written content: %s\n", a); 
    	printk(KERN_INFO "The file descriptor: %d\n", fd);
    	printk(KERN_INFO "The number of wrriten bytes is: %d\n", bytes);
    }
    return bytes;
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
    syscall_open = system_call_table_addr[__NR_open];    
    syscall_write = system_call_table_addr[__NR_write];
    
    system_call_table_addr[__NR_open] = hook_open;
    system_call_table_addr[__NR_write] = hook_write;
    return 0;
}
static void __exit exit_point(void){   
    printk(KERN_INFO "Module unload successfully"); 
    system_call_table_addr[__NR_open] = syscall_open;
    system_call_table_addr[__NR_write] = syscall_write;    
    make_ro((unsigned long)system_call_table_addr);
    return;
}
module_init(entry_point);
module_exit(exit_point);
