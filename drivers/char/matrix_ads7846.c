#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
// #include <mach/gpio-samsung.h>
#include <linux/spi/ads7846.h>
#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_data/spi-s3c64xx.h>

#define CONFIG_S3C_GPIO_SPACE 0

/*
 * Copyright (c) 2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C2410 - GPIO lib support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

/* some boards require extra gpio capacity to support external
 * devices that need GPIO.
 */

#ifndef GPIO_SAMSUNG_S3C24XX_H
#define GPIO_SAMSUNG_S3C24XX_H

/*
 * GPIO sizes for various SoCs:
 *
 *   2410 2412 2440 2443 2416
 *             2442
 *   ---- ---- ---- ---- ----
 * A  23   22   25   16   27
 * B  11   11   11   11   11
 * C  16   16   16   16   16
 * D  16   16   16   16   16
 * E  16   16   16   16   16
 * F  8    8    8    8    8
 * G  16   16   16   16   8
 * H  11   11   11   15   15
 * J  --   --   13   16   --
 * K  --   --   --   --   16
 * L  --   --   --   15   14
 * M  --   --   --   2    2
 */

/* GPIO bank sizes */

#define S3C2410_GPIO_A_NR	(32)
#define S3C2410_GPIO_B_NR	(32)
#define S3C2410_GPIO_C_NR	(32)
#define S3C2410_GPIO_D_NR	(32)
#define S3C2410_GPIO_E_NR	(32)
#define S3C2410_GPIO_F_NR	(32)
#define S3C2410_GPIO_G_NR	(32)
#define S3C2410_GPIO_H_NR	(32)
#define S3C2410_GPIO_J_NR	(32)	/* technically 16. */
#define S3C2410_GPIO_K_NR	(32)	/* technically 16. */
#define S3C2410_GPIO_L_NR	(32)	/* technically 15. */
#define S3C2410_GPIO_M_NR	(32)	/* technically 2. */

#if CONFIG_S3C_GPIO_SPACE != 0
#error CONFIG_S3C_GPIO_SPACE cannot be nonzero at the moment
#endif

#define S3C2410_GPIO_NEXT(__gpio) \
	((__gpio##_START) + (__gpio##_NR) + CONFIG_S3C_GPIO_SPACE + 0)

#ifndef __ASSEMBLY__

enum s3c_gpio_number {
	S3C2410_GPIO_A_START = 0,
	S3C2410_GPIO_B_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_A),
	S3C2410_GPIO_C_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_B),
	S3C2410_GPIO_D_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_C),
	S3C2410_GPIO_E_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_D),
	S3C2410_GPIO_F_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_E),
	S3C2410_GPIO_G_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_F),
	S3C2410_GPIO_H_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_G),
	S3C2410_GPIO_J_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_H),
	S3C2410_GPIO_K_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_J),
	S3C2410_GPIO_L_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_K),
	S3C2410_GPIO_M_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_L),
};

#endif /* __ASSEMBLY__ */

/* S3C2410 GPIO number definitions. */

#define S3C2410_GPA(_nr)	(S3C2410_GPIO_A_START + (_nr))
#define S3C2410_GPB(_nr)	(S3C2410_GPIO_B_START + (_nr))
#define S3C2410_GPC(_nr)	(S3C2410_GPIO_C_START + (_nr))
#define S3C2410_GPD(_nr)	(S3C2410_GPIO_D_START + (_nr))
#define S3C2410_GPE(_nr)	(S3C2410_GPIO_E_START + (_nr))
#define S3C2410_GPF(_nr)	(S3C2410_GPIO_F_START + (_nr))
#define S3C2410_GPG(_nr)	(S3C2410_GPIO_G_START + (_nr))
#define S3C2410_GPH(_nr)	(S3C2410_GPIO_H_START + (_nr))
#define S3C2410_GPJ(_nr)	(S3C2410_GPIO_J_START + (_nr))
#define S3C2410_GPK(_nr)	(S3C2410_GPIO_K_START + (_nr))
#define S3C2410_GPL(_nr)	(S3C2410_GPIO_L_START + (_nr))
#define S3C2410_GPM(_nr)	(S3C2410_GPIO_M_START + (_nr))

