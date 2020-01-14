#ifndef __INC_MATRIX_LED_v2_H
#define __INC_MATRIX_LED_v2_H

// Power (current) consumption
#define BRIGHTNESS    150       // стандартная маскимальная яркость (0-255)
#define CURRENT_LIMIT 2000      // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

// Matrix parameters
    // Geometry
#define WIDTH  16               // ширина матрицы - координата X
#define HEIGHT 16               // высота матрицы - координата Y
#define NUM_LEDS WIDTH * HEIGHT // Количество светодиодов в матрице
#define COLOR_ORDER GRB         // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

    // Physical implementation
#define MATRIX_TYPE      0      // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE 1 // 0      // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION  3 // 0      // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз

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