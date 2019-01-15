/*
 * (C) Copyright 2017 - Datalogic Automation
 */
#include <common.h>
#include <malloc.h>
#include <fs.h>
#include <asm/gpio.h>


///////// MACRO & CONSTANTS ///////////////////////////////////////////////////

#define ENV_VAR_BPRESS              "BUTTON_PRESSED" //! Env. var set by check button function
#define ENV_VAR_ETH0                "ethaddr"
#define ENV_VAR_ETH1                "eth1addr"
#define ENV_VAR_RESTORE_DEFAULT     "userrestoredefault"
#define ENV_VAR_BOOT_LOADER         "userbootloader"
#define ENV_VAR_SWITCH_TO_AUX       "userswitchtoaux"
#define MAC_ADDR0_STR               "macAddress"
#define MAC_ADDR1_STR               "macAddress1"

#define MODE_STANDARD              0
#define MODE_DEFAULT_WAIT          10
#define MODE_DEFAULT_CONFIRM       11
#define MODE_LOADER                20
#define MODE_SWITCH                30
#define MODE_DEFAULT_WAIT_TO_MS    4000
#define MODE_DEFAULT_CONFIRM_TO_MS 1000
#define MODE_LOADER_TO_MS          4000
#define MODE_DEFAULT_LED_TO_MS     250
#define MODE_SWITCH_TO_MS          4000
#define MODE_SWITCH_RETRIES        10

#define GPIO0_BUTTON                72

///////// STRUCTURE DEFINISION ///////////////////////////////////////////////////

struct led {
	const char * name;
	uint id;
};

static struct led leds[] = {
	{ "STATUS" , 33 },
	{ "COM" , 34 },
	{ "TRIGGER" , 35 },	
	{ "GOOD" , 36 },
	{ "READY" , 37 },
};

#define GPIO0_LED_STATUS (leds[0].id)
#define GPIO0_LED_COM (leds[1].id)
#define GPIO0_LED_TRIGGER  (leds[2].id)
#define GPIO0_LED_GOOD  (leds[3].id)
#define GPIO0_LED_READY (leds[4].id)

///////// UTILITY FUNCTIONS ///////////////////////////////////////////////////

static int dla_utils_led_check(char* id)
{
	for (int i = 0; i < sizeof(leds)/sizeof(struct led); i++)
		if (strcmp(id, leds[i].name) == 0)
			return leds[i].id;
	
	return -1;
}
  
static int dla_utils_led_set(unsigned long id, unsigned long val)
{
    if (!val)
        gpio_direction_input(id);
    else
        gpio_set_value(id, 0);
    return 0;
}

static int dla_utils_led_setall(unsigned long val)
{
	for (int i = 0; i < sizeof(leds)/sizeof(struct led); i++)
		dla_utils_led_set(leds[i].id, val);
    return 0;
}

static int dla_utils_button_pressed(void)
{
	return gpio_get_value(GPIO0_BUTTON);
}
///////// COMMANDS IMPLEMENTATION /////////////////////////////////////////

/**
 * @brief Sample button and set env var ENV_VAR_BPRESS
 * Usage: <cmd>
 */
static int do_hmi_buttoncheck(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	setenv(ENV_VAR_BPRESS, dla_utils_button_pressed() ? "true" : "false");
	return 0;
}

/**
 * @brief TODO
 * Usage: <cmd>
 */
