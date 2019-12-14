#include <cstdlib>
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

atomic<unsigned int> do_while_counter {};
class spin_lock 
{
    atomic<unsigned int> m_spin ;
public:
    spin_lock(): m_spin(0) {}
    ~spin_lock() { assert( m_spin.load(memory_order_relaxed) == 0);}

    void lock()
    {
        unsigned int nCur;
        do { nCur = 0; 
            do_while_counter.fetch_add(1);} // устанавливаем "старое значение" в 0
        while ( !m_spin.compare_exchange_weak( nCur, 1, memory_order_acquire )); 
        // если значение m_spin == 0 то меняем m_spin на 1 и выходим из цикла
        // если значение m_spin == 1 то меняем nCur на 1 и идем на еще один цикл
    }
    void unlock()
    {
        m_spin.store( 0, memory_order_release );
    }
};

int value=0;
spin_lock lck;



void add(){
    lck.lock();
    value++;
    lck.unlock();
}

int value_mtx=0;
std::mutex mtx;
void add_mutex(){
    std::lock_guard<std::mutex> lck(mtx);
    value_mtx++;
}


int main(int argc, char** argv) {

    std::vector<std::thread*> threads;
    
    auto begin = std::chrono::high_resolution_clock::now();
    for(int i=0;i<10000;i++) threads.push_back(new std::thread(add));
    for(auto t:threads) t->join();
    auto end =std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast <std::chrono::milliseconds>(end - begin).count() << std::endl;

    threads.clear();
    begin = std::chrono::high_resolution_clock::now();
    for(int i=0;i<10000;i++) threads.push_back(new std::thread(add_mutex));
    for(auto t:threads) t->join();
    end =std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast <std::chrono::milliseconds>(end - begin).count() << std::endl;
    
    std::cout << "Values:" << value << " ," <<value_mtx << std::endl;
    std::cout << "While loops:" << do_while_counter.load() << std::endl;
    return 0;
}

