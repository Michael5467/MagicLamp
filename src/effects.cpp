#include <Arduino.h>
#include <FastLED.h>
#include "functions.h"

// ****************************** Meteor Shower ****************************
meteor_object_t meteors[METEOR_MAX_OBJECTS];
track_object_t tracks[METEOR_MAX_OBJECTS][METEOR_TAIL_LENGTH];
int8_t meteors_count = 0;

void starfallRoutine(the_lamp_state_t *lamp_state)
{
	uint8_t new_meteors = 0;
	int32_t edge_index = 0;
	int32_t x = 0;
	int32_t y = HEIGHT * 16 - 1;
	uint8_t meteor_index = 0;

	if (lamp_state->loadingFlag)
	{
		lamp_state->loadingFlag = false;
		for (uint8_t i = 0; i < METEOR_MAX_OBJECTS; i++)
		{
			meteors[i].exist = false;
			meteors[i].visible = false;
			meteors[i].track_head = METEOR_TAIL_LENGTH;
			meteors[i].track_tail = METEOR_TAIL_LENGTH - 1;
			for (uint8_t j = 0; j <= METEOR_TAIL_LENGTH - 1; j++)
			{
				tracks[i][j].exist = false;
			}
		}
		meteors_count = 0;
	}

	if (meteors_count == 0)
	{
		randomSeed(micros());
	}

	// Move stars and fade tracks
	for (uint8_t i = 0; i <= METEOR_MAX_OBJECTS - 1; i++)
	{
		if (meteors[i].exist)
		{
			if (meteors[i].visible)
			{
				meteors[i].x += meteors[i].speed_vector_x;
				meteors[i].y += meteors[i].speed_vector_y;
				if ((meteors[i].x < 0) || (meteors[i].x > (WIDTH * 16 - 1) || (meteors[i].y < 0)) || (meteors[i].y > (HEIGHT * 16 - 1)))
				{
					meteors[i].visible = false;
				}
			}

			// Fade tail
			uint8_t track_index = meteors[i].track_head;
			do
			{
				fadePixel(lamp_state->leds, tracks[i][track_index].x, tracks[i][track_index].y, meteors[i].track_fade);
				track_index = (uint8_t)ChangeParameterValue(track_index, +1, METEOR_TAIL_LENGTH - 1, false);
			} while ((track_index != meteors[i].track_tail + 1) &&
					!((track_index == 0) && (meteors[i].track_tail == METEOR_TAIL_LENGTH - 1)));

			if (getPixelColorXY(lamp_state->leds, tracks[i][meteors[i].track_tail].x, tracks[i][meteors[i].track_tail].y) == 0)
			{
				meteors[i].track_tail = (uint8_t)ChangeParameterValue(meteors[i].track_tail, -1, METEOR_TAIL_LENGTH - 1, false);
			}

			if ((meteors[i].track_head == meteors[i].track_tail + 1) ||
					(((meteors[i].track_head == 0) && (meteors[i].track_tail == METEOR_TAIL_LENGTH - 1)) && (!meteors[i].visible)))
			{
				meteors[i].exist = false;
				meteors_count--;
			}

			if (meteors[i].visible)
			{
				uint8_t x_matrix = (uint8_t)(meteors[i].x >> 4);
				uint8_t y_matrix = (uint8_t)(meteors[i].y >> 4);

				drawPixelXY(lamp_state->leds, x_matrix, y_matrix, meteors[i].color);

				meteors[i].track_head = (uint8_t)ChangeParameterValue(meteors[i].track_head, -1, METEOR_TAIL_LENGTH - 1, false);
				tracks[i][meteors[i].track_head].x = x_matrix;
				tracks[i][meteors[i].track_head].y = y_matrix;
			}
		}
	}

	if (meteors_count >= METEOR_MAX_OBJECTS)
	{
		return;
	}

	if (random(lamp_state->scale) == 0)
	{
		new_meteors = 1;
	}
	else
	{
		if (meteors_count == 0)
		{
			if (random(lamp_state->scale / 4) == 0)
			{
				new_meteors = 1;
			}
		}
	}

	while (new_meteors > 0)
	{
		edge_index = random((HEIGHT * 3 / 4 + WIDTH * 3 / 4 - 1) * 16);
		x = 0;
		y = HEIGHT * 16 - 1;

		if (edge_index < (HEIGHT * 3 / 4) * 16)
		{
			y = (HEIGHT / 4) * 16 + edge_index;
		}
		else
		{
			x = edge_index - (HEIGHT * 3 / 4 + 1) * 16;
		}
		uint8_t x_matrix = (uint8_t)(x >> 4);
		uint8_t y_matrix = (uint8_t)(y >> 4);
		uint8_t track_length = random(5, METEOR_TAIL_LENGTH);

		meteor_index = 0;
		while (meteors[meteor_index].exist)
		{
			meteor_index++;
		}

		meteors[meteor_index].x = x;
		meteors[meteor_index].y = y;
		uint8_t speed = random(METEOR_MIN_SPEED, METEOR_MAX_SPEED);
		meteors[meteor_index].speed_vector_x =   speed + random(METEOR_DELTA_SPEED + 1);
		meteors[meteor_index].speed_vector_y = -(speed + random(METEOR_DELTA_SPEED + 1));
		meteors[meteor_index].track_head = METEOR_TAIL_LENGTH - 1;
		meteors[meteor_index].track_tail = METEOR_TAIL_LENGTH - 1;
		meteors[meteor_index].track_fade = 255 - (16 * track_length);
		meteors[meteor_index].color = CHSV(random(32, 256), random(32, METEOR_SATURATION), 255);
		meteors[meteor_index].exist = true;
		meteors[meteor_index].visible = true;

		tracks[meteor_index][METEOR_TAIL_LENGTH - 1].x = x_matrix;
		tracks[meteor_index][METEOR_TAIL_LENGTH - 1].y = y_matrix;
		drawPixelXY(lamp_state->leds, x_matrix, y_matrix, meteors[meteor_index].color);

		meteors_count++;
		new_meteors--;
	}
}

