/* AiO HotPlug v2.0, an All in One HotPlug for Traditional Quad-Core SoCs and 
 * Hexa/Octa-Core big.LITTLE SoCs.
 *
 * Copyright (c) 2017, Shoaib Anwar <Shoaib0595@gmail.com>
 *
 * Based on State_Helper HotPlug, by Pranav Vashi <neobuddy89@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

// Assume Quad-Core SoCs to be of Traditional Configuration and Hexa/Octa-Core SoCs to be of big.LITTLE Configuration.

#include <linux/cpu.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>

#define AIO_HOTPLUG			"AiO_HotPlug"
#define AIO_TOGGLE			0

#if (NR_CPUS == 4)
    #define DEFAULT_CORES		4
#elif (NR_CPUS == 6)
      #define DEFAULT_BIG_CORES		2
      #define DEFAULT_LITTLE_CORES	4
#elif (NR_CPUS == 8)
      #define DEFAULT_BIG_CORES		4
      #define DEFAULT_LITTLE_CORES	4
#endif


static struct AiO_HotPlug {
       unsigned int toggle;
       #if (NR_CPUS == 4)
       unsigned int cores;
       #elif (NR_CPUS == 6 || NR_CPUS == 8)
       unsigned int big_cores;
       unsigned int LITTLE_cores;
       #endif
} AiO = {
	.toggle 	 = AIO_TOGGLE,
	#if (NR_CPUS == 4)
	.cores		 = DEFAULT_CORES,
	#elif (NR_CPUS == 6 || NR_CPUS == 8)
	.big_cores	 = DEFAULT_BIG_CORES,
	.LITTLE_cores    = DEFAULT_LITTLE_CORES,
	#endif
};

static struct delayed_work AiO_work;
static struct workqueue_struct *AiO_wq;

int AiO_HotPlug;
extern int TEMP_SAFETY;
#ifdef CONFIG_ALUCARD_HOTPLUG
extern int alucard;
#endif

#ifdef CONFIG_FRANCO_THERMAL
#if (NR_CPUS == 6 || NR_CPUS == 8)
// Variable required to know whether the Thermal Frequency Table has been altered by the user on a big.LITTLE SoC.
extern int flag;
#endif
#endif

#ifdef CONFIG_CORE_CONTROL
// Variable required to know the status of Shoaib's Core Control.
extern bool core_control;
#endif

#if (NR_CPUS == 6 || NR_CPUS == 8)
// "Permission to Disable Core 0" Toggle.
extern bool hotplug_boost;
#endif

static void __ref AiO_HotPlug_work(struct work_struct *work)
{
         // Operations for a Traditional Quad-Core SoC.
         #if (NR_CPUS == 4)
	     if (AiO.cores == 1)
	     {	   
	        if (cpu_online(3))
	           cpu_down(3);
	        if (cpu_online(2))
	           cpu_down(2);
	        if (cpu_online(1)) 
                   cpu_down(1);
	     }
	     else if (AiO.cores == 2)
	     {
	   	     if (!cpu_online(1))
	      	        cpu_up(1);
	   
	   	     if (cpu_online(3))
	                cpu_down(3);
	   	     if (cpu_online(2))
	                cpu_down(2);
	     }
	     else if (AiO.cores == 3)
	     {
	   	     if (!cpu_online(1))
	      	        cpu_up(1);
	   	     if (!cpu_online(2))
	      	        cpu_up(2);
	   
	   	     if (cpu_online(3))
	      	        cpu_down(3);
	     }
	     else if (AiO.cores == 4)
	     {
	           if (!cpu_online(1))
	      	      cpu_up(1);
	           if (!cpu_online(2))
	      	      cpu_up(2);
	           if (!cpu_online(3))
	      	      cpu_up(3);
	     }
	  // Operations for a big.LITTLE SoC.
	  #elif (NR_CPUS == 6 || NR_CPUS == 8)
	        // Operations for big Cluster.
                if (AiO.big_cores == 0)
	        {
	           if (cpu_online(3))
	              cpu_down(3);
	   	   if (cpu_online(2))
	      	      cpu_down(2);
	   	   if (cpu_online(1)) 
              	      cpu_down(1);
	   	   if (cpu_online(0))
	      	      cpu_down(0);
	        }
	        else if (AiO.big_cores == 1)
	        {
	                if (!cpu_online(0))
	                   cpu_up(0);
	   
	                if (cpu_online(3))
	          	   cpu_down(3);
	      	        if (cpu_online(2))
	           	   cpu_down(2);
	        	if (cpu_online(1)) 
                   	   cpu_down(1);
		}
		else if (AiO.big_cores == 2)
		{
	        	if (!cpu_online(0))
	           	   cpu_up(0);
	   		if (!cpu_online(1))
	      	   	   cpu_up(1);
	   
	   		if (cpu_online(3))
	           	   cpu_down(3);
	   		if (cpu_online(2))
	           	   cpu_down(2);
		}
		else if (AiO.big_cores == 3)
		{
	   		if (!cpu_online(0))
	           	   cpu_up(0);
	   		if (!cpu_online(1))
	      	   	   cpu_up(1);
	   		if (!cpu_online(2))
	      	   	   cpu_up(2);
	   
	   		if (cpu_online(3))
	      	   	   cpu_down(3);
		}
		else if (AiO.big_cores == 4)
		{
	   		if (!cpu_online(0))
	      	   	   cpu_up(0);
	   		if (!cpu_online(1))
	      	   	   cpu_up(1);
	   		if (!cpu_online(2))
	      	   	   cpu_up(2);
	   		if (!cpu_online(3))
	      	   	   cpu_up(3);
		}
		// Operations for LITTLE Cluster.
		if (AiO.LITTLE_cores == 0)
		{
	   	   if (cpu_online(7))
	      	      cpu_down(7);
	   	   if (cpu_online(6))
	      	      cpu_down(6);
	  	   if (cpu_online(5)) 
             	      cpu_down(5);
	   	   if (cpu_online(4))
	      	      cpu_down(4);
		}
		else if (AiO.LITTLE_cores == 1)
		{
	   		if (!cpu_online(4))
	      	   	   cpu_up(4);
	   
	   		if (cpu_online(7))
	      	   	   cpu_down(7);
	   		if (cpu_online(6))
	      	   	   cpu_down(6);
	   		if (cpu_online(5)) 
              	   	   cpu_down(5);
		}
		else if (AiO.LITTLE_cores == 2)
		{
	   		if (!cpu_online(4))
	           	   cpu_up(4);
	   		if (!cpu_online(5))
	           	   cpu_up(5);
	   
	   		if (cpu_online(7))
	      	   	   cpu_down(7);
	   		if (cpu_online(6))
	                   cpu_down(6);
		}
		else if (AiO.LITTLE_cores == 3)
		{
	   		if (!cpu_online(4))
	      	   	   cpu_up(4);
	   		if (!cpu_online(5))
	      	   	   cpu_up(5);
	   		if (!cpu_online(6))
	      	   	   cpu_up(6);
	   
	   		if (cpu_online(7))
	      	  	   cpu_down(7);
		}
		else if (AiO.LITTLE_cores == 4)
		{
	  		if (!cpu_online(4))
	           	   cpu_up(4);
	   		if (!cpu_online(5))
	           	   cpu_up(5);
	   		if (!cpu_online(6))
	           	   cpu_up(6);
	   		if (!cpu_online(7))
	           	   cpu_up(7);
                }
          #endif
}

void reschedule_AiO(void)
{
	if (!AiO.toggle)
	   return;

	cancel_delayed_work_sync(&AiO_work);
	queue_delayed_work(AiO_wq, &AiO_work, msecs_to_jiffies(1000));
}

static void AiO_HotPlug_start(void)
{
	AiO_wq = alloc_workqueue("AiO_HotPlug_wq", WQ_HIGHPRI | WQ_FREEZABLE, 0);
	if (!AiO_wq) 
	{
	   pr_err("%s: Failed to allocate AiO workqueue\n", AIO_HOTPLUG);
	   goto err_out;
	}

	INIT_DELAYED_WORK(&AiO_work, AiO_HotPlug_work);
	reschedule_AiO();

	return;

err_out:
	AiO.toggle = 0;
	return;
}

static void __ref AiO_HotPlug_stop(void)
{
	int cpu;

	flush_workqueue(AiO_wq);
	cancel_delayed_work_sync(&AiO_work);

	/* Wake-Up All the Cores */
	for_each_possible_cpu(cpu)
	    if (!cpu_online(cpu))
	       cpu_up(cpu);
}

