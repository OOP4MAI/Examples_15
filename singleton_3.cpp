#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <condition_variable>
#include <vector>

struct sync_stream : std::stringstream{
    inline static std::mutex mtx;
    ~sync_stream(){
        std::lock_guard<std::mutex> lck(mtx);
        std::cout << this->rdbuf();
        std::cout.flush();
    }
};


class Singleton
{
private:
    inline static std::atomic<Singleton*>    instance;
    inline static std::atomic<bool>          create {false};
    Singleton(){
        sync_stream{} << "created\n";
    };

public:
    static Singleton *get_instance(){
        if (instance.load() == nullptr) {
        if (!create.exchange(true)) // сохраняем истину, ожидаем предыдущее значение 1
            instance.store(new Singleton()); // construct
            else while(instance.load() == nullptr) { } // spin
        }
        return instance.load();
    }
};

std::condition_variable cv;
std::mutex              cv_mutex;
bool                    start{false};

auto main() -> int
{
    std::vector<std::thread*> array;
    for(size_t i=0;i<1000;++i) array.push_back(new std::thread([]{
                                                                    std::unique_lock<std::mutex> lck(cv_mutex);
                                                                    cv.wait(lck,[](){return start;});
                                                                    (void)Singleton::get_instance();
                                                                }));

    {
        std::unique_lock<std::mutex> lck;

        sync_stream{} << "Press any key";
        std::cin.get();

        start = true;
        cv.notify_all();
    }
    for(auto *pt: array){
        pt->join();
        delete pt;
    }                                                           
    return 1;
}