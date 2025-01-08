#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h> // Ajouté pour procfs

// Prototypes
static int leds_probe(struct platform_device *pdev);
static int leds_remove(struct platform_device *pdev);
static ssize_t leds_read(struct file *file, char *buffer, size_t len, loff_t *offset);
static ssize_t leds_write(struct file *file, const char *buffer, size_t len, loff_t *offset);
static ssize_t pattern_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset);
static ssize_t pattern_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset);

// An instance of this structure will be created for every ensea_led IP in the system
struct ensea_leds_dev {
    struct miscdevice miscdev;
    void __iomem *regs;
    u8 leds_value;
    u8 pattern_value;
};

static struct proc_dir_entry *ensea_dir;

// Specify which device tree devices this driver supports
static struct of_device_id ensea_leds_dt_ids[] = {
    {
        .compatible = "dev,ensea"
    },
    { /* end of table */ }
};

// Inform the kernel about the devices this driver supports
MODULE_DEVICE_TABLE(of, ensea_leds_dt_ids);

// Data structure that links the probe and remove functions with our driver
static struct platform_driver leds_platform = {
    .probe = leds_probe,
    .remove = leds_remove,
    .driver = {
        .name = "Ensea LEDs Driver",
        .owner = THIS_MODULE,
        .of_match_table = ensea_leds_dt_ids
    }
};

// The file operations for /dev/ensea-led
static const struct file_operations ensea_leds_fops = {
    .owner = THIS_MODULE,
    .read = leds_read,
    .write = leds_write
};

// The file operations for /proc/ensea/pattern
static const struct file_operations pattern_fops = {
    .owner = THIS_MODULE,
    .read = pattern_read,
    .write = pattern_write
};

// Called when the driver is installed
static int leds_init(void)
{
    int ret_val = 0;
    pr_info("Initializing the Ensea LEDs module\n");

    // Register our driver with the "Platform Driver" bus
    ret_val = platform_driver_register(&leds_platform);
    if(ret_val != 0) {
        pr_err("platform_driver_register returned %d\n", ret_val);
        return ret_val;
    }

    pr_info("Ensea LEDs module successfully initialized!\n");

    return 0;
}

// Called whenever the kernel finds a new device that our driver can handle
static int leds_probe(struct platform_device *pdev)
{
    int ret_val = -EBUSY;
    struct ensea_leds_dev *dev;
    struct resource *r = 0;

    pr_info("leds_probe enter\n");

    // Get the memory resources for this LED device
    r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(r == NULL) {
        pr_err("IORESOURCE_MEM (register space) does not exist\n");
        goto bad_exit_return;
    }

    // Create structure to hold device-specific information (like the registers)
    dev = devm_kzalloc(&pdev->dev, sizeof(struct ensea_leds_dev), GFP_KERNEL);

    // Both request and ioremap a memory region
    dev->regs = devm_ioremap_resource(&pdev->dev, r);
    if(IS_ERR(dev->regs))
        goto bad_ioremap;

    // Turn the LEDs on (access the 0th register in the ensea LEDs module)
    dev->leds_value = 0xFF;
    iowrite32(dev->leds_value, dev->regs);

    // Initialize the misc device (this is used to create a character file in userspace)
    dev->miscdev.minor = MISC_DYNAMIC_MINOR;
    dev->miscdev.name = "ensea_leds";
    dev->miscdev.fops = &ensea_leds_fops;

    ret_val = misc_register(&dev->miscdev);
    if(ret_val != 0) {
        pr_info("Couldn't register misc device :(");
        goto bad_exit_return;
    }

    // Create /proc/ensea directory
    ensea_dir = proc_mkdir("ensea", NULL);
    if (!ensea_dir) {
        pr_err("Failed to create /proc/ensea\n");
        misc_deregister(&dev->miscdev);
        goto bad_exit_return;
    }

    // Register /proc/ensea/pattern
    if (!proc_create("pattern", 0666, ensea_dir, &pattern_fops)) {
        pr_err("Failed to create /proc/ensea/pattern\n");
        remove_proc_entry("ensea", NULL);
        misc_deregister(&dev->miscdev);
        goto bad_exit_return;
    }

    platform_set_drvdata(pdev, dev);

    pr_info("leds_probe exit\n");

    return 0;

bad_ioremap:
   ret_val = PTR_ERR(dev->regs);
bad_exit_return:
    pr_info("leds_probe bad exit :(");
    return ret_val;
}

// Function to read the current pattern from /proc/ensea/pattern
static ssize_t pattern_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset)
{
    struct ensea_leds_dev *dev = PDE_DATA(file_inode(file));
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "Pattern: 0x%X\n", dev->pattern_value);

    if (*offset > 0 || count < len)
        return 0;

    if (copy_to_user(user_buffer, buffer, len))
        return -EFAULT;

    *offset = len;
    return len;
}

// Function to write a new pattern to /proc/ensea/pattern
static ssize_t pattern_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset)
{
    struct ensea_leds_dev *dev = platform_get_drvdata(file->private_data);
    char buffer[8];

    if (count > sizeof(buffer) - 1)
        return -EINVAL;

    if (copy_from_user(buffer, user_buffer, count))
        return -EFAULT;

    buffer[count] = '\0';
    sscanf(buffer, "%hhx", &dev->pattern_value);
    pr_info("New pattern: 0x%X\n", dev->pattern_value);

    // Update the LEDs with the new pattern
    iowrite32(dev->pattern_value, dev->regs);

    return count;
}

// This function gets called whenever a read operation occurs on one of the character files
static ssize_t leds_read(struct file *file, char *buffer, size_t len, loff_t *offset)
{
    int success = 0;
    struct ensea_leds_dev *dev = container_of(file->private_data, struct ensea_leds_dev, miscdev);

    success = copy_to_user(buffer, &dev->leds_value, sizeof(dev->leds_value));

    if(success != 0) {
        pr_info("Failed to return current led value to userspace\n");
        return -EFAULT;
    }

    return sizeof(dev->leds_value);
}

// This function gets called whenever a write operation occurs on one of the character files
static ssize_t leds_write(struct file *file, const char *buffer, size_t len, loff_t *offset)
{
    int success = 0;
    struct ensea_leds_dev *dev = container_of(file->private_data, struct ensea_leds_dev, miscdev);

    success = copy_from_user(&dev->leds_value, buffer, sizeof(dev->leds_value));

    if(success != 0) {
        pr_info("Failed to read led value from userspace\n");
        return -EFAULT;
    } else {
        iowrite32(dev->leds_value, dev->regs);
    }

    return len;
}

// Gets called whenever a device this driver handles is removed.
static int leds_remove(struct platform_device *pdev)
{
    struct ensea_leds_dev *dev = platform_get_drvdata(pdev);

    pr_info("leds_remove enter\n");

    iowrite32(0x00, dev->regs);
    misc_deregister(&dev->miscdev);
    remove_proc_entry("pattern", ensea_dir);
    remove_proc_entry("ensea", NULL);

    pr_info("leds_remove exit\n");

    return 0;
}

// Called when the driver is removed
static void leds_exit(void)
{
    pr_info("Ensea LEDs module exit\n");

    platform_driver_unregister(&leds_platform);

    pr_info("Ensea LEDs module successfully unregistered\n");
}

module_init(leds_init);
module_exit(leds_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Devon Andrade <devon.andrade@oit.edu>");
MODULE_DESCRIPTION("Exposes a character device to user space that lets users turn LEDs on and off, with pattern control via /proc");
MODULE_VERSION("1.0");
