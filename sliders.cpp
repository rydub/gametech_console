#include <Arduino.h>
#include "sliders.h"

int left_level_check() {
	return analogRead(LEFT_PIN) / 256;
}

int right_level_check() {
	return analogRead(RIGHT_PIN) / 256;
}
