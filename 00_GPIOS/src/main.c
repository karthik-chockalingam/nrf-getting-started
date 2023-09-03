/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   500

// Retrieving the node from device tree; Following macros are useful
#define LED0_NODE 			DT_NODELABEL(led0)    
#define LED1_NODE			DT_ALIAS(led1)
#define LED2_NODE			DT_PATH(leds, led_2)

// From the gpios property, we can retrieve the following
// Retrieving phandle to gpio node(also called gpio port), gpio pin and flags
#define LED0_GPIO_PORT		DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_GPIO_PIN		DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_GPIO_FLAG		DT_GPIO_FLAGS(LED0_NODE, gpios)

#define LED1_GPIO_PORT		DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_GPIO_PIN		DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_GPIO_FLAG		DT_GPIO_FLAGS(LED1_NODE, gpios)

#define LED2_GPIO_PORT		DT_GPIO_LABEL(LED2_NODE, gpios)
#define LED2_GPIO_PIN		DT_GPIO_PIN(LED2_NODE, gpios)
#define LED2_GPIO_FLAG		DT_GPIO_FLAGS(LED2_NODE, gpios)
#define LED2_GPIO_LABEL		DT_PROP(LED2_NODE, label)

// Get the Button Node identifiers and properties
#define Button2_GPIO_NODE	DT_ALIAS(sw2)
#define Button2_GPIO_PIN	DT_GPIO_PIN(Button2_GPIO_NODE, gpios)

#define Button3_GPIO_NODE	DT_ALIAS(sw3)
#define Button3_GPIO_PIN	DT_GPIO_PIN(Button3_GPIO_NODE, gpios)

// Define the interrupt callback handler
const struct gpio_dt_spec led1_dt_spec = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
void button2_callback_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	gpio_pin_toggle_dt(&led1_dt_spec);
}

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */

int main(void)
{
	uint8_t buttonPress = 0;
	const struct gpio_dt_spec led0_dt_spec = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

	const struct gpio_dt_spec led2_dt_spec = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
	gpio_pin_configure_dt(&led0_dt_spec, LED0_GPIO_FLAG|GPIO_OUTPUT);
	gpio_pin_configure_dt(&led1_dt_spec, LED1_GPIO_FLAG|GPIO_OUTPUT);
	gpio_pin_configure_dt(&led2_dt_spec, LED2_GPIO_FLAG|GPIO_OUTPUT);

	const struct gpio_dt_spec button3_dt_spec = GPIO_DT_SPEC_GET(Button3_GPIO_NODE, gpios);
	gpio_pin_configure_dt(&button3_dt_spec, GPIO_INPUT);

	const struct gpio_dt_spec button2_dt_spec = GPIO_DT_SPEC_GET(Button2_GPIO_NODE, gpios);
	gpio_pin_configure_dt(&button2_dt_spec, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&button2_dt_spec, GPIO_INT_EDGE_TO_ACTIVE);

	// Init, register/add the callback function 
	struct gpio_callback button2_cb;
	gpio_init_callback(&button2_cb, button2_callback_handler, BIT(button2_dt_spec.pin));
	gpio_add_callback_dt(&button2_dt_spec, &button2_cb);

	while (1) {
		gpio_pin_toggle_dt(&led0_dt_spec);
		printk("Hello World..\n");
		printk("The label property of LED2 node is %s\n", LED2_GPIO_LABEL);

		// Button Polling Mode
		buttonPress = gpio_pin_get_dt(&button3_dt_spec);
		if(buttonPress)
		{
			gpio_pin_toggle_dt(&led2_dt_spec);
		}
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
