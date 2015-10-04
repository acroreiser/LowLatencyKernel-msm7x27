/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora Forum nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * Alternatively, provided that this notice is retained in full, this software
 * may be relicensed by the recipient under the terms of the GNU General Public
 * License version 2 ("GPL") and only version 2, in which case the provisions of
 * the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
 * software under the GPL, then the identification text in the MODULE_LICENSE
 * macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
 * recipient changes the license terms to the GPL, subsequent recipients shall
 * not relicense under alternate licensing terms, including the BSD or dual
 * BSD/GPL terms.  In addition, the following license statement immediately
 * below and between the words START and END shall also then apply when this
 * software is relicensed under the GPL:
 *
 * START
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 and only version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * END
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <linux/delay.h>
#include <mach/gpio.h>
#include "msm_fb.h"

#define LCDC_DEBUG

#ifdef LCDC_DEBUG
#define DPRINT(x...)	printk("s6d04h0a " x)
#else
#define DPRINT(x...)	
#endif

#define __LCD_CONTROL_BY_FILE__

#define GPIO_BL_CTRL	26
#define GPIO_LCD_DET	94

/*
 * Serial Interface
 */
#define LCD_CSX_HIGH	gpio_set_value(spi_cs, 1);
#define LCD_CSX_LOW		gpio_set_value(spi_cs, 0);

#define LCD_SCL_HIGH	gpio_set_value(spi_sclk, 1);
#define LCD_SCL_LOW		gpio_set_value(spi_sclk, 0);

#define LCD_SDI_HIGH	gpio_set_value(spi_sdi, 1);
#define LCD_SDI_LOW		gpio_set_value(spi_sdi, 0);

#define DEFAULT_USLEEP	1	

// #define LCD_DET_ENABLE   // minhyodebug

#ifdef LCD_DET_ENABLE
#include <linux/irq.h>

struct s6d04h0a_lcd_det_data {
	int             irq;
	struct work_struct work_lcd_det;  /* for lcd_det */
};

struct s6d04h0a_lcd_det_data lcd_det_data;
struct workqueue_struct *lcd_det_wq;
struct platform_device *plcdc_s6d04h0a_panel_device;
#endif

#if 0 // CALLISTO-REV00A, minhyo
#define WRPWD			0xF0
#define POWCTL			0xF3
#define EDSTEST			0xFF
#define VCMCTL			0xF4
#define SRCCTL			0xF5
#define SLPOUT			0x11
#define MADCTL			0x36
#define COLMOD			0x3A
#define DISCTL			0xF2
#define IFCTL			0xF6
#define GATECTL			0xFD
#define WRDISBV			0x51
#define WRCABCMB		0x5E
#define MIECTL1			0xCA
#define BCMODE			0xCB
#define MIECTL2			0xCC
#define MIDCTL3			0xCD
#define RPGAMCTL		0xF7
#define RNGAMCTL		0xF8
#define GPGAMCTL		0xF9
#define GNGAMCTL		0xFA
#define BPGAMCTL		0xFB
#define BNGAMCTL		0xFC
#define CASET			0x2A
#define PASET			0x2B
#define RAMWR           0x2C
#define WRCTRLD			0x53
#define WRCABC			0x55
#define DISPON			0x29
#define DISPOFF			0x28
#define SLPIN			0x10
#endif

struct setting_table {
	unsigned char command;	
	unsigned char parameters;
	unsigned char parameter[31]; // 31 > 90 > 45
	long wait;
};


extern int board_hw_revision;	// 20101005 hongkuk.son for LCD 180 degree rotate

