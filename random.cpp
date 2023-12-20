#include <stdlib.h>
#include <time.h>

#ifndef CSA_HW5_RANDOM_H
#define CSA_HW5_RANDOM_H

// Генерация рандомных чисел.
class Random {
private:
    // Нижняя граница генирации рандомных чисел.
    int min_;
    // Верхняя граница генирации рандомных чисел.
    int max_;

public:
    Random(int min, int max) {
        min_ = min;
        max_ = max;
        // Системные часы в качестве инициализатора.
        srand(static_cast<unsigned int>(time(0)));
    }

    // Генерация случайного числа в заданном диапазоне
    int get() const {
        return rand() % (max_ - min_ + 1) + min_;
    }
};

#endif //CSA_HW5_RANDOM_H
