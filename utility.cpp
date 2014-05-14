#include "utility.h"

int mod_custom(int num, int max) {

	int t = num%max;
	return !t ? (!num ? 0 : num/abs(num)*max) : t;

};