static int do_hmi_manager(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 button_pressed = 0;
	u32 reset_leds = 0;
	u32 status = MODE_STANDARD;
	u32 t0, t, tLed0, tLed, ledCount = 0, retries;
	
	// clear output variabled
	setenv(ENV_VAR_RESTORE_DEFAULT, "false");
	setenv(ENV_VAR_BOOT_LOADER, "false");
	setenv(ENV_VAR_SWITCH_TO_AUX, "false"); 

	if (dla_utils_button_pressed() == 0)
	{
		return 0;  // Button is not pressed, continue with normal boot
	}

	// Manage button
	status = MODE_DEFAULT_WAIT;
	t0 = get_timer(0);
	tLed0 = 0;
	retries=0;

	do
	{
		udelay(1000);

		button_pressed = dla_utils_button_pressed();

		// Manage status
		switch (status)
		{
			case MODE_DEFAULT_WAIT:
				if (button_pressed)
				{
					t = get_timer(t0);
					if (t > MODE_DEFAULT_WAIT_TO_MS)
					{
						// Time elapsed, move to loader selection (or switch)
						if (retries != MODE_SWITCH_RETRIES)
						{
							status = MODE_LOADER;
						}
						else
						{
							status = MODE_SWITCH;
						}
						t0 = get_timer(0);
						retries++;
					}
				}
				else
				{
					// Button released, If it will be pressed again restore default else exit
					status = MODE_DEFAULT_CONFIRM;
					t0 = get_timer(0);
				}
				break;

			case MODE_DEFAULT_CONFIRM:
				if (button_pressed)
				{
					printf("\n[%s] User select <restore defaults>.\n", __FUNCTION__);
					setenv(ENV_VAR_RESTORE_DEFAULT, "true");
					return 0;
				}
				t = get_timer(t0);
				if (t > MODE_DEFAULT_CONFIRM_TO_MS)
				{
					// Button has not been pressed again, exit
					status = MODE_STANDARD;
				}
				break;

			case MODE_LOADER:
				if (button_pressed)
				{
					t = get_timer(t0);
					if (t > MODE_LOADER_TO_MS)
					{
						// Time elapsed, move to restore default selection
						status = MODE_DEFAULT_WAIT;
						reset_leds = 1;
						t0 = get_timer(0);
					}
				}
				else
				{
					// Button released, boot loader image
					printf("\n[%s] User select <loader>.\n", __FUNCTION__);
					setenv(ENV_VAR_BOOT_LOADER, "true"); 
					status = MODE_STANDARD;
				}
				break;

			case MODE_SWITCH:
				if (button_pressed)
				{
					t = get_timer(t0);
					if (t > MODE_SWITCH_TO_MS)
					{
						// Time elapsed, move to restore default selection
						status = MODE_DEFAULT_WAIT;
						t0 = get_timer(0);
					}
				}
				else
				{
					// Button released, boot loader image
					printf("\n[%s] User select <switch_to_aux>.\n", __FUNCTION__);
					setenv(ENV_VAR_SWITCH_TO_AUX, "true"); 
					status = MODE_STANDARD;
				}
				break;

			case MODE_STANDARD:
				break;

			default:
				return 1;
		}

		// Manage leds
		switch (status)
		{
			case MODE_DEFAULT_WAIT:
				if (reset_leds == 1)
				{
					dla_utils_led_setall(0);
					reset_leds = 0;
				}
				tLed = get_timer(tLed0);
				if (tLed > MODE_DEFAULT_LED_TO_MS)
				{
					switch (ledCount) {
						case 0:
							dla_utils_led_setall(1);
							break;
						case 1:
							dla_utils_led_setall(0);
							break;
					}
					ledCount = !ledCount;
					tLed0 = get_timer(0);
				}
				break;

			case MODE_LOADER:
				dla_utils_led_setall(1);
				break;

			case MODE_SWITCH:
				dla_utils_led_set(GPIO0_LED_STATUS, 1);
				dla_utils_led_set(GPIO0_LED_COM, 1);
				dla_utils_led_set(GPIO0_LED_TRIGGER, 0);
				dla_utils_led_set(GPIO0_LED_GOOD, 1);
				dla_utils_led_set(GPIO0_LED_READY, 1);
				break;

			case MODE_DEFAULT_CONFIRM:
			case MODE_STANDARD:
				dla_utils_led_setall(0);
				break;

			default:
				return 1;
		}
	}
	while(status != MODE_STANDARD);

	return 0;
}

/**
 * @brief Get/Set led value
 * Usage: <cmd> <led-offset> <val> (Ex. dlaledset 16 1)
 */
static int do_hmi_ledset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long ret, id, val;
	if (argc != 3)
	{
		printf("\n[%s] Wrong number of arguments\n", __FUNCTION__);
		return 1;
	}

	id  = dla_utils_led_check(argv[1]);
	if (id < 0)
	{
			printf("\n[%s] Led not valid (%s)\n", __FUNCTION__, argv[1]);
			return 2;
	}
	
	val = simple_strtoul(argv[2], NULL , 10);

	//printf("\n[%s] Set led %lu to value %lu\n", __FUNCTION__, id, val);
	ret = dla_utils_led_set(id, val);
	if (ret != 0)
	{
		printf("\n[%s] Set led %lu to value %lu\n", __FUNCTION__, id, val);
		return 3;
	}

	return 0;
}

/**
 * @brief Blink all leds
 * Usage: <cmd> (Ex. dlablinkall)
 */
static int do_hmi_ledblinkall(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	dla_utils_led_setall(1);

	udelay(300*1000);

	dla_utils_led_setall(0);

	return 0;
}

////////// UBOOT COMMAND DEFINITION ///////////////////////////////////////////

static cmd_tbl_t cmd_hmi_sub[] = {
	U_BOOT_CMD_MKENT(ledset, CONFIG_SYS_MAXARGS, 0, do_hmi_ledset, "", ""),
	U_BOOT_CMD_MKENT(ledblinkall, CONFIG_SYS_MAXARGS, 0, do_hmi_ledblinkall, "", ""),
	U_BOOT_CMD_MKENT(buttoncheck, 1, 0, do_hmi_buttoncheck, "", ""),
	U_BOOT_CMD_MKENT(manager, 1, 0, do_hmi_manager, "", "")
};

static int do_hmi(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
        cmd_tbl_t *cp;

        if (argc < 2)
                return CMD_RET_USAGE;

        /* drop initial "env" arg */
        argc--;
        argv++;

        cp = find_cmd_tbl(argv[0], cmd_hmi_sub, ARRAY_SIZE(cmd_hmi_sub));

        if (cp)
                return cp->cmd(cmdtp, flag, argc, argv);

        return CMD_RET_USAGE;
}

static char hmi_help_text[] =
		"Hmi handling commands\n"
        "hmi ledset [name] [1/0] Turn LED ""name"" on or off.\n"
		"hmi ledblinkall Makes all LEDs blink.\n"
		"hmi buttoncheck Check HMI button and set " ENV_VAR_BPRESS " var.\n"
		"hmi manager Start the HMI manager for boot selection.\n";

U_BOOT_CMD(hmi, CONFIG_SYS_MAXARGS, 1, do_hmi, "HMI handling commands", hmi_help_text);