// Begin sysFS Interface
static ssize_t show_toggle(struct kobject *kobj,
			    struct kobj_attribute *attr, 
			    char *buf)
{
	return sprintf(buf, "%u\n", AiO.toggle);
}

static ssize_t store_toggle(struct kobject *kobj,
			     struct kobj_attribute *attr,
			     const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = sscanf(buf, "%u", &val);
	if (ret != 1 || val < 0 || val > 1)
	   return -EINVAL;
	
	#ifdef CONFIG_CORE_CONTROL
	// Allow AiO HotPlug to be Enabled only when Shoaib's Core Control is Disabled.
	if (core_control)
	   return -EINVAL;
#ifdef CONFIG_ALUCARD_HOTPLUG
	if (alucard)
	   return -EINVAL; 
#endif	
	if (val == AiO.toggle)
	   return count;

	AiO.toggle = val;
	AiO_HotPlug = AiO.toggle;

	if (AiO.toggle)
	   AiO_HotPlug_start();
	else
	   AiO_HotPlug_stop();

	return count;
}

#if (NR_CPUS == 4)
static ssize_t show_cores(struct kobject *kobj,
			  struct kobj_attribute *attr, 
			  char *buf)
{
	   return sprintf(buf, "%u\n", AiO.cores);
}
static ssize_t store_cores(struct kobject *kobj,
		           struct kobj_attribute *attr,
		           const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = sscanf(buf, "%u", &val);
	
	if (ret != 1 || val < 1 || val > 4)
	   return -EINVAL;

	AiO.cores = val;

	reschedule_AiO();

	return count;
}
#elif (NR_CPUS == 6 || NR_CPUS == 8)
static ssize_t show_big_cores(struct kobject *kobj,
			      struct kobj_attribute *attr, 
			      char *buf)
{	
	   return sprintf(buf, "%u\n", AiO.big_cores);
}

