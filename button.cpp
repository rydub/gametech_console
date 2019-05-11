#include <Arduino.h>
#include "button.h"


int check_button1() {
	return digitalRead(B1);
}