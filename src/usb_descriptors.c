#include <string.h>

#include "tusb.h"

enum {
    ITF_NUM_CDC,
    ITF_NUM_CDC_DATA,
    ITF_NUM_MIDI,
    ITF_NUM_MIDI_STREAMING,
    ITF_NUM_TOTAL,
};

enum {
    EPNUM_CDC_NOTIFICATION = 0x81,
    EPNUM_CDC_OUT = 0x02,
    EPNUM_CDC_IN = 0x82,
    EPNUM_MIDI_OUT = 0x03,
    EPNUM_MIDI_IN = 0x83,
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_MIDI_DESC_LEN)

static tusb_desc_device_t const device_descriptor = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x1209,
    .idProduct = 0x7273,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01,
};

uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *)&device_descriptor;
}

static uint8_t const configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 0, EPNUM_CDC_NOTIFICATION, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 0, EPNUM_MIDI_OUT, EPNUM_MIDI_IN, 64),
};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return configuration_descriptor;
}

static char const *string_descriptors[] = {
    (const char[]){0x09, 0x04},
    "FLUXPAD",
    "Isometric Piano Test",
    "0001",
};

static uint16_t string_descriptor[32];

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    if (index == 0) {
        memcpy(&string_descriptor[1], string_descriptors[0], 2);
        string_descriptor[0] = (TUSB_DESC_STRING << 8) | 4;
    } else {
        uint8_t character_count = strlen(string_descriptors[index]);

        if (character_count > 31) {
            character_count = 31;
        }

        for (uint8_t character = 0; character < character_count; character++) {
            string_descriptor[character + 1] = string_descriptors[index][character];
        }

        string_descriptor[0] = (TUSB_DESC_STRING << 8) | (character_count * 2 + 2);
    }

    return string_descriptor;
}
