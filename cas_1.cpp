#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>

struct Counter {
    int             value{};
    std::atomic_int value_a{};
};

std::mutex mtx;
std::condition_variable cv;
Counter cnt;

void add_func(bool atomic) {
    {
        std::unique_lock<std::mutex> lock(mtx); 
        cv.wait(lock); // ждем начала работы
    }
    if (atomic) { // делаем 1000 атомарных добавлений
        for (int i = 0; i < 1000; i++)
            cnt.value_a.fetch_add(1);
    } else { // делаем 1000 не-атомарных добавлений
        for (int i = 0; i < 1000; i++)
            ++ cnt.value;
    }

}

int main(int argc, char** argv) {
    std::vector<std::thread*> threads;


    for (int i = 0; i < 1000; i++) // запускаем тасячу потоков c атомарным добавлением
        threads.push_back(new std::thread(add_func, true));
    
    for (int i = 0; i < 1000; i++) // запускаем тасячу потоков с обычным добавлением
        threads.push_back(new std::thread(add_func, false));

    {
        std::cout << "Press any key" << std::endl;
        std::cin.get();
        std::unique_lock<std::mutex> lock(mtx);
        cv.notify_all(); // запускаем процесс
    }

    for (auto a : threads) a->join();
    std::cout << "Normal: " << cnt.value << std::endl;
    std::cout << "Atomic: " << cnt.value_a << std::endl;
    return 0;
}

