#ifndef __MACH_BOARD_H
#define __MACH_BOARD_H

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/notifier.h>
#include <asm/setup.h>
#include <plat/board.h>
#include <mach/sram.h>
#include <linux/i2c-gpio.h>

/* adc battery */
#ifdef CONFIG_BATTERY_RK30_ADC
struct rk30_adc_battery_platform_data {
        int (*io_init)(void);
        int (*io_deinit)(void);

        int dc_det_pin;
        int batt_low_pin;
        int charge_ok_pin;
        int charge_set_pin;

//        int adc_channel;

        int dc_det_level;
        int batt_low_level;
        int charge_ok_level;
        int charge_set_level;
};
#endif

#if defined (CONFIG_INPUT_LIGHTSENSOR_BPW16N)
/*lightsensor bpw16n */
struct bpw16n_platform_data {
       int DATA_ADC_CHN;
};
#endif

#if defined (CONFIG_TOUCHSCREEN_CT36X)
struct ct36x_ts_info_platform {
	int irq;
	int rst;
};
#endif

#ifndef _LINUX_WLAN_PLAT_H_
struct wifi_platform_data {
        int (*set_power)(int val);
        int (*set_reset)(int val);
        int (*set_carddetect)(int val);
        void *(*mem_prealloc)(int section, unsigned long size);
        int (*get_mac_addr)(unsigned char *buf);
};
#endif

#if defined (CONFIG_TOUCHSCREEN_FT5x0x)
struct ft5x0x_ts_platform_data{
	  u16     model;
    int     (*get_pendown_state)(void);
    int     (*init_platform_hw)(void);
    int     (*platform_sleep)(void);
    int     (*platform_wakeup)(void);
    void    (*exit_platform_hw)(void);
};
#endif

#if defined (CONFIG_TOUCHSCREEN_FT5306)
struct ft5x0x_platform_data{
	  u16     model;
    int     (*get_pendown_state)(void);
    int     (*init_platform_hw)(void);
    int     (*ft5x0x_platform_sleep)(void);
    int     (*ft5x0x_platform_wakeup)(void);
    void    (*exit_platform_hw)(void);
};
#endif

#if defined (CONFIG_TOUCHSCREEN_ELAN_TP)
struct ktf2k_platform_data{
	  u16     model;
    int     (*get_pendown_state)(void);
    int     (*init_platform_hw)(void);
    int     (*ktf2k_platform_sleep)(void);
    int     (*ktf2k_platform_wakeup)(void);
    void    (*exit_platform_hw)(void);
    int			rst_pin;
};
#endif

#if defined (CONFIG_TOUCHSCREEN_FT5306_WPX2)
struct ft5x0x_platform_data{
          u16     model;
    int     (*get_pendown_state)(void);
    int     (*init_platform_hw)(void);
    int     (*ft5x0x_platform_sleep)(void);
    int     (*ft5x0x_platform_wakeup)(void);
    void    (*exit_platform_hw)(void);
};
#endif

#if defined(CONFIG_TOUCHSCREEN_BYD693X)
struct byd_platform_data {
	u16     model;
	int     pwr_pin;
	int	  int_pin;
	int     rst_pin;
	int		pwr_on_value;
	int (*get_probe_state)(void);
	void   (*set_probe_state)(int );
	int 	*tp_flag;

	uint16_t screen_max_x;
	uint16_t screen_max_y;
	u8 swap_xy :1;
	u8 xpol :1;
	u8 ypol :1;	
};
#endif

#if defined (CONFIG_TOUCHSCREEN_BF6931A)
struct bf6931a_platform_data{
	  u16     model;
    int     (*get_pendown_state)(void);
    int     (*init_platform_hw)(void);
    int     (*bf6931a_platform_sleep)(void);
    int     (*bf6931a_platform_wakeup)(void);
    void    (*exit_platform_hw)(void);
};
#endif

struct codec_io_info{
	char	iomux_name[50];
	int		iomux_mode;	
};

