   #include <linux/kernel.h>         
   #include <linux/device.h>         
   #include <linux/random.h>
   #include <linux/init.h>        
   #include <linux/module.h>         
   #include <linux/fs.h>             
   #include <linux/uaccess.h>
            
   #define  DEVICE_NAME "project02"    
   #define  CLASS_NAME  "chardriv"               

   static int major;    
   static struct class* genCharClass; 
   static int onOpen(struct inode *i, struct file *f){
      printk(KERN_INFO "Device: on Open\n");
      return 0;
   }

   static ssize_t onRead(struct file *f, char *buf, size_t len, loff_t *off){
      printk(KERN_INFO "Device: on Read\n");
      int number;
      char messageResult[255]; 
      get_random_bytes(&number, sizeof(number));
      sprintf(messageResult, "%d", number);

      if (copy_to_user(buf, messageResult, 255)!=0){
         printk(KERN_ERR "Could not send to user\n");
         return -EFAULT;  
      }
      else {
         printk(KERN_INFO "Sent number: %d to user\n", number);
         return 0; 
      }
   }
   static struct file_operations fops =
   {
      .open = onOpen,
      .read = onRead,
   };

   static int __init genRandNumberInit(void){
      printk(KERN_INFO "Initializing the genChar\n");

      major = register_chrdev(0, DEVICE_NAME, &fops);
      if (major<0){
         printk(KERN_ERR "Register major number failed\n");
         return major;
      }
      printk(KERN_INFO "Major number: %d\n", major);
      // Create class
      if ((genCharClass = class_create(THIS_MODULE, CLASS_NAME))==NULL){               
         unregister_chrdev(major, DEVICE_NAME);
         printk(KERN_ERR "Register device class failed\n");
         return -1;          
      }
      printk(KERN_INFO "Register device class successfully \n");

      // Create device
      if (  device_create(genCharClass, NULL, MKDEV(major, 0), NULL, DEVICE_NAME)==NULL){               
         class_destroy(genCharClass);          
         unregister_chrdev(major, DEVICE_NAME);
         printk(KERN_ERR "Create device failed\n");
         return -1;
      }
      printk(KERN_INFO "Created device successfully\n"); 
      return 0;
   }

   static void __exit genRandNumberExit(void){
      device_destroy(genCharClass, MKDEV(major, 0));     
      class_unregister(genCharClass);
      class_destroy(genCharClass);
      unregister_chrdev(major, DEVICE_NAME);
      printk(KERN_INFO "Exit generation random number\n");
   }


   module_init(genRandNumberInit);
   module_exit(genRandNumberExit);
   MODULE_LICENSE("GPL");
   MODULE_AUTHOR("1712273 - 1712304");
   MODULE_DESCRIPTION("Character device driver generation random number");  