#ifdef CONFIG_CPU_S3C244X
#define S3C_GPIO_END	(S3C2410_GPJ(0) + 32)
#elif defined(CONFIG_CPU_S3C2443) || defined(CONFIG_CPU_S3C2416)
#define S3C_GPIO_END	(S3C2410_GPM(0) + 32)
#else
#define S3C_GPIO_END	(S3C2410_GPH(0) + 32)
#endif

#endif /* GPIO_SAMSUNG_S3C24XX_H */


#define DRVNAME "ads7846_device"

#ifdef MODULE
static struct spi_device *spi_device;
static void ads7846_device_spi_delete(struct spi_master *master, unsigned cs)
{
	struct device *dev;
	char str[32];

	snprintf(str, sizeof(str), "%s.%u", dev_name(&master->dev), cs);

	dev = bus_find_device_by_name(&spi_bus_type, NULL, str);
	if (dev) {
		pr_info(DRVNAME": Deleting %s\n", str);
		device_del(dev);
	}
}

static int ads7846_device_spi_device_register(struct spi_board_info *spi)
{
	struct spi_master *master;

	master = spi_busnum_to_master(spi->bus_num);
	if (!master) {
		pr_err(DRVNAME ":  spi_busnum_to_master(%d) returned NULL\n",
								spi->bus_num);
		return -EINVAL;
	}
	/* make sure it's available */
	ads7846_device_spi_delete(master, spi->chip_select);
	spi_device = spi_new_device(master, spi);
	put_device(&master->dev);
	if (!spi_device) {
		pr_err(DRVNAME ":    spi_new_device() returned NULL\n");
		return -EPERM;
	}
	return 0;
}
#else
static int ads7846_device_spi_device_register(struct spi_board_info *spi)
{
	return spi_register_board_info(spi, 1);
}
#endif

static int ads7846_get_pendown_state(void)
{
	return !gpio_get_value(S3C2410_GPG(11));
}

static struct s3c64xx_spi_csinfo spi0_csi = {
	.line = S3C2410_GPG(10),
	.fb_delay = 0x2,
};

static const struct ads7846_platform_data ads7846_ts_info = {
	.model		= 7846,
	.x_min		= 100,
	.y_min		= 100,
	.x_max		= 0x0fff,
	.y_max		= 0x0fff,
	.vref_mv	= 3300,
	.x_plate_ohms	= 256,
	.penirq_recheck_delay_usecs = 10,
	.settle_delay_usecs = 100,
	.keep_vref_on	= 1,
	.pressure_max	= 1500,
	.debounce_max	= 10,
	.debounce_tol	= 30,
	.debounce_rep	= 1,
	.get_pendown_state	= ads7846_get_pendown_state,    
};

static struct spi_board_info __initdata ads7846_boardinfo = {
    /* MicroWire (bus 2) CS0 has an ads7846e */
    .modalias               = "ads7846",
    .platform_data          = &ads7846_ts_info,
    .max_speed_hz           = 800 * 1000,
    .bus_num                = 0,
    .chip_select            = 1,		// second spi dev, first is fbtft
    .controller_data		= &spi0_csi,
};

static int __init ads7846_dev_init(void)
{	
	ads7846_boardinfo.irq = gpio_to_irq(S3C2410_GPG(11));
	ads7846_device_spi_device_register(&ads7846_boardinfo);
	return 0;
}

static void __exit ads7846_dev_exit(void)
{
	if (spi_device) {
		if (spi_device->master->cleanup) {
			spi_device->master->cleanup(spi_device);
		}	
		device_del(&spi_device->dev);
		kfree(spi_device);
	}
}

module_init(ads7846_dev_init);
module_exit(ads7846_dev_exit);
MODULE_DESCRIPTION("Add a ads7846 spi device.");
MODULE_AUTHOR("FriendlyARM");
MODULE_LICENSE("GPL");
