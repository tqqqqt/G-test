#ifndef PREDICT_H
#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <stdexcept>
#include <vector>

typedef unsigned int uint;

#define ALPHABET_SIZE 256
#define RESULTS_LIMIT 10000
#define QUANTIL 6.63

class Predictor {
    std::vector<std::vector<bool>> bytes;
    std::vector<uint> bytes_filled;
    std::vector<uint> count;
    uint in_first_part, in_second_part;

    uint lower_value_second_part, upper_value_second_part;
    uint step_second_part;
    uint second_part;
    uint g_period;

    bool results_of_prediction[RESULTS_LIMIT];
    uint result_size;

    std::vector<uint> data;
    std::vector<uint> point_in_data;
    uint data_size;

   public:
    void Init() {
        bytes.clear();
        bytes_filled.clear();
        count.clear();
        point_in_data.clear();
        result_size = 0;
        in_first_part = in_second_part = 0;
    }

    Predictor() { Init(); }

    ~Predictor() {}

    void SetSecondPart(const int& _lower_value, const int& _upper_value,
                       const int& _step){
        if(_lower_value<0 || _upper_value<=0 || _step<=0) throw std::runtime_error("second part value < 0");
        
        if(_lower_value>_upper_value) {
            lower_value_second_part = _upper_value;
            upper_value_second_part = _lower_value;
        }
        else {
            lower_value_second_part = _lower_value;
            upper_value_second_part = _upper_value;
        }
        step_second_part = _step;
    }

    void SetPeriod(const int& _period) {
        if(_period<=0) throw std::runtime_error("period < 0");

        g_period = _period;
        Init();
    }

    void AddSymbol(const uint& symb) {
        if(data_size==data.max_size()) return;
        
        data.push_back(symb);
        data_size += 1;
    }

   private:
    bool ChiSquare(const uint& num_window) {
        double chi;
        double probabiltyInG = 1.0 * bytes_filled[num_window] / ALPHABET_SIZE;
        double TheoreticalInFirstPart = (probabiltyInG)*second_part;
        double TheoreticalInSecondPart = (1.0 - probabiltyInG) * second_part;

        chi = (1.0 * TheoreticalInFirstPart - in_first_part) *
              (1.0 * TheoreticalInFirstPart - in_first_part) /
              TheoreticalInFirstPart;
        chi = chi + (1.0 * TheoreticalInSecondPart - in_second_part) *
                        (1.0 * TheoreticalInSecondPart - in_second_part) /
                        TheoreticalInSecondPart;
        return (chi > QUANTIL) ? true : false;
    }

    int Process(const uint& i, const uint& num_window) {
        count[num_window] += 1;
        if (count[num_window] > g_period) {
            if (result_size >= RESULTS_LIMIT) return -1;
            results_of_prediction[result_size++] = ChiSquare(num_window);
            in_first_part = 0;
            in_second_part = 0;
            return 0;
        } else if (count[num_window] > g_period - second_part) {
            if (bt(i, num_window))
                in_first_part++;
            else
                in_second_part++;
        } else
            bts(i % ALPHABET_SIZE, num_window);

        return 1;
    }

   public:
    void MainProcess() {
        size_t count_windows = floor((data_size * 1.0) / g_period);
        if (count_windows > RESULTS_LIMIT) count_windows = RESULTS_LIMIT;

        count = std::vector<uint>(count_windows, 0);
        bytes = std::vector<std::vector<bool>>(
            count_windows, std::vector<bool>(ALPHABET_SIZE, 0));
        bytes_filled = std::vector<uint>(count_windows, 0);
        point_in_data = std::vector<uint>(count_windows, 0);
        for (int i = 1; i < count_windows; ++i)
            point_in_data[i] = point_in_data[i - 1] + g_period + 1;

        for (int temp_second_part = upper_value_second_part;
             temp_second_part >= lower_value_second_part;
             temp_second_part -= step_second_part) {
            second_part = temp_second_part;
            result_size = 0;

            for (int number_window = 0; number_window < count_windows;
                 ++number_window) {
                while (Process(data[point_in_data[number_window]],
                               number_window) > 0) {
                    point_in_data[number_window] += 1;
                }
                point_in_data[number_window] =
                    point_in_data[number_window] - second_part;
                if (number_window == 0 &&
                    second_part == upper_value_second_part)
                    point_in_data[number_window] += 1;
                count[number_window] = count[number_window] - second_part - 1;
            }

            ShowTotalStatistics();
        }
    }

   private:
    inline bool bt(const uint& i, const uint& num_window) {
        return bytes[num_window][i];
    }

    void bts(const uint& i, const uint& num_window) {
        if (bytes[num_window][i]) return;
        bytes[num_window][i] = true;
        bytes_filled[num_window]++;
    }

#pragma region ShowStatistic
    void ShowTotalStatistics() {
        uint tmp = 0;
        for (uint i = 0; i < result_size; i++) {
            tmp = (results_of_prediction[i]) ? tmp + 1 : tmp;
        }
        printf("SeconPart=%d g-period=%d - ", second_part, g_period);
        printf("Prediction probability %.3f, NumberOfBroken=%d from %d\n",
               1.0f * tmp / result_size, tmp, result_size);
    }
#pragma endregion
};
#endif
