#include <inttypes.h>
#include <stdio.h>

#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "tusb.h"

enum {
    ANALOG_KEY_COUNT = 3,
    SAMPLES_PER_READING = 8,
    SCAN_INTERVAL_US = 250,
    VELOCITY_HIGH_ADC = 1600,
    VELOCITY_LOW_ADC = 1200,
    VELOCITY_FASTEST_US = 2000,
    VELOCITY_SLOWEST_US = 12000,
    VELOCITY_MINIMUM = 24,
    RELEASE_THRESHOLD_ADC = 1500,
};

static const uint analog_key_pins[ANALOG_KEY_COUNT] = {28, 27, 26};
static const uint8_t midi_notes[ANALOG_KEY_COUNT] = {67, 64, 60};

typedef struct {
    bool is_pressed;
    bool is_timing;
    bool has_reading;
    uint16_t last_reading;
    uint64_t timing_start_us;
} key_state_t;

static key_state_t key_states[ANALOG_KEY_COUNT];

static uint16_t read_analog_key(uint pin) {
    uint32_t total = 0;

    adc_select_input(pin - 26);
    for (uint sample = 0; sample < SAMPLES_PER_READING; sample++) {
        total += adc_read();
    }

    return total / SAMPLES_PER_READING;
}

static uint32_t integer_square_root(uint32_t value) {
    uint32_t root = 0;

    while ((root + 1) * (root + 1) <= value) {
        root++;
    }

    return root;
}

static uint8_t time_to_velocity(uint64_t time_us) {
    if (time_us <= VELOCITY_FASTEST_US) {
        return 127;
    }

    uint8_t minimum = (uint8_t)VELOCITY_MINIMUM;

    if (time_us >= VELOCITY_SLOWEST_US) {
        return minimum;
    }

    uint32_t range_us = VELOCITY_SLOWEST_US - VELOCITY_FASTEST_US;
    uint32_t above_fastest = (uint32_t)time_us - VELOCITY_FASTEST_US;
    uint32_t normalized = 1000000ULL * above_fastest / range_us;
    uint32_t root_per_mille = integer_square_root(normalized);

    return (uint8_t)(minimum + (127 - minimum) * (1000 - root_per_mille) / 1000);
}

static void send_midi_note_on(uint key, uint8_t velocity) {
    uint8_t message[] = {0x90, midi_notes[key], velocity};

    tud_midi_stream_write(0, message, sizeof(message));
}

static void send_midi_note_off(uint key) {
    uint8_t message[] = {0x80, midi_notes[key], 0};

    tud_midi_stream_write(0, message, sizeof(message));
}

static void write_debug_message(char const *message, int length) {
    if (tud_cdc_connected()) {
        if (tud_cdc_write_available() >= (uint32_t)length) {
            tud_cdc_write(message, length);
            tud_cdc_write_flush();
        }
    }
}

static void print_press(uint key, uint64_t time_us, uint8_t velocity) {
    char message[96];
    int length = snprintf(message, sizeof(message), "[KEY_%u]: press - [time_us=%" PRIu64 "] - velocity %u\r\n",
                          key + 2, time_us, velocity);

    write_debug_message(message, length);
}

static void print_release(uint key) {
    char message[32];
    int length = snprintf(message, sizeof(message), "[KEY_%u]: release\r\n", key + 2);

    write_debug_message(message, length);
}

static void update_key_state(uint key, uint16_t reading, uint64_t current_time_us) {
    key_state_t *state = &key_states[key];

    if (!state->has_reading) {
        state->has_reading = true;
        state->last_reading = reading;
        return;
    }

    if (state->is_pressed) {
        if (reading >= RELEASE_THRESHOLD_ADC) {
            send_midi_note_off(key);
            print_release(key);
            state->is_pressed = false;
        }
    } else if (state->is_timing) {
        if (reading < VELOCITY_LOW_ADC) {
            uint64_t travel_time_us = current_time_us - state->timing_start_us;
            uint8_t velocity = time_to_velocity(travel_time_us);

            send_midi_note_on(key, velocity);
            print_press(key, travel_time_us, velocity);
            state->is_timing = false;
            state->is_pressed = true;
        } else if (reading >= VELOCITY_HIGH_ADC) {
            state->is_timing = false;
        }
    } else {
        if (reading < VELOCITY_HIGH_ADC) {
            state->is_timing = true;
            state->timing_start_us = current_time_us;
        }
    }

    state->last_reading = reading;
}

void tud_cdc_line_coding_cb(uint8_t interface, cdc_line_coding_t const *line_coding) {
    (void)interface;

    if (line_coding->bit_rate == 1200) {
        reset_usb_boot(0, 0);
    }
}

int main(void) {
    uint64_t last_scan_time_us = 0;

    adc_init();
    tusb_init();

    for (uint key = 0; key < ANALOG_KEY_COUNT; key++) {
        adc_gpio_init(analog_key_pins[key]);
    }

    while (true) {
        uint64_t current_time_us = time_us_64();

        tud_task();

        if (current_time_us - last_scan_time_us >= SCAN_INTERVAL_US) {
            last_scan_time_us = current_time_us;

            for (uint key = 0; key < ANALOG_KEY_COUNT; key++) {
                update_key_state(key, read_analog_key(analog_key_pins[key]), current_time_us);
            }
        }

        while (tud_midi_available()) {
            uint8_t message[4];
            tud_midi_packet_read(message);
        }

        tight_loop_contents();
    }
}