// ******************************* The Matrix ******************************

// The array contains color step values for the current line.
// If element == 0 - the line is empty and ready to draw
// The color step value determines the length of the line: the bigger step is the shorter line
uint8_t usedLine[WIDTH];
uint8_t totalLines;

void matrixRoutine(the_lamp_state_t *lamp_state)
{
	if (lamp_state->loadingFlag)
	{
		lamp_state->loadingFlag = false;
		for (uint8_t x = 0; x < WIDTH; x++)
			usedLine[x] = 0;
	}

	shiftMatrixDown(lamp_state->leds);

	// Check, who many empty rows are we have
	totalLines = 0;
	for (uint8_t x = 0; x < WIDTH; x++)
	{
		if (usedLine[x] == 0)
			totalLines++;
	}

	// Filling the top line:
	// continue the existing line or randomly generate a new one
	for (uint8_t x = 0; x < WIDTH; x++)
	{
		if (usedLine[x] > 0)
		{
			fadePixel(lamp_state->leds, x, HEIGHT - 1, usedLine[x]);

			// Clear faded pixels
			if (getPixelColorXY(lamp_state->leds, x, HEIGHT - 1) == 0)
			{
				usedLine[x] = 0;
			}
		}
		else
		{
			if (random(0, lamp_state->scale /*THE_MATRIX_DENSITY*/) == 0)
			{
				drawPixelXY(lamp_state->leds, x, HEIGHT - 1, THE_MATRIX_COLOR);
				usedLine[x] = 255 - 16 * random(3, /*(lamp_state->scale + 3)*/ THE_MATRIX_LENGTH);
			}
		}
	}
}

// ********************************* FLASH *********************************
void sparklesRoutine(the_lamp_state_t *lamp_state)
{
	uint8_t count = 0;
	uint8_t try_count = 0;

	// Decreasing the matrix brightness: sparks will be faded automatically
	fadeMatrix(lamp_state->leds, FLARE_COLOR_STEP_LIB);

	while ((count < lamp_state->scale) && (try_count < 255))
	{
		uint8_t x = random(0, WIDTH);
		uint8_t y = random(0, HEIGHT);
		if (getPixelColorXY(lamp_state->leds, x, y) == 0)
		{
			lamp_state->leds[getPixelNumber(x, y)] = CHSV(random(0, 256), 255, 255);
			count++;
		}
		try_count++;
	}
}

