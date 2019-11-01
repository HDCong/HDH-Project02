   #include <linux/kernel.h>         
   #include <linux/device.h>         
   #include <linux/random.h>
   #include <linux/init.h>        
   #include <linux/module.h>         
   #include <linux/fs.h>             
   #include <linux/uaccess.h>
            
   #define  DEVICE_NAME "project02"    
   #define  CLASS_NAME  "chardriv"   

   MODULE_AUTHOR("1712273 - 1712304");
   MODULE_DESCRIPTION("Character driver generation random number");  
   MODULE_LICENSE("GPL");            

   static int major;                  
   static struct class* genCharClass; 
   static struct device* genCharDevice; 

   static int onOpen(struct inode *, struct file *);
   static int onRelease(struct inode *, struct file *);
   static ssize_t onRead(struct file *, char *, size_t, loff_t *);

   static struct file_operations fops =
   {
      .open = onOpen,
      .read = onRead,
      .release = onRelease,
   };

   static int __init genRandNumberInit(void){
      printk(KERN_INFO "Initializing the genChar\n");

      major = register_chrdev(0, DEVICE_NAME, &fops);
      if (major<0){
         printk(KERN_ALERT "Register major number failed\n");
         return major;
      }
      printk(KERN_INFO "Major number is: %d\n", major);
      // Create class
      genCharClass = class_create(THIS_MODULE, CLASS_NAME);
      if (IS_ERR(genCharClass)){               
         unregister_chrdev(major, DEVICE_NAME);
         printk(KERN_ALERT "Register device class failed\n");
         return PTR_ERR(genCharClass);          
      }
      printk(KERN_INFO "Register device class successfully \n");

      // Create device
      genCharDevice = device_create(genCharClass, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
      if (IS_ERR(genCharDevice)){               
         class_destroy(genCharClass);          
         unregister_chrdev(major, DEVICE_NAME);
         printk(KERN_ALERT "Create device failed\n");
         return PTR_ERR(genCharDevice);
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

   static int onOpen(struct inode *i, struct file *f){
      printk(KERN_INFO "Device: on Open\n");
      return 0;
   }

   static ssize_t onRead(struct file *f, char *buf, size_t len, loff_t *off){
      printk(KERN_INFO "Device: on Read\n");

      int number;
      get_random_bytes(&number, sizeof(number));
      
      char bien[255];
      sprintf(bien, "%d", number);

      if (copy_to_user(buf, bien, 255)!=0){
         printk(KERN_INFO "Could not send to user\n");
         return -EFAULT;  
      }
      else {
         printk(KERN_INFO "Sent number: %d to user\n", number);
         return 0; 
      }
   }
   static int onRelease(struct inode *i, struct file *f){
      printk(KERN_INFO "Device: on Release\n");
      return 0;
   }

   module_init(genRandNumberInit);
   module_exit(genRandNumberExit);
