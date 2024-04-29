#include <omp.h>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

int main() {
    const int num_items = 25;
    vector<int> values(num_items);
    vector<int> weights(num_items);
    int max_weight = 100;

    // Генеруємо випадкові ваги та вартості для предметів
    srand(42); // Задаємо seed для rand()
    for (int i = 0; i < num_items; ++i) {
        values[i] = rand() % 100 + 1;  // Вартість від 1 до 100
        weights[i] = rand() % 50 + 1;  // Вага від 1 до 50
    }
    

    int best_value = 0;

    // Початок вимірювання часу
    double start_time = omp_get_wtime();

    // Використовуємо OpenMP для паралельного перебору комбінацій
    omp_set_num_threads(1);//кількість ядер
    #pragma omp parallel for reduction(max: best_value)
    for (int i = 0; i < (1 << num_items); ++i) {
        int cw = 0, cv = 0;
        for (int j = 0; j < num_items; ++j) {
            if (i & (1 << j)) {
                cw += weights[j];
                cv += values[j];
            }
        }
        if (cw <= max_weight) {
            best_value = max(best_value, cv);
        }
    }
    // Закінчення вимірювання часу
    double end_time = omp_get_wtime();

    cout << "Maximum value in knapsack = " << best_value << endl;
    cout << "Execution time: " << end_time - start_time << " seconds." << endl;

    return 0;
}
