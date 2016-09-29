/*
 * Configuration file for the SAMA5D2 Xplained Board.
 *
 * Copyright (C) 2015 Atmel Corporation
 *		      Wenyou Yang <wenyou.yang@atmel.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* No NOR flash, this definition should put before common header */
#define CONFIG_SYS_NO_FLASH

#include "at91-sama5_common.h"

/* serial console */
#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE			115200
#define CONFIG_ATMEL_USART
#define CONFIG_USART_BASE		ATMEL_BASE_UART1
#define CONFIG_USART_ID			ATMEL_ID_UART1

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE           ATMEL_BASE_DDRCS
#define CONFIG_SYS_SDRAM_SIZE		0x08000000

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_INIT_SP_ADDR		0x210000
#else
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + 4 * 1024 - GENERATED_GBL_DATA_SIZE)
#endif

#define CONFIG_SYS_LOAD_ADDR		0x22000000 /* load address */

#undef CONFIG_AT91_GPIO
#define CONFIG_ATMEL_PIO4

/* SerialFlash */
#define CONFIG_QSPI
#define CONFIG_SF_DEFAULT_BUS		0
#define CONFIG_SF_DEFAULT_CS		0
#define CONFIG_SF_DEFAULT_SPEED		54000000

/* NAND flash */
#undef CONFIG_CMD_NAND

/* MMC */
#define CONFIG_CMD_MMC

#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_SDHCI
#define CONFIG_SUPPORT_EMMC_BOOT
#endif

/* Software I2C */
#define CONFIG_CMD_I2C

#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_SOFT
#define CONFIG_SYS_I2C_SOFT_SPEED       50000
#define CONFIG_SOFT_I2C_GPIO_SCL        GPIO_PIN_PD(5)
#define CONFIG_SOFT_I2C_GPIO_SDA        GPIO_PIN_PD(4)
#define CONFIG_AT24MAC_ADDR		0x5c
#define CONFIG_AT24MAC_REG		0x9a

/* USB */
#define CONFIG_CMD_USB

#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_ATMEL
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS	3
#define CONFIG_USB_STORAGE
#endif

/* USB device */
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_ATMEL_USBA
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_RNDIS
#define CONFIG_USBNET_MANUFACTURER      "Atmel SAMA5D2 XPlained"

#if defined(CONFIG_CMD_USB) || defined(CONFIG_CMD_MMC)
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif

/* Ethernet Hardware */
#define CONFIG_MACB
#define CONFIG_RMII
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_MACB_SEARCH_PHY

/* LCD */
/* #define CONFIG_LCD */

#ifdef CONFIG_LCD
#define LCD_BPP				LCD_COLOR16
#define LCD_OUTPUT_BPP                  24
#define CONFIG_LCD_LOGO
#define CONFIG_LCD_INFO
#define CONFIG_LCD_INFO_BELOW_LOGO
#define CONFIG_SYS_WHITE_ON_BLACK
#define CONFIG_ATMEL_HLCD
#define CONFIG_ATMEL_LCD_RGB565
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#endif

#ifdef CONFIG_SYS_USE_QSPIFLASH

/* u-boot env in serial flash, by default is bus 0 and cs 0 */
/* Override defaults in "at91-sama5_common.h" */
#undef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_SIZE
#undef CONFIG_ENV_SECT_SIZE
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTARGS

#define CONFIG_ENV_OFFSET		0x10000
#define CONFIG_ENV_SIZE			0x10000
#define CONFIG_ENV_SECT_SIZE		0x10000

#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_ENV_OFFSET_REDUND 	0x20000

