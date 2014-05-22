#include "utility.h"

int mod_custom(int num, int max) {

	int t = num%max;
	return !t ? (!num ? 0 : num/abs(num)*max) : t;

};

int _0(const int x) {
	return x < 0 ? 0 : x;
}

int _0ia(const int x) {
	return x < 0 ? abs(x) : 0;
}

double round(double number) {
	return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}