static struct setting_table power_on_setting_table[] = {
	// Level2 command access
	{	0xF0,	2,	{	0x5A, 0x5A, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	// Power Setting Sequence
	{	0xF4,	11,	{	0x0B, 0x00, 0x00, 0x00, 0x22, 
						0x67, 0x04, 0x02, 0x2A, 0x75, 
						0x05, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0xF5,	6,	{	0x00, 0x6F, 0x5A, 0x00, 0x00, 
						0x07, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0xF6,	9,	{	0x03, 0x01, 0x07, 0x00, 0x02, 
						0x0C, 0x02, 0x08, 0x07, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00 	},	0 },

	// Initializing Sequence
	{	0x35,	1,	{	0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0x36,	1,	{	0x48, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0x3A,	1,	{	0x66, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0xF7,	4,	{	0x02, 0x01, 0x70, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00 	},	0 },
						
	{	0xF2,	12,	{	0x28, 0x65, 0x03, 0x08, 0x08, 
						0x00, 0x00, 0x19, 0x48, 0x04, 
						0x03, 0x01, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },

	// Gamma Setting Sequence
	{	0xF9,	1,	{	0x04, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0xFA,	16,	{	0x00, 0x00, 0x00, 0x00, 0x2A, 
						0x3F, 0x31, 0x20, 0x20, 0x20, 
						0x20, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	
	
	{	0xFB,	16,	{	0x00, 0x00, 0x1D, 0x1D, 0x1D, 
						0x20, 0x0E, 0x3F, 0x2A, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	
	
	{	0xF9,	1,	{	0x02, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0xFA,	16,	{	0x00, 0x00, 0x00, 0x00, 0x2A, 
						0x3F, 0x31, 0x20, 0x20, 0x20, 
						0x20, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	
	
	{	0xFB,	16,	{	0x00, 0x00, 0x1D, 0x1D, 0x1D, 
						0x20, 0x0E, 0x3F, 0x2A, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	
	
	{	0xF9,	1,	{	0x01, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	{	0xFA,	16,	{	0x00, 0x00, 0x00, 0x00, 0x2A, 
						0x3F, 0x31, 0x20, 0x20, 0x20, 
						0x20, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	
	
	{	0xFB,	16,	{	0x00, 0x00, 0x1D, 0x1D, 0x1D, 
						0x20, 0x0E, 0x3F, 0x2A, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

	// Level2 command blocking
	{	0xF0,	2,	{	0xA5, 0xA5, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	
	// Sleep out
	{	0x11,	0,	{	0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	160 },	

	{	0x29,	0,	{	0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00	},	0 },	

};

#if 0
static struct setting_table power_on_setting_table[] = {
//	Level2 command access
	{    0xF0, 2, { 0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF1, 2, { 0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	Power Setting Sequence
	{    0xF4, 14, { 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x75, 0x03, 0x00, 0x52, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xF5, 12, { 0x00, 0x56, 0x55, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x2A, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
	{    0xF5, 12, { 0x00, 0x47, 0x75, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x2A, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	Initializing Sequence
	{    0x3A, 1, { 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF2, 19, { 0x3B, 0x38, 0x03, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x54, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF6, 8, { 0x00, 0x00, 0x05, 0x03, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xF7, 5, { 0x48, 0x01, 0xF0, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
	{    0xF7, 5, { 0x48, 0x81, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF8, 2, { 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	Gamma Setting
	{    0xF9, 1, { 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x27, 0x29, 0x2A, 0x29, 0x1E, 0x2D, 0x35, 0x3D, 0x00, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x2F, 0x3F, 0x1F, 0x2A, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//
	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x2F, 0x3F, 0x3F, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//mod
	{    0xF9, 1, { 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x27, 0x29, 0x2A, 0x29, 0x1E, 0x2D, 0x35, 0x3D, 0x3F, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x28, 0x38, 0x1A, 0x20, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//
	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x28, 0x38, 0x3A, 0x30, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//mod
	{    0xF9, 1, { 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x27, 0x37, 0x35, 0x33, 0x17, 0x27, 0x32, 0x37, 0x3F, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x2A, 0x2E, 0x17, 0x22, 0x25, 0x30, 0x1A, 0x2F, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//
	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x2A, 0x2E, 0x17, 0x22, 0x25, 0x30, 0x3A, 0x37, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//mod

	{    0x35, 1, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//TEON			//
	{    0x2B, 4, { 0x00, 0x00, 0x01, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//PASET			//
	{    0x2A, 4, { 0x00, 0x00, 0x01, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//CASET			//
	{    0x2C, 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//RAMWR			//


//	Sleep out
	{    0x11, 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 120 },
//	Display on
	{    0x29, 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	BACKLIGHT SETTING and CONTROL	
	{    0x53, 1, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
	{    0x51, 1, { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
};

static struct setting_table power_on_setting_table_hw2[] = {
//	Level2 command access
	{    0xF0, 2, { 0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF1, 2, { 0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	Power Setting Sequence
	{    0xF4, 14, { 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x75, 0x03, 0x00, 0x52, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xF5, 12, { 0x00, 0x56, 0x55, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x2A, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
	{    0xF5, 12, { 0x00, 0x47, 0x75, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x2A, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	Initializing Sequence
	{    0x3A, 1, { 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF2, 19, { 0x3B, 0x38, 0x03, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x54, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF6, 8, { 0x00, 0x00, 0x05, 0x03, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xF7, 5, { 0x48, 0x01, 0xF0, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
	{    0xF7, 5, { 0x48, 0x81, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
	{    0xF8, 2, { 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	Gamma Setting
	{    0xF9, 1, { 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x27, 0x29, 0x2A, 0x29, 0x1E, 0x2D, 0x35, 0x3D, 0x00, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x2F, 0x3F, 0x1F, 0x2A, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//
	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x2F, 0x3F, 0x3F, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//mod
	{    0xF9, 1, { 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x27, 0x29, 0x2A, 0x29, 0x1E, 0x2D, 0x35, 0x3D, 0x3F, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x28, 0x38, 0x1A, 0x20, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//
	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x23, 0x27, 0x1D, 0x28, 0x28, 0x38, 0x3A, 0x30, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//mod
	{    0xF9, 1, { 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//
//	{    0xFA, 16, { 0x00, 0x00, 0x0A, 0x27, 0x37, 0x35, 0x33, 0x17, 0x27, 0x32, 0x37, 0x3F, 0x35, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x2A, 0x2E, 0x17, 0x22, 0x25, 0x30, 0x1A, 0x2F, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//
	{    0xFA, 16, { 0x32, 0x00, 0x0A, 0x22, 0x1E, 0x2A, 0x2E, 0x17, 0x22, 0x25, 0x30, 0x3A, 0x37, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//				//mod

	{    0x35, 1, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//TEON			//
	{    0x2B, 4, { 0x00, 0x00, 0x01, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//PASET			//
	{    0x2A, 4, { 0x00, 0x00, 0x01, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//CASET			//
	{    0x2C, 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//RAMWR			//

	{    0x36, 1, { 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },	//MADCTL


//	Sleep out
	{    0x11, 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 120 },
//	Display on
	{    0x29, 0, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
//	BACKLIGHT SETTING and CONTROL	
	{    0x53, 1, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
	{    0x51, 1, { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },
};
#endif
static struct setting_table power_off_setting_table[] = {
//	Sleep In
	{ 0x10, 0,  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 120 },
};

/* 20100824 hongkuk.son */
static struct setting_table backlight_setting_table[] = {
	{    0x51, 1, { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0 },//DISPON
};
/***/

#define POWER_ON_SETTINGS	(int)(sizeof(power_on_setting_table)/sizeof(struct setting_table))
#define POWER_ON_SETTINGS_HW2	(int)(sizeof(power_on_setting_table_hw2)/sizeof(struct setting_table))
#define POWER_OFF_SETTINGS	(int)(sizeof(power_off_setting_table)/sizeof(struct setting_table))

static int lcdc_s6d04h0a_panel_off(struct platform_device *pdev);

static int lcd_brightness = -1;

static int spi_cs;
static int spi_sclk;
//static int spi_sdo;
static int spi_sdi;
//static int spi_dac;
static int lcd_en;
static int lcd_reset;

#if 0
static unsigned char bit_shift[8] = { (1 << 7),	/* MSB */
	(1 << 6),
	(1 << 5),
	(1 << 4),
	(1 << 3),
	(1 << 2),
	(1 << 1),
	(1 << 0)		               /* LSB */
};
#endif

static int delayed_backlight_value = -1;
static void lcdc_s6d04h0a_set_brightness_in_blu(int level);

struct s6d04h0a_state_type{
	boolean disp_initialized;
	boolean display_on;
	boolean disp_powered_up;
};

static struct s6d04h0a_state_type s6d04h0a_state = { 
	.disp_initialized = TRUE,
	.display_on = TRUE,
	.disp_powered_up = TRUE,
};

static struct msm_panel_common_pdata *lcdc_s6d04h0a_pdata;

static void setting_table_write(struct setting_table *table)
{
	long i, j;

	LCD_CSX_HIGH
	udelay(DEFAULT_USLEEP);
	LCD_SCL_HIGH 
	udelay(DEFAULT_USLEEP);

	/* Write Command */
	LCD_CSX_LOW
	udelay(DEFAULT_USLEEP);
	LCD_SCL_LOW 
	udelay(DEFAULT_USLEEP);		
	LCD_SDI_LOW 
	udelay(DEFAULT_USLEEP);
	
	LCD_SCL_HIGH 
	udelay(DEFAULT_USLEEP); 

   	for (i = 7; i >= 0; i--) { 
		LCD_SCL_LOW
		udelay(DEFAULT_USLEEP);
		if ((table->command >> i) & 0x1)
			LCD_SDI_HIGH
		else
			LCD_SDI_LOW
		udelay(DEFAULT_USLEEP);	
		LCD_SCL_HIGH
		udelay(DEFAULT_USLEEP);	
	}

	LCD_CSX_HIGH
	udelay(DEFAULT_USLEEP);	

	/* Write Parameter */
	if ((table->parameters) > 0) {
	for (j = 0; j < table->parameters; j++) {
		LCD_CSX_LOW 
		udelay(DEFAULT_USLEEP); 	
		
		LCD_SCL_LOW 
		udelay(DEFAULT_USLEEP); 	
		LCD_SDI_HIGH 
		udelay(DEFAULT_USLEEP);
		LCD_SCL_HIGH 
		udelay(DEFAULT_USLEEP); 	

		for (i = 7; i >= 0; i--) { 
			LCD_SCL_LOW
			udelay(DEFAULT_USLEEP);	
			if ((table->parameter[j] >> i) & 0x1)
				LCD_SDI_HIGH
			else
				LCD_SDI_LOW
			udelay(DEFAULT_USLEEP);	
			LCD_SCL_HIGH
			udelay(DEFAULT_USLEEP);					
		}

			LCD_CSX_HIGH
			udelay(DEFAULT_USLEEP);	
	}
	}

	mdelay(table->wait);
}

static void spi_init(void)
{
	/* Setting the Default GPIO's */
	spi_sclk = *(lcdc_s6d04h0a_pdata->gpio_num);
	spi_cs   = *(lcdc_s6d04h0a_pdata->gpio_num + 1);
	spi_sdi  = *(lcdc_s6d04h0a_pdata->gpio_num + 2);
	lcd_en   = *(lcdc_s6d04h0a_pdata->gpio_num + 3);
	lcd_reset= *(lcdc_s6d04h0a_pdata->gpio_num + 4);
//	spi_sdo  = *(lcdc_s6d04h0a_pdata->gpio_num + 3);

	/* Set the output so that we dont disturb the slave device */
	gpio_set_value(spi_sclk, 0);
	gpio_set_value(spi_sdi, 0);

	/* Set the Chip Select De-asserted */
	gpio_set_value(spi_cs, 0);

}

static void s6d04h0a_disp_powerup(void)
{
	DPRINT("start %s\n", __func__);	

	if (!s6d04h0a_state.disp_powered_up && !s6d04h0a_state.display_on) {
		/* Reset the hardware first */
		//LCD_RESET_N_HI
		gpio_set_value(lcd_reset, 1);

		//LCD_EN_HI
		//gpio_set_value(lcd_en, 1);
		//TODO: turn on ldo
		mdelay(50);
		
		//LCD_RESET_N_LO
		gpio_set_value(lcd_reset, 0);
		mdelay(100);
		//LCD_RESET_N_HI
		gpio_set_value(lcd_reset, 1);
		mdelay(10);

		
		/* Include DAC power up implementation here */
		
	    s6d04h0a_state.disp_powered_up = TRUE;
	}
}

static void s6d04h0a_disp_powerdown(void)
{
	DPRINT("start %s\n", __func__);	

		/* Reset Assert */
		gpio_set_value(lcd_reset, 0);		

		/* turn off LDO */
		//TODO: turn off LDO
		//gpio_set_value(lcd_en, 0);

	    s6d04h0a_state.disp_powered_up = FALSE;
}

static void s6d04h0a_init(void)
{
#if 0
	/* Image interface settings */
	serigo(GORDON_REG_IMGCTL2, 0x00);
	serigo(GORDON_REG_IMGSET1, 0x00);

	/* Exchange the RGB signal for J510(Softbank mobile) */
	serigo(GORDON_REG_IMGSET2, 0x12);
	serigo(GORDON_REG_LCDIFSET1, 0x00);

	/* Pre-charge settings */
	serigo(GORDON_REG_PCCTL, 0x09);
	serigo(GORDON_REG_LCDIFCTL2, 0x7B);
#endif
	mdelay(1);
}

static void s6d04h0a_disp_on(void)
{
	int i;

	DPRINT("start %s\n", __func__);	

//	if (s6d04h0a_state.disp_powered_up && !s6d04h0a_state.display_on) {
		s6d04h0a_init();
		mdelay(50);
		for (i = 0; i < POWER_ON_SETTINGS; i++)
			setting_table_write(&power_on_setting_table[i]);		

		mdelay(1);
		s6d04h0a_state.display_on = TRUE;
//	}
}

static int lcdc_s6d04h0a_panel_on(struct platform_device *pdev)
{
	DPRINT("start %s\n", __func__);	

/* 20100824 hongkuk.son */
#if 0
	if (!s6d04h0a_state.disp_initialized) {
#endif
/***/
		/* Configure reset GPIO that drives DAC */
		lcdc_s6d04h0a_pdata->panel_config_gpio(1);
		//spi_dac = *(lcdc_s6d04h0a_pdata->gpio_num + 4);
		//gpio_set_value(spi_dac, 0);
		//udelay(15);
		//gpio_set_value(spi_dac, 1);
		s6d04h0a_disp_powerup();
		spi_init();	/* LCD needs SPI */
		s6d04h0a_disp_on();

		mdelay(50);
		s6d04h0a_state.disp_initialized = TRUE;
/* 20100824 hongkuk.son */
#if 0
	}
#endif
/***/

#ifdef LCD_DET_ENABLE
	/* enable INT */
	enable_irq(lcd_det_data.irq);	
#endif
	
	// TEMP CODE for BLU
	//gpio_set_value(GPIO_BL_CTRL, 1);
	//gpio_set_value(91, 1); 
	
	return 0;
}

static int lcdc_s6d04h0a_panel_off(struct platform_device *pdev)
{
	int i;

	DPRINT("start %s\n", __func__);	

	// TEMP CODE for BLU
	//gpio_set_value(GPIO_BL_CTRL, 0);
	//gpio_set_value(91, 0); 

#ifdef LCD_DET_ENABLE
	disable_irq(lcd_det_data.irq);
#endif

	if (s6d04h0a_state.disp_powered_up && s6d04h0a_state.display_on) {
		
		for (i = 0; i < POWER_OFF_SETTINGS; i++)
			setting_table_write(&power_off_setting_table[i]);	
		
		lcdc_s6d04h0a_pdata->panel_config_gpio(0);
		s6d04h0a_state.display_on = FALSE;
		s6d04h0a_state.disp_initialized = FALSE;
		mdelay(120);
		s6d04h0a_disp_powerdown();
	}
	return 0;
}

#if 1

// brightness tuning
#define MAX_BRIGHTNESS_LEVEL 255
#define LOW_BRIGHTNESS_LEVEL 30
#define MAX_BACKLIGHT_VALUE 58 // 59	// minhyo0614 64
#define LOW_BACKLIGHT_VALUE 12 // 13  // minhyo0614 3
#define DIM_BACKLIGHT_VALUE 7 // 8   // minhyo0614 1 

static DEFINE_SPINLOCK(bl_ctrl_lock);
static DEFINE_SPINLOCK(bl_ctrl_irq_lock);

// brightness setting for KTD259 (Backlight IC)
#define MAX_BRIGHTNESS_IN_BLU	64 // minhyo0611 32

static void lcdc_s6d04h0a_set_brightness_in_blu(int level)
{
	unsigned long irqflags;
	int tune_level = level;
	int pulse;

//	printk("================minhyodebug: lcdc_s6d04h0a_set_brightness_in_blu level=%d\n",level);
	// LCD should be turned on prior to backlight
	if(s6d04h0a_state.disp_initialized == FALSE && tune_level > 0)
	{
		//DPRINT("skip bl:%d\n", tune_level);	
		delayed_backlight_value = tune_level;
//		printk("================minhyodebug: lcdc_s6d04h0a_set_brightness_in_blu A\n");
		return;
	}
	else
	{
		//DPRINT("set bl:%d\n", tune_level);	
		delayed_backlight_value = -1;
	}

	//sm.kim: 20100331
	// when LCD is absent, don't turn on BLU
	// over current may distroy inductor element
#if 0//1
	if(gpio_get_value(GPIO_LCD_DET) == 1 && tune_level > 0)
		{
		DPRINT("LCD may be absent. skip bl.\n");
		return;
		}
#endif

	//TODO: lock
	spin_lock(&bl_ctrl_lock);

	if (tune_level <= 0) 
	{
		/* keep back light OFF */
		gpio_set_value(GPIO_BL_CTRL, 0);
		mdelay(3);		//guaranteed shutdown
		lcd_brightness = tune_level;
	} 
	else 
	{
		/* keep back light ON */
		if(unlikely(lcd_brightness < 0))
		{
			// first time
			int val = gpio_get_value(GPIO_BL_CTRL);
			//printk("================minhyodebug: lcdc_s6d04h0a_set_brightness_in_blu val=%d\n", val);
			if(val)
			{
				lcd_brightness = 0;
				gpio_set_value(GPIO_BL_CTRL, 0);
				mdelay(3);		//guaranteed shutdown
			}
		}

		if(!lcd_brightness)
		{
			//printk("================minhyodebug: lcdc_s6d04h0a_set_brightness_in_blu B\n");
			gpio_set_value(GPIO_BL_CTRL, 1);
			udelay(3); 
			lcd_brightness = MAX_BRIGHTNESS_IN_BLU;
		}
			
		pulse = (lcd_brightness - tune_level + MAX_BRIGHTNESS_IN_BLU) % MAX_BRIGHTNESS_IN_BLU;

		spin_lock_irqsave(&bl_ctrl_irq_lock, irqflags);

/* 20100824 hongkuk.son */
#if 0
		for(; pulse>0; pulse--)
		{
		//	printk("pulse # %d\n", pulse);
			gpio_set_value(GPIO_BL_CTRL, 0);
			udelay(3);
			gpio_set_value(GPIO_BL_CTRL, 1);
			udelay(3);
		}
#endif
		backlight_setting_table[0].parameter[0] = (unsigned char) level;
//		if( lcd_brightness != level )
//			setting_table_write ( &backlight_setting_table[0] );
//		mdelay ( 60 );
//		msleep ( 60 );
		lcd_brightness = level;
/***/
				
		spin_unlock_irqrestore(&bl_ctrl_irq_lock, irqflags);
		lcd_brightness = tune_level;
	
		}

//	printk("================minhyodebug: lcdc_s6d04h0a_set_brightness_in_blu Z\n");
	
	//TODO: unlock
	spin_unlock(&bl_ctrl_lock);

}

static void lcdc_s6d04h0a_set_backlight(struct msm_fb_data_type *mfd)
{	
/* 20100824 hongkuk.son */
#if 0	
	int bl_level = mfd->bl_level;
	int tune_level;

	// brightness tuning
	if(bl_level >= LOW_BRIGHTNESS_LEVEL)
		tune_level = (bl_level - LOW_BRIGHTNESS_LEVEL) * (MAX_BACKLIGHT_VALUE-LOW_BACKLIGHT_VALUE) / (MAX_BRIGHTNESS_LEVEL-LOW_BRIGHTNESS_LEVEL) + LOW_BACKLIGHT_VALUE;
	else if(bl_level > 0)
		tune_level = DIM_BACKLIGHT_VALUE;
	else
		tune_level = bl_level;

	DPRINT("%s:%d,%d\n", __func__, bl_level, tune_level);	

	// turn on lcd if needed
#ifdef __LCD_ON_EARLY__
	if(tune_level > 0)
		{
		if(!s6d04h0a_state.disp_powered_up)
			s6d04h0a_disp_powerup();
		if(!s6d04h0a_state.display_on)
			s6d04h0a_disp_on();
		}
#endif

	lcdc_s6d04h0a_set_brightness_in_blu(MAX_BRIGHTNESS_LEVEL - tune_level);
#endif
	DPRINT("%s:%d,%d\n", __func__, mfd->bl_level);
	lcdc_s6d04h0a_set_brightness_in_blu ( mfd->bl_level );
/***/
}
#endif


#ifdef __LCD_CONTROL_BY_FILE__
static int s3cfb_sysfs_show_lcd_power(struct device *dev, struct device_attribute *attr, char *buf)
{
        return snprintf(buf, PAGE_SIZE, "%d\n", s6d04h0a_state.disp_initialized);
}

static int s3cfb_sysfs_store_lcd_power(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	struct platform_device* pdev;
	
    if (len < 1)
        return -EINVAL;

	pdev = container_of(dev, struct platform_device, dev);

    if (strnicmp(buf, "on", 2) == 0 || strnicmp(buf, "1", 1) == 0)
    	{
		lcdc_s6d04h0a_panel_on(pdev);
    	lcdc_s6d04h0a_set_brightness_in_blu(15);
    	}
    else if (strnicmp(buf, "off", 3) == 0 || strnicmp(buf, "0", 1) == 0)
    	{
    	lcdc_s6d04h0a_set_brightness_in_blu(0);
		lcdc_s6d04h0a_panel_off(pdev);
    	}
    else
            return -EINVAL;

    return len;
}

static DEVICE_ATTR(lcd_power, 0666,						//sm.kim: give write permission for application
                        s3cfb_sysfs_show_lcd_power,
                        s3cfb_sysfs_store_lcd_power);
#endif

#ifdef LCD_DET_ENABLE // minhyo0614

static irqreturn_t lcd_det_irq_handler(int irq, void *dev_id)
{
	struct s6d04h0a_lcd_det_data *lcd_det = dev_id;

	printk("[LCD] lcd_det->irq = %d\n",lcd_det->irq);

	if(lcd_det->irq !=-1)
	{
		disable_irq(lcd_det->irq);
		printk("[LCD] disable_irq \n");

		queue_work(lcd_det_wq, &lcd_det->work_lcd_det);

	}
	
	printk("[LCD] IRQ_HANDLED \n");

	return IRQ_HANDLED;
}

static void lcd_det_work_func(struct work_struct *work)
{
	/* LCD init */
	lcdc_s6d04h0a_panel_off(plcdc_s6d04h0a_panel_device);
	mdelay(2);
	lcdc_s6d04h0a_panel_on(plcdc_s6d04h0a_panel_device);

	/* enable INT */
//	enable_irq(lcd_det->irq);
}

#endif

static int __devinit s6d04h0a_probe(struct platform_device *pdev)
{
	int ret;
	
	printk("++++++%s\n", __func__);
	printk("++++++pdev->id=%d\n", pdev->id);


#ifdef __LCD_CONTROL_BY_FILE__
    ret = device_create_file(&(pdev->dev), &dev_attr_lcd_power);
    if (ret < 0)
            printk(KERN_WARNING "s6d04h0a: failed to add entries\n");
#endif

	if (pdev->id == 0)
	{
		lcdc_s6d04h0a_pdata = pdev->dev.platform_data;    // lcdc_s6d04h0a_panel_data in board-msm7x27.c

		#ifdef LCD_DET_ENABLE
		{ 
			int config, irq, err;

			plcdc_s6d04h0a_panel_device = pdev;

			/* work queue setting */
			lcd_det_wq= create_singlethread_workqueue("lcd_det_wq");
			if (!lcd_det_wq)
				return -ENOMEM;
			INIT_WORK(&lcd_det_data.work_lcd_det, lcd_det_work_func);
			printk("Workqueue Settings complete\n");

			/* gpio config */
			config = GPIO_CFG(GPIO_LCD_DET, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);
			err = gpio_tlmm_config(config, GPIO_CFG_ENABLE);
			if (err) 
				printk(KERN_ERR "%s: gpio_tlmm_config(%d)=%d\n", __func__, (int)GPIO_LCD_DET, err);
			
			/* INT setting */
			irq = gpio_to_irq(GPIO_LCD_DET);
			lcd_det_data.irq = -1;
			set_irq_type(irq, IRQ_TYPE_EDGE_RISING);
			err = request_irq(irq, lcd_det_irq_handler, IRQF_DISABLED, "lcd_det_int", &lcd_det_data);
			if (err)
			{
				printk("%s: request_irq failed for lcd_det\n", __func__);
				return 0;
			}
			lcd_det_data.irq = irq;
			printk("INT Settings complete\n");
		}
		#endif
		
		return 0;
	}

	printk("------%s\n", __func__);

	
	msm_fb_add_device(pdev);

	return 0;
}

static void s6d04h0a_shutdown(struct platform_device *pdev)
{
	DPRINT("start %s\n", __func__);	

   	lcdc_s6d04h0a_set_brightness_in_blu(0);
	lcdc_s6d04h0a_panel_off(pdev);
}

static struct platform_driver this_driver = {
	.probe  = s6d04h0a_probe,
	.shutdown	= s6d04h0a_shutdown,
	.driver = {
		.name   = "lcdc_s6d04h0a_qvga",
	},
};

static struct msm_fb_panel_data s6d04h0a_panel_data = {
	.on = lcdc_s6d04h0a_panel_on,
	.off = lcdc_s6d04h0a_panel_off,
	.set_backlight = lcdc_s6d04h0a_set_backlight,
};

static struct platform_device this_device = {
	.name   = "lcdc_panel",
	.id	= 1,
	.dev	= {
		.platform_data = &s6d04h0a_panel_data,
	}
};

//
//if you change screen resolution, you should modify same information in hardware/msm7k/libgralloc-qsd8k/framebuffer.cpp
//

#define LCDC_FB_XRES	240
#define LCDC_FB_YRES	320

#define LCDC_HBP		16//3//22
#define LCDC_HPW		5//3//14
#define LCDC_HFP		16//3//14
#define LCDC_VBP		6//8
#define LCDC_VPW		2//2
#define LCDC_VFP		8//8//2

#define LCDC_PCLK		(LCDC_FB_XRES + LCDC_HBP + LCDC_HPW + LCDC_HFP) * (LCDC_FB_YRES + LCDC_VBP + LCDC_VPW + LCDC_VFP) * 2

static int __init lcdc_s6d04h0a_panel_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;

	printk("++++++%s\n", __func__);

#ifdef CONFIG_FB_MSM_TRY_MDDI_CATCH_LCDC_PRISM
	if (msm_fb_detect_client("lcdc_s6d04h0a_qvga"))
		{
		printk(KERN_ERR "%s: msm_fb_detect_client failed!\n", __func__); 
		return 0;
		}
#endif
	
	ret = platform_driver_register(&this_driver);
	if (ret)
		{
		printk(KERN_ERR "%s: platform_driver_register failed! ret=%d\n", __func__, ret); 
		return ret;
		}

	pinfo = &s6d04h0a_panel_data.panel_info;
	pinfo->xres = LCDC_FB_XRES;
	pinfo->yres = LCDC_FB_YRES;
	pinfo->height = 70; // minhyo-rgb888
	pinfo->width = 42;  // minhyo-rgb888
	pinfo->type = LCDC_PANEL;
	pinfo->pdest = DISPLAY_1;
	pinfo->wait_cycle = 0;
//	pinfo->bpp = 24;
	pinfo->bpp = 18;
	pinfo->fb_num = 2;
	pinfo->clk_rate = (8384 * 1000);
	pinfo->bl_max = 255;
	pinfo->bl_min = 1;

	pinfo->lcdc.h_back_porch = LCDC_HBP;
	pinfo->lcdc.h_front_porch = LCDC_HFP;
	pinfo->lcdc.h_pulse_width = LCDC_HPW;
	pinfo->lcdc.v_back_porch = LCDC_VBP;
	pinfo->lcdc.v_front_porch = LCDC_VFP;
	pinfo->lcdc.v_pulse_width = LCDC_VPW;
	pinfo->lcdc.border_clr = 0;     /* blk */
	pinfo->lcdc.underflow_clr = 0xff;       /* blue */
	pinfo->lcdc.hsync_skew = 0;

	ret = platform_device_register(&this_device);
	if (ret)
		{
		printk(KERN_ERR "%s: platform_device_register failed! ret=%d\n", __func__, ret); 
		platform_driver_unregister(&this_driver);
		}

	printk("--%s\n", __func__);

	return ret;
}

module_init(lcdc_s6d04h0a_panel_init);
