#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <thread>
using namespace std;

//Очистка экрана
void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\x1B[2J\x1B[H"; //Альтернатива: system("clear")
#endif
}
// Создание поля путем чтения из файла и создание второго массива
void creationOfTheUniverse(char**& universe, char**& newUniverse, int& rows, int& cols, const string& filename) {
    ifstream data(filename);
    data >> rows >> cols;

    universe = new char* [rows];
    for (int i = 0; i < rows; i++) {
        universe[i] = new char[cols];
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            universe[i][j] = '-';
        }
    }

    int x, y;
    while (data >> x >> y) {
        if (x < rows && x >= 0 && y < cols && y >= 0) {
            universe[x][y] = '*';
        }
    }
    data.close();
    //второй массив
    newUniverse = new char* [rows];
    for (int i = 0; i < rows; i++) {
        newUniverse[i] = new char[cols];
    }
}
// Просчет соседей
int countLivingNeighbors(char** universe, int rows, int cols, int x, int y) {
    //счетчик живых соседей
    int count = 0;
    //Пробегаем в окрестностях от -1 до +1 относительно клетки по x
    for (int dx = -1; dx <= 1; ++dx) {
        //Пробегаем в окрестностях от -1 до +1 относительно клетки по y
        for (int dy = -1; dy <= 1; ++dy) {
            //вычисление координат соседней клетки
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || nx >= rows || ny < 0 || ny >= cols) {
                continue; /* Пропускаем клетку, если она находится за пределами поля,
                            то есть пропуск краёв вселенной */
            }
            //пропуск центра
            if (dx == 0 && dy == 0) {
                continue;
            }
            //если сосед живой, то увеличиваем счетчик
            if (universe[nx][ny] == '*') {
                count++;
            }
        }
    }
    //возвращаем счетчик
    return count;
}
// Вывод поля на экран
int outputOnDisplay(char** universe, int rows, int cols, int& livingCells) {
    livingCells = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << universe[i][j] << " ";
            if (universe[i][j] == '*') {
                livingCells++;
            }
        }
        cout << endl;
    }
    return livingCells;
}
// Применение правил игры
void applyRules(char** universe, char** newUniverse, int rows, int cols) {

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int livingNeighbors = countLivingNeighbors(universe, rows, cols, i, j);
            if (universe[i][j] == '*') {
                if (livingNeighbors < 2 || livingNeighbors > 3) {
                    newUniverse[i][j] = '-';
                }
                else {
                    newUniverse[i][j] = '*';
                }
            }
            else if (universe[i][j] == '-' && livingNeighbors == 3) {
                newUniverse[i][j] = '*';
            }
            else {
                newUniverse[i][j] = universe[i][j];
            }
        }
    }
}
//Обновление поля
void update(char** universe, char** newUniverse, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            universe[i][j] = newUniverse[i][j];
        }
    }
}
// Проверка массивов на стагнацию
bool coincidence(char** universe, char** newUniverse, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (newUniverse[i][j] != universe[i][j])
                return false;
        }
    }
    return true;
}
// Очистка памяти
void freeingMemory(char** universe, int rows) {
    for (int i = 0; i < rows; i++) {
        delete[] universe[i];
    }
    delete[] universe;
}

int main() {
    setlocale(LC_ALL, "Russian");
    char** universe;
    char** newUniverse;
    int rows = 0;
    int cols = 0;
    int livingCells = 0;
    int generation = 0;

    creationOfTheUniverse(universe, newUniverse, rows, cols, "set.txt");

    while (true) {
        generation++;
        clear_screen();
        cout << "Поколение: " << generation << endl;
        outputOnDisplay(universe, rows, cols, livingCells);
        applyRules(universe, newUniverse, rows, cols);

        if (livingCells == 0) {
            cout << "Все клетки мертвы. Игра окончена." << endl;
            break;
        }
        else if (generation > 1 && coincidence(universe, newUniverse, rows, cols)) {
            cout << "Живых клеток: " << livingCells << endl;
            cout << "Стагнация. Игра окончена." << endl;
            break;
        }
        else
            cout << "Живых клеток: " << livingCells << endl;

        update(universe, newUniverse, rows, cols);
        this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    freeingMemory(universe, rows);
    freeingMemory(newUniverse, rows);
    return 0;
}