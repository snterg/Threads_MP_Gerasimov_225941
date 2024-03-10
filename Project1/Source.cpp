#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>

// Функция, выполняемая каждым потоком
void findEvenSum(const std::vector<int>& array, std::atomic<int>& sum, int start, int end) {
    std::thread::id threadId = std::this_thread::get_id();
    int localSum = 0;
    for (int i = start; i < end; ++i) {
        if (array[i] % 2 == 0) {
            localSum += array[i];
        }
    }
    sum += localSum;
    std::cout << "Поток " << threadId << " завершил работу" << std::endl;
}

int maiвn() {

    std::random_device rd;
    std::mt19937 generator(rd()); // Используем std::mt19937 в качестве генератора
    setlocale(LC_ALL, "RUS");
    std::vector<int> array;
    // Определение диапазона случайных чисел
    int minNumber = 1;
    int maxNumber = 1e10;
    std::uniform_int_distribution<int> distribution(minNumber, maxNumber);
    for (int i = 0; i < 1000000; ++i)
    {
        array.push_back(distribution(generator));
    }
    int numThreads = 4; // Количество потоков
    std::atomic<int> sum(0); // Атомарная переменная для хранения суммы

    std::vector<std::thread> threads;
    int size = array.size();
    int blockSize = size / numThreads;
    int start = 0;
    int end = blockSize;

    // Создание и запуск потоков
    for (int i = 0; i < numThreads; ++i) {
        if (i == numThreads - 1) {
            end = size;
        }
        threads.emplace_back(findEvenSum, std::ref(array), std::ref(sum), start, end);
        start = end;
        end += blockSize;
    }

    // Дождитесь завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Сумма четных чисел: " << sum << std::endl;

    return 0;
}