static ssize_t store_big_cores(struct kobject *kobj,
			       struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = sscanf(buf, "%u", &val);

	#ifdef CONFIG_FRANCO_THERMAL
	if (NR_CPUS == 6)
	{
	   if (ret != 1 || val < 0 || val > 2 || (val == 0 && (hotplug_boost == false || flag == 1 || AiO.LITTLE_cores == 0)))
	      return -EINVAL;
	}
	else if (NR_CPUS == 8)
	{
	        if (ret != 1 || val < 0 || val > 4 || (val == 0 && (hotplug_boost == false || flag == 1 || AiO.LITTLE_cores == 0)))
	           return -EINVAL;
	}
	#else
	if (NR_CPUS == 6)
	{
	   if (ret != 1 || val < 0 || val > 2 || (val == 0 && (hotplug_boost == false || AiO.LITTLE_cores == 0)))
	      return -EINVAL;
	}
	else if (NR_CPUS == 8)
	{
		if (ret != 1 || val < 0 || val > 4 || (val == 0 && (hotplug_boost == false || AiO.LITTLE_cores == 0)))
	           return -EINVAL;
	}
	#endif

	AiO.big_cores = val;

	reschedule_AiO();

	return count;
}

static ssize_t show_LITTLE_cores(struct kobject *kobj,
				 struct kobj_attribute *attr, 
				 char *buf)
{
	   return sprintf(buf, "%u\n", AiO.LITTLE_cores);
}

