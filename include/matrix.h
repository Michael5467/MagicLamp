#ifndef __INC_MATRIX_LED_v2_H
#define __INC_MATRIX_LED_v2_H

// Power (current) consumption
#define BRIGHTNESS       150            // Default maximum brightness (0-255)
#define CURRENT_LIMIT    2000           // Current limit in milliamps, brightness automatically controlled; 0 - no limit

// Matrix parameters
    // Geometry
#define WIDTH            16             // matrix width (X coordinate)
#define HEIGHT           16             // matrix height (Y coordinate)
#define NUM_LEDS         WIDTH * HEIGHT // The number of LEDs in the matrix (in the strip)
#define COLOR_ORDER      GRB            // Color order ()

    // Physical implementation
#define MATRIX_TYPE      0              // Matrix type (by control): 0 - zigzag, 1 - parallel
#define CONNECTION_ANGLE 0              // Connection angle: 0 - bottom left, 1 - top left, 2 - top right, 3 - bottom right
#define STRIP_DIRECTION  0              // Strip direction from the corner: 0 - to the right, 1 - up, 2 - to the left, 3 - down

    // Matrix configuration
#if (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 0)
#define THIS_WIDTH WIDTH
#define THIS_X x
#define THIS_Y y

#elif (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 1)
#define THIS_WIDTH HEIGHT
#define THIS_X y
#define THIS_Y x

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 0)
#define THIS_WIDTH WIDTH
#define THIS_X x
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 3)
#define THIS_WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y x

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 2)
#define THIS_WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 3)
#define THIS_WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y (WIDTH - x - 1)

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 2)
#define THIS_WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y y

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 1)
#define THIS_WIDTH HEIGHT
#define THIS_X y
#define THIS_Y (WIDTH - x - 1)

#else
#define THIS_WIDTH WIDTH
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters! Set to default"

#endif
    
#endif // __INC_MATRIX_LED_v2_H