#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const int num_items = 25;
    vector<int> values(num_items), weights(num_items);

    if (world_rank == 0) {
        srand(42);
        for (int i = 0; i < num_items; ++i) {
            values[i] = rand() % 100 + 1;
            weights[i] = rand() % 50 + 1;
        }
    }

    MPI_Bcast(values.data(), num_items, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(weights.data(), num_items, MPI_INT, 0, MPI_COMM_WORLD);

    int max_weight = 100;
    int best_value = 0;
    int local_best_value = 0;

    unsigned long long total_combinations = 1ULL << num_items;
    unsigned long long range_size = total_combinations / world_size;
    
    unsigned long long start_index, end_index;
    double start_time = MPI_Wtime();
    // Process 0 distributes the ranges to each process
    if (world_rank == 0) {
        for (int i = 0; i < world_size; i++) {
            start_index = i * range_size;
            end_index = (i == world_size - 1) ? total_combinations : start_index + range_size;
            if (i == 0) {
                continue; // Process 0 already knows its range
            }
            // Send start_index and end_index to each process
            MPI_Send(&start_index, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end_index, 1, MPI_UNSIGNED_LONG_LONG, i, 1, MPI_COMM_WORLD);
        }
    } else {
        // Receive start_index and end_index from process 0
        MPI_Recv(&start_index, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end_index, 1, MPI_UNSIGNED_LONG_LONG, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }


    // Compute best_value for the given range
    for (unsigned long long k = start_index; k < end_index; ++k) {
        int current_weight = 0, current_value = 0;
        for (int j = 0; j < num_items; ++j) {
            if (k & (1ULL << j)) {
                current_weight += weights[j];
                current_value += values[j];
            }
        }
        if (current_weight <= max_weight) {
            local_best_value = max(local_best_value, current_value);
        }
    }

    MPI_Reduce(&local_best_value, &best_value, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (world_rank == 0) {
        cout << "Maximum value in knapsack = " << best_value << endl;
        cout << "Execution time: " << end_time - start_time << " seconds." << endl;
    }

    MPI_Finalize();
    return 0;
    }