static ssize_t store_LITTLE_cores(struct kobject *kobj,
				  struct kobj_attribute *attr,
				  const char *buf, size_t count)
{
	int ret;
	unsigned int val;

	ret = sscanf(buf, "%u", &val);
	
	if (ret != 1 || val < 0 || val > 4 || (val == 0 && AiO.big_cores == 0))
	   return -EINVAL;

	AiO.LITTLE_cores = val;

	reschedule_AiO();

	return count;
}
#endif

#define KERNEL_ATTR_RW(_name) 				\
static struct kobj_attribute _name##_attr = 		\
       __ATTR(_name, 0664, show_##_name, store_##_name)

#define KERNEL_ATTR_RO(_name) 				\
static struct kobj_attribute _name##_attr = 		\
       __ATTR(_name, 0444, show_##_name, NULL)

KERNEL_ATTR_RW(toggle);
#if (NR_CPUS == 4)
    KERNEL_ATTR_RW(cores);
#elif (NR_CPUS == 6 || NR_CPUS == 8)
      KERNEL_ATTR_RW(big_cores);
      KERNEL_ATTR_RW(LITTLE_cores);
#endif

static struct attribute *AiO_HotPlug_attrs[] = {
	&toggle_attr.attr,
	#if (NR_CPUS == 4)
	    &cores_attr.attr,
	#elif (NR_CPUS == 6 || NR_CPUS == 8)
	      &big_cores_attr.attr,
	      &LITTLE_cores_attr.attr,
	#endif
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = AiO_HotPlug_attrs,
	.name  = AIO_HOTPLUG,
};
//  End sysFS Interface

static int AiO_HotPlug_probe(struct platform_device *pdev)
{
	int ret = 0;

	ret = sysfs_create_group(kernel_kobj, &attr_group);

	if (AiO.toggle)
	   AiO_HotPlug_start();

	return ret;
}

static struct platform_device AiO_HotPlug_device = {
	.name = AIO_HOTPLUG,
	.id = -1,
};

static int AiO_HotPlug_remove(struct platform_device *pdev)
{
	if (AiO.toggle)
	   AiO_HotPlug_stop();

	return 0;
}

static struct platform_driver AiO_HotPlug_driver = {
	.probe = AiO_HotPlug_probe,
	.remove = AiO_HotPlug_remove,
	.driver = {
		  .name  = AIO_HOTPLUG,
		  .owner = THIS_MODULE,
	},
};

static int __init AiO_HotPlug_init(void)
{
	int ret;

	ret = platform_driver_register(&AiO_HotPlug_driver);
	if (ret) 
	{
		pr_err("%s: Driver register failed: %d\n", AIO_HOTPLUG, ret);
		return ret;
	}

	ret = platform_device_register(&AiO_HotPlug_device);
	if (ret) 
	{
		pr_err("%s: Device register failed: %d\n", AIO_HOTPLUG, ret);
		return ret;
	}

	pr_info("%s: Device init\n", AIO_HOTPLUG);

	return ret;
}

static void __exit AiO_HotPlug_exit(void)
{
	platform_device_unregister(&AiO_HotPlug_device);
	platform_driver_unregister(&AiO_HotPlug_driver);
}

late_initcall(AiO_HotPlug_init);
module_exit(AiO_HotPlug_exit);

MODULE_AUTHOR("Shoaib Anwar <Shoaib0595@gmail.com>");
MODULE_DESCRIPTION("AiO HotPlug v2.0, an All in One HotPlug for Traditional Quad-Core SoCs and Hexa-Core and Octa-Core big.LITTLE SoCs.");
MODULE_LICENSE("GPLv2");
