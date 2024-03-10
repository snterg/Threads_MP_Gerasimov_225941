#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

std::mutex mtx; // Мьютекс для синхронизации доступа к столу
std::condition_variable cv; // Условная переменная для синхронизации курильщиков и посредника
bool isIngredientAvailable = false; // Флаг, указывающий, есть ли доступные компоненты на столе
int selectedIngredient = -1; // Выбранный компонент на столе (-1 - выбранного компонента нет)

// Функция курильщика
void smoker(int id, const std::string& ingredient) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return isIngredientAvailable; });

        if (selectedIngredient == id) {
            std::cout << "Курильщик " << id << " скрутил сигарету из " << ingredient << " и начал курить." << std::endl;

            // Курильщик "курит" (задержка в выполнении)
            std::this_thread::sleep_for(std::chrono::seconds(2));

            std::cout << "Курильщик " << id << " закончил курить." << std::endl;
            isIngredientAvailable = false;
            selectedIngredient = -1;
            cv.notify_one();
        }
    }
}

// Функция посредника
void mediator() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 2);

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !isIngredientAvailable; });

        int ingredient1 = distribution(generator);
        int ingredient2 = distribution(generator);

        // Генерация двух разных случайных компонентов
        while (ingredient2 == ingredient1) {
            ingredient2 = distribution(generator);
        }

        // Помещение компонентов на стол
        std::cout << "Посредник поместил компоненты " << ingredient1 << " и " << ingredient2 << " на стол." << std::endl;
        selectedIngredient = ingredient1;
        isIngredientAvailable = true;
        cv.notify_all();
    }
}

int main() {
    setlocale(LC_ALL, "RUS");
    std::thread smokers[3];
    for (int i = 0; i < 3; ++i) {
        smokers[i] = std::thread(smoker, i, (i == 0 ? "табак" : (i == 1 ? "бумага" : "спички")));
    }

    std::thread mediatorThread(mediator);

    for (int i = 0; i < 3; ++i) {
        smokers[i].join();
    }
    mediatorThread.join();

    return 0;
}