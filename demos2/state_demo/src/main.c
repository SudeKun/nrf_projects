#include <zephyr/kernel.h>
#include <zephyr/smf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

// LED device tree specifications 
#if DT_NODE_HAS_STATUS(DT_ALIAS(led0), okay)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
#else
#error "LED0 device tree alias is not defined or not okay"
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(led1), okay)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
#else
#error "LED1 device tree alias is not defined or not okay"
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(led2), okay)
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
#else
#error "LED2 device tree alias is not defined or not okay"
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(led3), okay)
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);
#else
#error "LED3 device tree alias is not defined or not okay"
#endif

// Forward declaration of state table 
static const struct smf_state demo_states[];

// User defined object
struct s_object {
        struct smf_ctx ctx;

        struct k_event smf_event;
        int32_t events;
} s_obj;


//Flat State Machine Example
/*

// List of demo states 
enum demo_state { S0, S1, S2, S3 };

// User defined object
struct s_object {
        // This must be first 
        struct smf_ctx ctx;

        // Other state specific data add here 
} s_obj;

// State S0 - LED0 control 
static void s0_entry(void *o)
{
        gpio_pin_set_dt(&led0, 1);
}
static void s0_run(void *o)
{
        smf_set_state(SMF_CTX(&s_obj), &demo_states[S1]);
}
static void s0_exit(void *o)
{
        gpio_pin_set_dt(&led0, 0);
}

// State S1 - LED1 control 
static void s1_entry(void *o)
{
        gpio_pin_set_dt(&led1, 1);
}
static void s1_run(void *o)
{
        smf_set_state(SMF_CTX(&s_obj), &demo_states[S2]);
}
static void s1_exit(void *o)
{
        gpio_pin_set_dt(&led1, 0);
}

// State S2 - LED2 control 
static void s2_entry(void *o)
{
        gpio_pin_set_dt(&led2, 1);
}
static void s2_run(void *o)
{
        smf_set_state(SMF_CTX(&s_obj), &demo_states[S3]);
}
static void s2_exit(void *o)
{
        gpio_pin_set_dt(&led2, 0);
}

// State S3 - All LEDs off, transition back to S0 
static void s3_entry(void *o)
{
        // All LEDs should be off at this point 
}
static void s3_run(void *o)
{
        smf_set_state(SMF_CTX(&s_obj), &demo_states[S0]);
}

// Populate state table 
static const struct smf_state demo_states[] = {
        [S0] = SMF_CREATE_STATE(s0_entry, s0_run, s0_exit, NULL, NULL),
        [S1] = SMF_CREATE_STATE(s1_entry, s1_run, s1_exit, NULL, NULL),
        [S2] = SMF_CREATE_STATE(s2_entry, s2_run, s2_exit, NULL, NULL),
        [S3] = SMF_CREATE_STATE(s3_entry, s3_run, NULL, NULL, NULL),
};

int main(void)
{
        int32_t ret;

        // Initialize LEDs 
        if (!gpio_is_ready_dt(&led0) || !gpio_is_ready_dt(&led1) || !gpio_is_ready_dt(&led2)) {
                return -1;
        }
        
        gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);

        // Set initial state 
        smf_set_initial(SMF_CTX(&s_obj), &demo_states[S0]);

        // Run the state machine 
        while(1) {
                // State machine terminates if a non-zero value is returned 
                ret = smf_run_state(SMF_CTX(&s_obj));
                if (ret) {
                        // handle return code and terminate state machine 
                        break;
                }
                k_msleep(1000);
        }
}
*/

//Hierarchical State Machine Example