#define CONFIG_BOOTCOMMAND      "sf probe; env set ethact usb_ether; run production_check; run bootcmd_flash; run bootcmd_usb_recursive"
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"base_bootargs="						\
		"console=ttyS0,115200 earlyprintk rootfstype=ramfs "	\
		"root=/dev/ram0 rw \0"					\
	"cdc_connect_timeout=120;\0"					\
	"bootcmd_flash="						\
		"setenv bootargs ${base_bootargs} ${mtdparts}; "	\
		"ubi part boot; "					\
		"ubifsmount ubi0:boot; "				\
		"ubifsload 0x21000000 uImage.dtb; "			\
		"ubifsload 0x22000000 uImage.bin; "			\
		"ubifsload 0x23000000 rootfs.cpio.gz.u-boot; "		\
		"bootm 0x22000000 0x23000000 0x21000000;\0"		\
	"bootcmd_usb="							\
                "echo Run from USB...; " 		                \
		"env set ethact usb_ether; "				\
		"setenv bootargs ${base_bootargs} ${mtdparts}; "	\
		"gpio set 33; "						\
		"usb start; "                                           \
		"tftp 0x21000000 matrix120-recovery/uImage.dtb; "       \
		"sleep ${usb_wait_t}; "                                 \
		"tftp 0x22000000 matrix120-recovery/uImage.bin; "       \
		"sleep ${usb_wait_t}; "                                 \
		"tftp 0x23000000 matrix120-recovery/rootfs.cpio.gz.u-boot; "\
		"usb stop; "                                            \
		"bootm 0x22000000 0x23000000 0x21000000; "		\
		"gpio set 35;\0"					\
        "bootcmd_usb_recursive="                                        \
		"run bootcmd_usb;"					\
		"run bootcmd_usb_recursive;\0"				\
	"ipaddr=192.168.0.1\0"				           	\
	"serverip=192.168.0.2\0"					\
	"netmask=255.255.255.0\0"					\
	"ethaddr=aa:bb:cc:dd:ee:ff\0"				\
	"usbnet_devaddr=aa:bb:cc:dd:ee:ff;\0"				\
	"usbnet_hostaddr=aa:bb:cc:dd:ee:ee;\0"				\
	"mtdparts="							\
		"mtdparts=f0020000.qspi:1M(u-boot),26M(boot),-(user)\0" \
	"mtdids="							\
		"nor0=f0020000.qspi\0"					\
	"load_boot_ubifs="                                              \
		"usb start; "                                           \
		"tftp 0x21000000 matrix120/boot.ubifs; "                \
		"usb stop;\0" 						\
	"load_user_ubifs="                                              \
		"usb start; "                                           \
		"tftp 0x21000000 matrix120/user.ubifs; "                \
		"usb stop;\0"						\
	"update_boot_ubifs="						\
		"env set filesize 0; run load_boot_ubifs; "		\
		"if test ${filesize} != 0 ; "				\
		"then "							\
			"sf erase 0x00100000 0x01A00000; "		\
			"sf write 0x21000000 0x00100000 ${filesize}; "	\
		"fi\0" 							\
	"update_user_ubifs="						\
		"env set filesize 0; run load_user_ubifs; "		\
		"if test ${filesize} != 0 ; "				\
		"then " 						\
			"sf erase 0x01B00000 0x00500000; "              \
			"sf write 0x21000000 0x01B00000 ${filesize}; "  \
		"fi\0" 							\
	"update_usb="                                                   \
		"run update_boot_ubifs; "				\
		"run update_user_ubifs; "				\
		"run bootcmd_flash\0"					\
	"usb_wait_t=3;\0"						\
	"production_file="						\
		"/AppData/BootInfo.txt\0"				\
	"production_check="								\
		"ubi part user; "							\
		"ubifsmount ubi0:user; "						\
		"setenv filesize ''; "							\
		"ubifsload 0x21000000 ${production_file}; "				\
		"ubifsumount; "								\
		"if test -n ${filesize}; then "						\
			"gpio set 37;"							\
			"echo Run production test suite...; "				\
			"usb start; "							\
			"tftp 0x21000000 matrix120-pts/uImage.dtb; "		\
			"sleep ${usb_wait_t}; "					\
			"tftp 0x22000000 matrix120-pts/uImage.bin; "		\
			"sleep ${usb_wait_t}; "					\
			"tftp 0x23000000 matrix120-pts/rootfs.cpio.gz.u-boot; "	\
			"usb stop; "							\
			"setenv bootargs ${base_bootargs} ${mtdparts}; "		\
			"bootm 0x22000000 0x23000000 0x21000000; "			\
			"gpio set 34;"							\
		"fi;\0"
#endif

/* SPL */
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_TEXT_BASE		0x200000
#define CONFIG_SPL_MAX_SIZE		0x10000
#define CONFIG_SPL_BSS_START_ADDR	0x20000000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000
#define CONFIG_SYS_SPL_MALLOC_START	0x20080000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x80000

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_GPIO_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT

#define CONFIG_SPL_BOARD_INIT
#define CONFIG_SYS_MONITOR_LEN		(512 << 10)

#ifdef CONFIG_SYS_USE_MMC
#define CONFIG_SPL_LDSCRIPT		arch/arm/mach-at91/armv7/u-boot-spl.lds
#define CONFIG_SPL_MMC_SUPPORT
#define CONFIG_SYS_U_BOOT_MAX_SIZE_SECTORS	0x400
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR 0x200
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION	1
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME		"u-boot.img"
#define CONFIG_SPL_FAT_SUPPORT
#define CONFIG_SPL_LIBDISK_SUPPORT

#elif CONFIG_SYS_USE_SERIALFLASH
#define CONFIG_SPL_SPI_SUPPORT
#define CONFIG_SPL_SPI_FLASH_SUPPORT
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x8000

/* UBIFS */
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_LZO

#endif

#endif
