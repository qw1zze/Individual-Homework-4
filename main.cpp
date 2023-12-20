#include <iostream>
#include <pthread.h>
#include <vector>
#include "random.cpp"

// Максимальные значения для пчел и участков
const int MAX_NUM_BEES = 100;
const int MAX_NUM_SECTORS = 1000;

// Секторы для проверки
std::vector<int> exploredSectors;
FILE *file_out;
pthread_mutex_t mutex;

// Расположение Винни Пуха
int vinniPosition = 0;

// Ошибка некорреткного ввода в командной строке
void error1() {
    printf("Incorrect command line!\n"
           "  Waited:\n"
           "     command -f input_file output_file\n"
           "  Or:\n"
           "     command -n <number1> <number2> output_file\n");
}

// Ошибка некорректного значения в командной строке
void error2() {
    printf("Incorrect qualifier value!\n"
           "  Waited:\n"
           "     command -f input_file output_file\n"
           "  Or:\n"
           "     command -n <number1> <number2> output_file\n");
}

// Ошибка некорректного значения
void error3() {
    printf("Incorrect data format in the file!\n");
}

// Ошибка неверного числа пчел
void error4() {
    printf("Incorrect number of bees. Set 0 < number <= %d\n", MAX_NUM_BEES);
}

// Ошибка неверного числа участков
void error5() {
    printf("Incorrect pot sectors. Set 0 < number <= %d\n", MAX_NUM_SECTORS);
}

// Считывает количество пчёл и учатсков из файла.
bool readArgs(FILE *file_in, int &bees, int &sectors) {
    if (fscanf(file_in, "%d", &bees) != EOF) {
        fscanf(file_in, "%d", &sectors);
        return true;
    }
    return false;
}


// Получает значения из командной строки.
int getData(int argc, char *argv[], int &bees, int &sectors) {
    if (argc != 5 && argc != 4) {
        error1();
        return 1;
    }

    if (argc == 4 && !strcmp(argv[1], "-f")) {
        FILE *file = fopen(argv[2], "r");
        if (file == nullptr) {
            printf("Cannot find file with name %s\n", argv[2]);
            return 7;
        }
        if (!readArgs(file, bees, sectors)) {
            error3();
            return 3;
        }
        fclose(file);
    } else if (argc == 5 && !strcmp(argv[1], "-n")) {
        bees = atoi(argv[2]);
        sectors = atoi(argv[3]);
    } else {
        error2();
        return 2;
    }

    if (bees <= 0 || bees > MAX_NUM_BEES) {
        error4();
        return 4;
    }
    if (sectors <= 0 || sectors > MAX_NUM_SECTORS) {
        error5();
        return 5;
    }
    return 0;
}

// Имитация работы одной стаи пчел
void *beeThread(void *arg) {
    int beeId = *((int *) arg);

    while (true) {
        pthread_mutex_lock(&mutex);
        if (exploredSectors.size() == 0) {
            std::cout << "Стая пчел № " + std::to_string(beeId) + " закончила поиски.\n";
            std::fprintf(file_out, "Стая пчел № %d закончила поиски.\n", beeId);
            pthread_mutex_unlock(&mutex);
            break;
        }

        int currentSector = exploredSectors.back();
        exploredSectors.pop_back();

        std::cout << "Стая пчел № " + std::to_string(beeId) + " исследует участок " + std::to_string(currentSector) + "\n";
        std::fprintf(file_out, "Стая пчел № %d исследует участок %d\n", beeId, currentSector);
        if (vinniPosition == currentSector) {
            std::cout << "Стая пчел № " + std::to_string(beeId) + " нашла Винни Пуха на участке " +
                         std::to_string(currentSector) + "! Показательно наказали и возращаются.\n";
            std::fprintf(file_out, "Стая пчел № %d нашла Винни Пуха на участке %d! Показательно наказали и возращаются.\n", beeId, currentSector);
            // Перемещение Винни Пуха в непроверенный участок
            if (exploredSectors.size() == 0) {
                std::cout << "Винни Пух ушел из леса!\n";
                std::fprintf(file_out, "Винни Пух ушел из леса!\n");
            } else {
                auto rand = Random(0, exploredSectors.size() - 1);
                vinniPosition = exploredSectors[rand.get()];
                std::cout << "Винни Пух перешел на участок " + std::to_string(vinniPosition) + "\n";
                std::fprintf(file_out, "Винни Пух перешел на участок %d\n", vinniPosition);
                pthread_mutex_unlock(&mutex);
            }
        } else {
            pthread_mutex_lock(&mutex);
            std::cout << "Стая пчел № " + std::to_string(beeId) + " не нашли Винни Пуха на участке " +
                         std::to_string(currentSector) + "\n";
            std::fprintf(file_out, "Стая пчел № %d не нашли Винни Пуха на участке %d\n", beeId, currentSector);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int numbBees, sectors;

    int res = getData(argc, argv, numbBees, sectors);
    if (res) {
        return 0;
    }

    pthread_t bees[numbBees];
    int beeIds[numbBees];

    for (int i = sectors; i >= 1; --i) {
        exploredSectors.push_back(i);
    }

    // Запись в файл
    file_out = fopen(argv[argc - 1], "w");

    // Первое перемещение Винни Пуха
    auto rand = Random(0, sectors);
    vinniPosition = exploredSectors[rand.get()];
    std::cout << "Винни Пух перешел на участок " + std::to_string(vinniPosition) + "\n";
    std::fprintf(file_out, "Винни Пух перешел на участок %d\n", vinniPosition);

    // Инициализация потоков
    for (int i = 0; i < numbBees; ++i) {
        beeIds[i] = i + 1;
        pthread_create(&bees[i], nullptr, beeThread, (void *) &beeIds[i]);
    }

    // Ожидание завершения потоков
    for (int i = 0; i < numbBees; ++i) {
        pthread_join(bees[i], nullptr);
    }
    return 0;
}
