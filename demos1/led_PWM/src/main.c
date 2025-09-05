#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/pwm.h>

// Define the PWM period and pulse width in nanoseconds
// If PWM_PERIOD_NS == PWM_PULSE_WIDTH then just lights up the LED
// If PWM_PERIOD_NS > PWM_PULSE_WIDTH then the LED will blink
// If PWM_PERIOD_NS < PWM_PULSE_WIDTH then the LED will not light up
#define PWM_PERIOD_NS 10000000
#define PWM_PULSE_WIDTH 5000000
//#define PWM_PERIOD_NS 20 000 000
//#define PWM_PULSE_WIDTH 1 400 000
#define PWM_LED0    DT_ALIAS(pwm_led0)

LOG_MODULE_REGISTER(pwm_led, LOG_LEVEL_INF);

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(PWM_LED0);

int main(void)
{
        int err;
        if (!pwm_is_ready_dt(&pwm_led0)) {
                LOG_ERR("Error: PWM device %s is not ready\n", pwm_led0.dev->name);
                return 0;
        }

        err = pwm_set_dt(&pwm_led0, PWM_PERIOD_NS, PWM_PULSE_WIDTH);
        if (err) {
                LOG_ERR("Error in pwm_set_dt(), err: %d", err);
                return 0;
        }

        return 0;
}
