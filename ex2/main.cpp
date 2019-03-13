// Histogramming.cpp : Defines the entry point for the console application.
//
#include <thread>
#include "stdio.h"
#include <atomic>

// Эта структура описывает задание на построение гистограммы для потока
struct HistogrammingTask {
    char *data;                    // Исходные данные, по которым нужно считать гистограмму
    unsigned int from;            // Начальный индекс, с которого поток должен начать вычисления (включается в вычисления)
    unsigned int to;            // Конечный индекс, перед которым поток должен остановится (не включается в вычисления)
    std::atomic<long> *histogramm;            // Результирующая гистограмма
};


// Функция потока для вычисления гистограммы
unsigned int histogramming_thread(void *param) {
    auto *task = static_cast<HistogrammingTask *>(param);

    // Основной цикл вычисления гистограммы.
    for (unsigned int i = task->from; i < task->to; i++) { // Нужно исправить код в этой строке
        std::atomic_fetch_add(&task->histogramm[task->data[i]], static_cast<long>(1));

    }

    delete task;        // За удаление структуры-задания отвечает поток.
    return 0;
}


int main() {
    const unsigned int data_size = 1024 * 1024 * 50;        // Размер и массив исходных данных
    auto *data = new char[data_size];

    const int num_bins = 20;            // Размер гистограммы (и одновременно максимальное значение данных)
    auto* histogramm = new std::atomic<long>[num_bins];        // Гистограмма, = {} инициализирует массив нулями.

    for( int i = 0; i < num_bins; i++ ){
        std::atomic_init(&histogramm[i], static_cast<long>(0));

    }

    // Заполнение исходных данных
    for (unsigned int i = 0; i < data_size; i++)
        data[i] = (char) (i % num_bins);

    // Число потоков
    unsigned int num_threads;

    printf("Enter number of threads: ");
    scanf("%u", &num_threads);

    auto *threads = new std::thread[num_threads];

    // Запуск потоков
    for (unsigned int thread = 0; thread < num_threads; thread++) {
        auto *task = new HistogrammingTask;

        task->data = data;
        task->histogramm = histogramm;
        task->from = data_size / num_threads * thread;
        if (thread != num_threads - 1) {
            task->to = data_size / num_threads * (thread + 1);
        } else {
            task->to = data_size;
        }

        threads[thread] = std::thread(histogramming_thread, task);
    }

    // Ожидание завершения вычислений
    for (int i = 0; i < num_threads; i++){
        threads[i].join();
    }

    // Печать результата
    for (int i = 0; i < num_bins; i++) {
        printf("%ld ", std::atomic_load(&histogramm[i]));
    }

    // Освобождение памяти
    delete[] threads;
    delete[] data;

    return 0;
}

