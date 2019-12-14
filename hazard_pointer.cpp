#include <thread>
#include <atomic>
#include <sstream>
#include <iostream>

struct sync_stream : std::stringstream
{
    inline static std::mutex mtx;
    ~sync_stream()
    {
        std::lock_guard<std::mutex> lck(mtx);
        std::cout << this->rdbuf();
        std::cout.flush();
    }
};

class HazardPointer
{
private:
    HazardPointer   *next;
    std::atomic_bool active;

    inline static std::atomic<HazardPointer *> head{nullptr};
    inline static std::atomic_int32_t list_length{};

public:
    void *hazard; // data

    HazardPointer() : next(nullptr), active(false){};

    static HazardPointer *get_head()
    {
        return head;
    }

    static HazardPointer *acquire()
    {
        HazardPointer *p = head;
        for (; p; p = p->next)
        {
            if (p->active)
                continue; // CAS?
            return p;
        }

        int old_length{};
        do
        {
            old_length = list_length.load();
        } while (list_length.compare_exchange_strong(old_length, old_length + 1));

        p = new HazardPointer();
        p->active = true;
        p->hazard = nullptr;

        HazardPointer *old{};
        do{
            old = head;
        } while (head.compare_exchange_strong(old, p));
        return p;
    }

    static void release(HazardPointer *ptr){
        ptr->hazard = nullptr;
        ptr->active = false;
    }
};

struct Retired
{
    inline static const size_t R = 100;
    inline static const size_t N = 100;
    inline static const size_t K = 100;
    unsigned dcount = 0;

    // массив готовых к удалению данных
    void *dlist[R];
    void RetireNode(void *node)
    {
        dlist[dcount++] = node;
        // Если массив заполнен – вызываем основную функцию Scan
        if (dcount == R)
            Scan();
    }

    // Основная функция
    // Удаляет все элементы массива dlist, которые не объявлены
    // как Hazard Pointer
    void Scan()
    {
        unsigned i;
        unsigned p = 0;
        unsigned new_dcount = 0; // 0 .. N
        void *hptr;
        void *plist[N];
        void *new_dlist[N];

        // Stage 1 – проходим по всем HP всех потоков
        // Собираем общий массив plist защищенных указателей
        for (unsigned t = 0; t < P; ++t)
        {
            void **pHPThread = get_thread_data(t)->HP;
            for (i = 0; i < K; ++i)
            {
                hptr = pHPThread[i];
                if (hptr != nullptr)
                    plist[p++] = hptr;
            }
        }

        // Stage 2 – сортировка hazard pointer'ов
        // Сортировка нужна для последующего бинарного поиска
        std::sort(plist);

        // Stage 3 – удаление элементов, не объявленных как hazard
        for (i = 0; i < R; ++i)
        {
            // Если dlist[i] отсутствует в списке plist всех Hazard Pointer’ов
            // то dlist[i] может быть удален
            if (std::binary_search(dlist[i], plist))
                new_dlist[new_dcount++] = dlist[i];
            else
                free(dlist[i]);
        }

        // Stage 4 – формирование нового массива отложенных элементов.
        for (i = 0; i < new_dcount; ++i)
            dlist[i] = new_dlist[i];
        dcount = new_dcount;
    }
};

auto main() -> int
{
    return 1;
}