#pragma once
#include <math.h>

// Деление по модулю с использованием ненулевого элемента
int mod_custom(int num, int max);

// Ограничение числа снизу по уровню 0
int _0(const int x);

// Обратное ограничение числа по уровню 0 с использованием abs
int _0ia(const int x);

double round(double number);