struct rt3261_platform_data{
	unsigned int codec_en_gpio;
	struct codec_io_info codec_en_gpio_info;
	int (*io_init)(int, char *, int);
};

#if defined (CONFIG_TOUCHSCREEN_NOVATEK)
struct novatek_i2c_platform_data {
	uint32_t version;		/* Use this entry for panels with */

	int (*ts_init_platform_hw)(void);
	int (*ts_exit_platform_hw)(void);
	int (*get_probe_state)(void);
	void (*set_probe_state)(int);	
	int gpio_rst;
	int gpio_irq;
	bool irq_edge; 		/* 0:rising edge, 1:falling edge */
	uint16_t touch_max_x;
	uint16_t touch_max_y;
	uint16_t screen_max_x;
	uint16_t screen_max_y;
	u8 swap_xy :1;
	u8 xpol :1;
	u8 ypol :1;
};
#endif

extern struct rk29_sdmmc_platform_data default_sdmmc0_data;
extern struct rk29_sdmmc_platform_data default_sdmmc1_data;

extern struct i2c_gpio_platform_data default_i2c_gpio_data; 
extern struct rk29_vmac_platform_data board_vmac_data;

void __init rk30_map_common_io(void);
void __init rk30_init_irq(void);
void __init rk30_map_io(void);
struct machine_desc;
void __init rk30_fixup(struct machine_desc *desc, struct tag *tags, char **cmdline, struct meminfo *mi);
void __init rk30_clock_data_init(unsigned long gpll,unsigned long cpll,u32 flags);
void __init board_clock_init(void);
void board_gpio_suspend(void);
void board_gpio_resume(void);
void __sramfunc board_pmu_suspend(void);
void __sramfunc board_pmu_resume(void);

extern struct sys_timer rk30_timer;

enum _periph_pll {
	periph_pll_1485mhz = 148500000,
	periph_pll_297mhz = 297000000,
	periph_pll_300mhz = 300000000,
	periph_pll_1188mhz = 1188000000, /* for box*/
};
enum _codec_pll {
	codec_pll_360mhz = 360000000, /* for HDMI */
	codec_pll_408mhz = 408000000,
	codec_pll_456mhz = 456000000,
	codec_pll_504mhz = 504000000,
	codec_pll_552mhz = 552000000, /* for HDMI */
	codec_pll_600mhz = 600000000,
	codec_pll_742_5khz = 742500000,
	codec_pll_768mhz = 768000000,
	codec_pll_798mhz = 798000000,
	codec_pll_1188mhz = 1188000000,
};

//has extern 27mhz
#define CLK_FLG_EXT_27MHZ 			(1<<0)
//max i2s rate
#define CLK_FLG_MAX_I2S_12288KHZ 	(1<<1)
#define CLK_FLG_MAX_I2S_22579_2KHZ 	(1<<2)
#define CLK_FLG_MAX_I2S_24576KHZ 	(1<<3)
#define CLK_FLG_MAX_I2S_49152KHZ 	(1<<4)
//uart 1m\3m
#define CLK_FLG_UART_1_3M			(1<<5)
#define CLK_CPU_HPCLK_11				(1<<6)


#ifdef CONFIG_RK29_VMAC

#define RK30_CLOCKS_DEFAULT_FLAGS (CLK_FLG_MAX_I2S_12288KHZ/*|CLK_FLG_EXT_27MHZ*/|CLK_CPU_HPCLK_11)
#define periph_pll_default periph_pll_300mhz
#define codec_pll_default codec_pll_1188mhz

#else

#define RK30_CLOCKS_DEFAULT_FLAGS (CLK_FLG_MAX_I2S_12288KHZ/*|CLK_FLG_EXT_27MHZ*/|CLK_CPU_HPCLK_11)
#if (RK30_CLOCKS_DEFAULT_FLAGS&CLK_FLG_UART_1_3M)
#define codec_pll_default codec_pll_768mhz
#else
#define codec_pll_default codec_pll_798mhz
#endif
#define periph_pll_default periph_pll_297mhz

#endif






#endif
