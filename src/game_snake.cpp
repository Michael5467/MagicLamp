#include <Arduino.h>
#include "functions.h"

#if (USE_SNAKE == 1)

// Snake: length, head, motion vector, body and tail:
uint16_t snakeLength;
uint16_t p_head; // Pointer to the p_head element in the 'body array'
uint16_t p_tail; // Pointer to the tail element in the 'body array'
int8_t headX;
int8_t headY;
int8_t tailX;
int8_t tailY;
// int8_t  vectorX, vectorY;
// Motion vector: 0 - right, 1 - left, 2 - up, 3 - down
snake_motion_t head_vector;
snake_motion_t body_vector[SNAKE_MAX_LENGTH];
boolean snake_increase;

// Apple: coordinates and flags
uint8_t appleX;
uint8_t appleY;
boolean apple_flag; // The apple is exist on the matrix; if not - draw it

uint32_t color_under_head;
boolean gameFail;

void snakeRoutine(the_lamp_state_t *lamp_state)
{
	if (lamp_state->loadingFlag)
	{
		lamp_state->loadingFlag = false;
		newGameSnake(lamp_state->leds);
	}

	newApple(lamp_state->leds);
	calculate_new_coordinate(&headX, &headY, body_vector[p_head]); // The snake movement: save the head turn and move it

	gameFail = check_fail(lamp_state->leds, headX, headY);

	// If fail - clear all and start it again
	if (gameFail)
	{
		lamp_state->loadingFlag = true;
		delay(3000);
		return;
	}

	// If we are here - the snake is still alive and moves!

	// Check whether we are eating an apple or not
	if (getPixelColorXY(lamp_state->leds, headX, headY) == SNAKE_APPLE_COLOR)
	{
		if (shift_pointer(p_head) == p_tail)
		{
			lamp_state->loadingFlag = true;
			delay(3000);
			return;
		}

		apple_flag = false;
		snakeLength++;
		snake_increase = true;
	}

	// Redraw the snake:
	// A 'new' head
	drawPixelXY(lamp_state->leds, headX, headY, SNAKE_BODY_COLOR);
	p_head = shift_pointer(p_head);

	// A 'new' tail: paint over it or not
	if (!snake_increase)
	{
		drawPixelXY(lamp_state->leds, tailX, tailY, SNAKE_FIELD_COLOR);
		calculate_new_coordinate(&tailX, &tailY, body_vector[p_tail]);
		p_tail = shift_pointer(p_tail);
	}
	snake_increase = false;

	body_vector[p_head] = snake_brain(lamp_state->leds);
}

// Calculate new coordinates from passed parameters with motion vector
void calculate_new_coordinate(int8_t *x, int8_t *y, snake_motion_t vector)
{
	switch (vector)
	{
	case s_v_Right:
		(*x)++;
		break;
	case s_v_Left:
		(*x)--;
		break;
	case s_v_Up:
		(*y)++;
		break;
	case s_v_Down:
		(*y)--;
		break;
	}
}

// Crash and boundary violation checking
// Return TRUE in the accident case and FALSE if everything is ok
boolean check_fail(CRGB *leds, int8_t x, int8_t y)
{
	uint32_t color_under_head;

	// Check boundaries
	if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1)
	{
		return true;
	}

	// Check the snake crashes into something but not into an apple
	color_under_head = getPixelColorXY(leds, x, y);
	if (color_under_head != SNAKE_FIELD_COLOR && color_under_head != SNAKE_APPLE_COLOR)
	{
		return true;
	}

	// No problems
	return false;
}

// Shift the pointer with rotation through an array boundary
uint16_t shift_pointer(uint16_t pointer)
{
	if (pointer == SNAKE_MAX_LENGTH - 1)
	{
		return 0;
	}
	return (pointer + 1);
}

// Change direction if the current course is wrong
// The changing occurs clockwise
snake_motion_t change_direction(snake_motion_t current_direction)
{
	switch (current_direction)
	{
		case s_v_Up:
			return s_v_Right;
		case s_v_Right:
			return s_v_Down;
		case s_v_Down:
			return s_v_Left;
		case s_v_Left:
			return s_v_Up;
	}

	// Impossible! We should not have been here...
	// Return something...
	return s_v_Up;
}

// The snake is thinking about next motion
snake_motion_t snake_brain(CRGB *leds)
{
	snake_motion_t desire_direction = s_v_Up;
	int8_t nextX = headX;
	int8_t nextY = headY;
	uint8_t count = 0;

	// Looking for the apple
	// First of all by horizontal line
	if (appleX < headX)
	{
		desire_direction = s_v_Left;
	}
	else if (appleX > headX)
	{
		desire_direction = s_v_Right;
	}
	else
	{
		// The apple is on the same line by vertically with snake head
		// Let's check the vertical line
		if (appleY < headY)
			desire_direction = s_v_Down;
		else if (appleY > headY)
			desire_direction = s_v_Up;
		else
			desire_direction = s_v_Up; // Impossible! We are on the apple! Let's select anything...
	}

	do
	{
		if (count > 0)
			desire_direction = change_direction(desire_direction);
		count++;

		// Get the following possible coordinate
		nextX = headX;
		nextY = headY;
		calculate_new_coordinate(&nextX, &nextY, desire_direction);
	} while (check_fail(leds, nextX, nextY) && (count <= 3));

	return desire_direction;
}

// Creating a new apple
void newApple(CRGB *leds)
{
	while (!apple_flag)
	{
		// Randomly generate coordinates of the apple
		appleX = random(0, WIDTH);
		appleY = random(0, HEIGHT);

		// Check that the apple's coordinate doesn't match with the snake's body
		if (getPixelColorXY(leds, appleX, appleY) == SNAKE_FIELD_COLOR)
		{
			apple_flag = true;
			drawPixelXY(leds, appleX, appleY, SNAKE_APPLE_COLOR);
		}
	}
}

// Creating a new snake in the matrix's centre
void newSnake(CRGB *leds)
{
	randomSeed(millis());

	snakeLength = SNAKE_START_LENGTH;
	p_head = snakeLength - 1;
	p_tail = 0;
	snake_increase = false;

	// Head: centre of the matrix
	headX = WIDTH / 2;
	headY = HEIGHT / 2;

	// Body: vertical down
	// Motion vector: up
	for (uint8_t i = 0; i < snakeLength; i++)
	{
		drawPixelXY(leds, headX, headY - i, SNAKE_BODY_COLOR);
		body_vector[i] = s_v_Up;
	}

	// Tail: lower by X in start length
	tailX = headX;
	tailY = headY - snakeLength + 1;

	// Motion vector of head: up
	head_vector = s_v_Up;
}

// Creating a new game: new snake, new appel and show the matrix
void newGameSnake(CRGB *leds)
{
	randomSeed(analogRead(0) + analogRead(1) + analogRead(2) + analogRead(3) + millis());
	gameFail = false;
	apple_flag = false;
	fillAllbyColor(leds, SNAKE_FIELD_COLOR);
	newSnake(leds);
	newApple(leds);
	FastLED.show();
}

#elif (USE_SNAKE == 0)

void snakeRoutine()
{
	return;
}

#endif