/*

// List of demo states 
enum demo_state { PARENT, S0, S1, S2 };

// Parent State 
static void parent_entry(void *o)
{
        gpio_pin_set_dt(&led0, 1);
        gpio_pin_set_dt(&led1, 1);
        gpio_pin_set_dt(&led2, 1);
        gpio_pin_set_dt(&led3, 1);
}
static void parent_exit(void *o)
{
        gpio_pin_set_dt(&led0, 0);
        gpio_pin_set_dt(&led1, 0);
        gpio_pin_set_dt(&led2, 0);
        gpio_pin_set_dt(&led3, 0);
}

// State S0 
static void s0_run(void *o)
{
        gpio_pin_set_dt(&led0, 1);
        smf_set_state(SMF_CTX(&s_obj), &demo_states[S1]);
}

// State S1 
static void s1_run(void *o)
{
        gpio_pin_set_dt(&led1, 1);
        smf_set_state(SMF_CTX(&s_obj), &demo_states[S2]);
}
// State S2 
static void s2_run(void *o)
{
        gpio_pin_set_dt(&led2, 1);
        smf_set_state(SMF_CTX(&s_obj), &demo_states[S0]);
}
static const struct smf_state demo_states[] = {
        // Parent state does not have a run action 
        [PARENT] = SMF_CREATE_STATE(parent_entry, NULL, parent_exit, NULL, NULL),
        // Child states do not have entry or exit actions 
        [S0] = SMF_CREATE_STATE(NULL, s0_run, NULL, &demo_states[PARENT], NULL),
        [S1] = SMF_CREATE_STATE(NULL, s1_run, NULL, &demo_states[PARENT], NULL),
        // State S2 do not have entry or exit actions and no parent 
        [S2] = SMF_CREATE_STATE(NULL, s2_run, NULL, NULL, NULL),
};

int main(void)
{
        int32_t ret;

        // Initialize LEDs 
        if (!gpio_is_ready_dt(&led0) || !gpio_is_ready_dt(&led1) || 
            !gpio_is_ready_dt(&led2) || !gpio_is_ready_dt(&led3)) {
                return -1;
        }
        
        gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);

        // Set initial state 
        smf_set_initial(SMF_CTX(&s_obj), &demo_states[S0]);

        // Run the state machine 
        while(1) {
                // State machine terminates if a non-zero value is returned 
                ret = smf_run_state(SMF_CTX(&s_obj));
                if (ret) {
                        // handle return code and terminate state machine 
                        break;
                }
                k_msleep(1000);
        }
}
*/

// Event Driven State Machine Example
#define SW0_NODE        DT_ALIAS(sw0)
#define EVENT_BTN_PRESS BIT(0)

static const struct gpio_dt_spec button =GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

static struct gpio_callback button_cb_data;


/* List of demo states */
enum demo_state { S0, S1 };


/* State S0 */
static void s0_entry(void *o)
{
        printk("STATE0\n");
}

static void s0_run(void *o)
{
        struct s_object *s = (struct s_object *)o;

        /* Change states on Button Press Event */
        if (s->events & EVENT_BTN_PRESS) {
                smf_set_state(SMF_CTX(&s_obj), &demo_states[S1]);
        }
}

/* State S1 */
static void s1_entry(void *o)
{
        printk("STATE1\n");
}

static void s1_run(void *o)
{
        struct s_object *s = (struct s_object *)o;

        /* Change states on Button Press Event */
        if (s->events & EVENT_BTN_PRESS) {
                smf_set_state(SMF_CTX(&s_obj), &demo_states[S0]);
        }
}

/* Populate state table */
static const struct smf_state demo_states[] = {
        [S0] = SMF_CREATE_STATE(s0_entry, s0_run, NULL, NULL, NULL),
        [S1] = SMF_CREATE_STATE(s1_entry, s1_run, NULL, NULL, NULL),
};

void button_pressed(const struct device *dev,
                struct gpio_callback *cb, uint32_t pins)
{
        /* Generate Button Press Event */
        k_event_post(&s_obj.smf_event, EVENT_BTN_PRESS);
}

int main(void)
{
        int ret;

        if (!gpio_is_ready_dt(&button)) {
                printk("Error: button device %s is not ready\n",
                        button.port->name);
                return 0;
        }

        ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
        if (ret != 0) {
                printk("Error %d: failed to configure %s pin %d\n",
                        ret, button.port->name, button.pin);
                return 0;
        }

        ret = gpio_pin_interrupt_configure_dt(&button,
                GPIO_INT_EDGE_TO_ACTIVE);
        if (ret != 0) {
                printk("Error %d: failed to configure interrupt on %s pin %d\n",
                        ret, button.port->name, button.pin);
                return 0;
        }

        gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
        gpio_add_callback(button.port, &button_cb_data);

        /* Initialize the event */
        k_event_init(&s_obj.smf_event);

        /* Set initial state */
        smf_set_initial(SMF_CTX(&s_obj), &demo_states[S0]);

        /* Run the state machine */
        while(1) {
                /* Block until an event is detected */
                s_obj.events = k_event_wait(&s_obj.smf_event,
                                EVENT_BTN_PRESS, true, K_FOREVER);

                /* State machine terminates if a non-zero value is returned */
                ret = smf_run_state(SMF_CTX(&s_obj));
                if (ret) {
                        /* handle return code and terminate state machine */
                        break;
                }
        }
}