// ********************************* FIRE **********************************
unsigned char matrixValue[8][16];
unsigned char line[WIDTH];
int pcnt = 0;

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[8][16] PROGMEM =
		{
				{32,   0,    0,   0,   0,   0,   0,  32,  32,   0,   0,   0,   0,   0,   0,  32},
				{64,   0,    0,   0,   0,   0,   0,  64,  64,   0,   0,   0,   0,   0,   0,  64},
				{96,   32,   0,   0,   0,   0,  32,  96,  96,  32,   0,   0,   0,   0,  32,  96},
				{128,  64,  32,   0,   0,  32,  64, 128, 128,  64,  32,   0,   0,  32,  64, 128},
				{160,  96,  64,  32,  32,  64,  96, 160, 160,  96,  64,  32,  32,  64,  96, 160},
				{192, 128,  96,  64,  64,  96, 128, 192, 192, 128,  96,  64,  64,  96, 128, 192},
				{255, 160, 128,  96,  96, 128, 160, 255, 255, 160, 128,  96,  96, 128, 160, 255},
				{255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[8][16] PROGMEM = {
		{1, 11, 19, 25, 25, 22, 11,  1,  1, 11, 19, 25, 25, 22, 11,  1},
		{1,  8, 13, 19, 25, 19,  8,  1,  1,  8, 13, 19, 25, 19,  8,  1},
		{1,  8, 13, 16, 19, 16,  8,  1,  1,  8, 13, 16, 19, 16,  8,  1},
		{1,  5, 11, 13, 13, 13,  5,  1,  1,  5, 11, 13, 13, 13,  5,  1},
		{1,  5, 11, 11, 11, 11,  5,  1,  1,  5, 11, 11, 11, 11,  5,  1},
		{0,  1,  5,  8,  8,  5,  1,  0,  0,  1,  5,  8,  8,  5,  1,  0},
		{0,  0,  1,  5,  5,  1,  0,  0,  0,  0,  1,  5,  5,  1,  0,  0},
		{0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0}};

void fireRoutine(the_lamp_state_t *lamp_state)
{
	if (lamp_state->loadingFlag)
	{
		lamp_state->loadingFlag = false;
		generateLine();
		memset(matrixValue, 0, sizeof(matrixValue));
	}
	if (pcnt >= 100)
	{
		shiftUp();
		generateLine();
		pcnt = 0;
	}
	drawFrame(lamp_state->leds, pcnt);
	pcnt += 30;
}

// Randomly generate the next line (matrix row)
void generateLine()
{
	for (uint8_t x = 0; x < WIDTH; x++)
		line[x] = random(64, 255);
}

//shift all values in the matrix up one row
void shiftUp()
{
	for (uint8_t y = HEIGHT - 1; y > 0; y--)
	{
		for (uint8_t x = 0; x < WIDTH; x++)
		{
			uint8_t newX = x;
			if (x > 15)
				newX = x - 15;
			if (y > 7)
				continue;
			matrixValue[y][newX] = matrixValue[y - 1][newX];
		}
	}

	for (uint8_t x = 0; x < WIDTH; x++)
	{
		uint8_t newX = x;
		if (x > 15)
		{
			newX = x - 15;
		}
		matrixValue[0][newX] = line[newX];
	}
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation
void drawFrame(CRGB *leds, int pcnt)
{
	int nextv;

	//each row interpolates with the one before it
	for (unsigned char y = HEIGHT - 1; y > 0; y--)
	{
		for (unsigned char x = 0; x < WIDTH; x++)
		{
			uint8_t newX = x;
			if (x > 15)
			{
				newX = x - 15;
			}
			if (y < 8)
			{
				nextv = (((100.0 - pcnt) * matrixValue[y][newX] + pcnt * matrixValue[y - 1][newX]) / 100.0) - pgm_read_byte(&(valueMask[y][newX]));

				CRGB color = CHSV(
						FIRE_HUE_ADD + pgm_read_byte(&(hueMask[y][newX])), // H
						255,																							 // S
						(uint8_t)max(0, nextv)														 // V
				);

				leds[getPixelNumber(x, y)] = color;
			}
			else
			{
				if (y == 8 && FIRE_SPARKLES)
				{
					if (random(0, 20) == 0 && getPixelColorXY(leds, x, y - 1) != 0)
						drawPixelXY(leds, x, y, getPixelColorXY(leds, x, y - 1));
					else
						drawPixelXY(leds, x, y, 0);
				}
				else
				{
					if (FIRE_SPARKLES)
					{
						// старая версия для яркости
						if (getPixelColorXY(leds, x, y - 1) > 0)
							drawPixelXY(leds, x, y, getPixelColorXY(leds, x, y - 1));
						else
							drawPixelXY(leds, x, y, 0);
					}
				}
			}
		}
	}

	//first row interpolates with the "next" line
	for (unsigned char x = 0; x < WIDTH; x++)
	{
		uint8_t newX = x;
		if (x > 15)
			newX = x - 15;
		CRGB color = CHSV(
				FIRE_HUE_ADD + pgm_read_byte(&(hueMask[0][newX])),														 // H
				255,																																					 // S
				(uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
		);
		leds[getPixelNumber(newX, 0)] = color;
	}
}

// ********************************* BALLS *********************************
int32_t ball_coord[BALLS_MAX_AMOUNT][2]; // Current coordinates: [ball][x,y]
int8_t ball_vector[BALLS_MAX_AMOUNT][2]; // Speed with sign: [ball][x,y]
CRGB ball_color[BALLS_MAX_AMOUNT];

void ballsRoutine(the_lamp_state_t *lamp_state)
{
	// Balls' vectors and colors initialization by random before start
	if (lamp_state->loadingFlag)
	{
		lamp_state->loadingFlag = false;

		for (byte i = 0; i < lamp_state->scale; i++)
		{
			uint8_t sign;

			// Axis X
			ball_coord[i][0] = WIDTH / 2 * 16; // Middle of matrix by X
			sign = random(0, 2);							 // Speed sign: 0 - is positive; 1 - is negative
			if (sign == 0)
			{
				ball_vector[i][0] = random(BALLS_MIN_SPEED, BALLS_MAX_SPEED);
			}
			else
			{
				ball_vector[i][0] = -random(BALLS_MIN_SPEED, BALLS_MAX_SPEED);
			}

			// Axis Y
			ball_coord[i][1] = HEIGHT / 2 * 16; // Middle of matrix by Y
			sign = random(0, 2);								// Speed sign: 0 - is positive; 1 - is negative
			if (sign == 0)
			{
				ball_vector[i][1] = random(BALLS_MIN_SPEED, BALLS_MAX_SPEED);
			}
			else
			{
				ball_vector[i][1] = -random(BALLS_MIN_SPEED, BALLS_MAX_SPEED);
			}

			// Color
			ball_color[i] = CHSV((random(1, 17) * 16 - 1), 255, 255);
		}
	}

	if (BALLS_USE_TRACK)
	{
		fadeMatrix(lamp_state->leds, BALLS_TRACK_LENGTH); // Decreasing the matrix brightness: track will be faded automatically
	}
	else
	{
		FastLED.clear(); // Without tracks just clear the matrix
	}

	// Balls movement
	for (uint8_t i = 0; i < lamp_state->scale /*BALLS_AMOUNT*/; i++)
	{
		for (uint8_t j = 0; j < 2; j++)
		{
			ball_coord[i][j] += ball_vector[i][j];

			// Check bottom boundaries by X and Y
			if (ball_coord[i][j] < 0)
			{
				ball_coord[i][j] = 0;
				ball_vector[i][j] = -ball_vector[i][j];
			}
		}

		// Check the top boundary by X
		if (ball_coord[i][0] > (WIDTH * 16 - 1))
		{
			ball_coord[i][0] = (WIDTH * 16 - 1);
			ball_vector[i][0] = -ball_vector[i][0];
		}

		// Check the top boundary by Y
		if (ball_coord[i][1] > (HEIGHT * 16 - 1))
		{
			ball_coord[i][1] = (HEIGHT * 16 - 1);
			ball_vector[i][1] = -ball_vector[i][1];
		}

		drawPixelXY(lamp_state->leds, (ball_coord[i][0] >> 4), (ball_coord[i][1] >> 4), ball_color[i]);
	}
}

// ********************************** SNOW **********************************
void snowRoutine(the_lamp_state_t *lamp_state)
{
	// Shift matrix down on Y
	for (byte x = 0; x < WIDTH; x++)
	{
		for (byte y = 0; y < HEIGHT - 1; y++)
		{
			lamp_state->leds[getPixelNumber(x, y)] = lamp_state->leds[getPixelNumber(x, y + 1)];
			//drawPixelXY(x, y, getPixColorXY(x, y + 1));
		}
	}

	// Fill the top line by random ()
	for (byte x = 0; x < WIDTH; x++)
	{
		// two blocks cann't match by vertically
		if ((uint32_t)getPixelColorXY(lamp_state->leds, x, HEIGHT - 2) == 0 && (random(0, lamp_state->scale) == 0))
		{
			drawPixelXY(lamp_state->leds, x, HEIGHT - 1, (uint32_t)(SNOW_COLOR - SNOW_COLOR_STEP * random(0, 4)));
		}
		else
		{
			drawPixelXY(lamp_state->leds, x, HEIGHT - 1, 0x000000);
		}
	}
}
