#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

std::mutex mtx; // ������� ��� ������������� ������� � �����
std::condition_variable cv; // �������� ���������� ��� ������������� ����������� � ����������
bool isIngredientAvailable = false; // ����, �����������, ���� �� ��������� ���������� �� �����
int selectedIngredient = -1; // ��������� ��������� �� ����� (-1 - ���������� ���������� ���)

// ������� ����������
void smoker(int id, const std::string& ingredient) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return isIngredientAvailable; });

        if (selectedIngredient == id) {
            std::cout << "��������� " << id << " ������� �������� �� " << ingredient << " � ����� ������." << std::endl;

            // ��������� "�����" (�������� � ����������)
            std::this_thread::sleep_for(std::chrono::seconds(2));

            std::cout << "��������� " << id << " �������� ������." << std::endl;
            isIngredientAvailable = false;
            selectedIngredient = -1;
            cv.notify_one();
        }
    }
}

// ������� ����������
void mediator() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 2);

    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !isIngredientAvailable; });

        int ingredient1 = distribution(generator);
        int ingredient2 = distribution(generator);

        // ��������� ���� ������ ��������� �����������
        while (ingredient2 == ingredient1) {
            ingredient2 = distribution(generator);
        }

        // ��������� ����������� �� ����
        std::cout << "��������� �������� ���������� " << ingredient1 << " � " << ingredient2 << " �� ����." << std::endl;
        selectedIngredient = ingredient1;
        isIngredientAvailable = true;
        cv.notify_all();
    }
}

int main() {
    setlocale(LC_ALL, "RUS");
    std::thread smokers[3];
    for (int i = 0; i < 3; ++i) {
        smokers[i] = std::thread(smoker, i, (i == 0 ? "�����" : (i == 1 ? "������" : "������")));
    }

    std::thread mediatorThread(mediator);

    for (int i = 0; i < 3; ++i) {
        smokers[i].join();
    }
    mediatorThread.join();

    return 0;
}