RGB_MATRIX_EFFECT(raw_rgb)
RGB_MATRIX_EFFECT(unlocked)

#if defined(RGB_MATRIX_CUSTOM_EFFECT_IMPLS)
RGB raw_rgb_data[DRIVER_LED_TOTAL] = { 0 };

static uint8_t normalize_component(uint8_t component) {
    uint16_t x = (uint16_t)component;
    x *= rgb_matrix_config.hsv.v; // Multiply by current brightness
    x /= 255; // Divide by maximum brightness
    return (uint8_t)x;
}

static RGB normalize_index(uint8_t i) {
    RGB raw = raw_rgb_data[i];
    RGB rgb = {
        .r = normalize_component(raw.r),
        .g = normalize_component(raw.g),
        .b = normalize_component(raw.b),
    };
    return rgb;
}

static bool raw_rgb(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        RGB rgb = normalize_index(i);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < DRIVER_LED_TOTAL;
}

static uint8_t unlocked_keys[8][2] = {
    {2, 7}, // U
    {4, 6}, // N
    {3, 9}, // L
    {2, 9}, // O
    {4, 3}, // C
    {3, 8}, // K
    {2, 3}, // E
    {3, 3}, // D
};

static uint8_t unlocked_ticks = 0;
static uint8_t unlocked_i = 0;
static uint8_t unlocked_leds_count = 0;
static uint8_t unlocked_leds[2] = { 0, 0 };

static bool unlocked(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    unlocked_ticks++;

    if (params->init) {
        unlocked_ticks = 0;
        unlocked_i = 0;
    }

    if (unlocked_ticks == 0) {
        if (unlocked_i == 8) {
            unlocked_leds_count = 0;
            unlocked_i = 0;
        } else {
            unlocked_leds_count = rgb_matrix_map_row_column_to_led(
                unlocked_keys[unlocked_i][0],
                unlocked_keys[unlocked_i][1],
                unlocked_leds
            );
            unlocked_i++;
        }
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();

        HSV hsv = {
            .h = i + unlocked_ticks,
            .s = 0xFF,
            .v = 0x70,
        };
        for (uint8_t j = 0; j < unlocked_leds_count; j++) {
            if (i == unlocked_leds[j]) {
                hsv.s = 0;
                hsv.v = 0xFF;
            }
        }

        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return led_max < DRIVER_LED_TOTAL;
}
#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS