/*
 * K2G: SoC definitions
 *
 * (C) Copyright 2015
 *     Texas Instruments Incorporated, <www.ti.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __ASM_ARCH_HARDWARE_K2G_H
#define __ASM_ARCH_HARDWARE_K2G_H

#define KS2_NUM_DSPS	0

/* Power and Sleep Controller (PSC) Domains */
#define KS2_LPSC_ALWAYSON		0
#define KS2_LPSC_PMMC			1
#define KS2_LPSC_DEBUG			2
#define KS2_LPSC_NSS			3
#define KS2_LPSC_SA			4
#define KS2_LPSC_TERANET		5
#define KS2_LPSC_SYS_COMP		6
#define KS2_LPSC_QSPI			7
#define KS2_LPSC_MMC			8
#define KS2_LPSC_GPMC			9
#define KS2_LPSC_MLB			11
#define KS2_LPSC_EHRPWM			12
#define KS2_LPSC_EQEP			13
#define KS2_LPSC_ECAP			14
#define KS2_LPSC_MCASP			15
#define KS2_LPSC_SR			16
#define KS2_LPSC_MSMC			17
#define KS2_LPSC_GEM			18
#define KS2_LPSC_ARM			19
#define KS2_LPSC_ASRC			20
#define KS2_LPSC_ICSS			21
#define KS2_LPSC_DSS			23
#define KS2_LPSC_PCIE			24
#define KS2_LPSC_USB_0			25
#define KS2_LPSC_USB			KS2_LPSC_USB_0
#define KS2_LPSC_USB_1			26
#define KS2_LPSC_DDR3			27
#define KS2_LPSC_SPARE0_LPSC0		28
#define KS2_LPSC_SPARE0_LPSC1		29
#define KS2_LPSC_SPARE1_LPSC0		30
#define KS2_LPSC_SPARE1_LPSC1		31

#define KS2_LPSC_CPGMAC			KS2_LPSC_NSS
#define KS2_LPSC_CRYPTO			KS2_LPSC_SA

#endif /* __ASM_ARCH_HARDWARE_K2G_H */