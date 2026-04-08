#ifndef PREDICT_H
#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <vector>

typedef unsigned int uint;

#define ALPHABET_SIZE 256
#define RESULTSLIMIT 10000
#define QUANTIL 6.63

class Predictor {
    std::vector<std::vector<bool>> bytes;
    std::vector<uint> bytes_filled;
    uint lower_value_second_part, upper_value_second_part;
    uint step_second_part;
    uint SECOND_PART;
    uint G_PERIOD;
    bool resultsOfPrediction[RESULTSLIMIT];
    uint resultSize;
    std::vector<uint> count;
    uint inFirstPart, InSecondPart;
    std::vector<uint> data;
    std::vector<uint> point_in_data;
    uint data_size;

   public:
    void Init() {
        bytes.clear();
        bytes_filled.clear();
        count.clear();
        point_in_data.clear();
        resultSize = 0;
        inFirstPart = InSecondPart = 0;
    }

    Predictor() {
        resultSize = 0;
        Init();
    }

    ~Predictor() {}

    void SetSecondPart(const uint& _lower_value, const uint& _upper_value,
                       const uint& _step) noexcept {
        lower_value_second_part = _lower_value;
        upper_value_second_part = _upper_value;
        step_second_part = _step;
    }

    void SetPeriod(const uint& _period) {
        G_PERIOD = _period;
        Init();
    }

    void AddSymbol(uint symb) {
        data.push_back(symb);
        data_size += 1;
    }

    bool ChiSquare(uint num_window) {
        double chi;
        double probabiltyInG = 1.0 * bytes_filled[num_window] / ALPHABET_SIZE;
        double TheoreticalInFirstPart = (probabiltyInG)*SECOND_PART;
        double TheoreticalInSecondPart = (1.0 - probabiltyInG) * SECOND_PART;

        chi = (1.0 * TheoreticalInFirstPart - inFirstPart) *
              (1.0 * TheoreticalInFirstPart - inFirstPart) /
              TheoreticalInFirstPart;
        chi = chi + (1.0 * TheoreticalInSecondPart - InSecondPart) *
                        (1.0 * TheoreticalInSecondPart - InSecondPart) /
                        TheoreticalInSecondPart;
        return (chi > QUANTIL) ? true : false;
    }

    int Process(uint i, uint num_window) {
        count[num_window] += 1;
        if (count[num_window] > G_PERIOD) {
            if (resultSize >= RESULTSLIMIT) return -1;
            resultsOfPrediction[resultSize++] = ChiSquare(num_window);
            inFirstPart = 0;
            InSecondPart = 0;
            return 0;
        } else {
            if (count[num_window] > G_PERIOD - SECOND_PART) {
                if (bt(i, num_window)) {
                    inFirstPart++;
                } else {
                    InSecondPart++;
                }
            } else {
                bts(i % ALPHABET_SIZE, num_window);
            }
        }
        return 1;
    }

    void MainProcess() {
        size_t count_windows = floor((data_size * 1.0) / G_PERIOD);
        if (count_windows > RESULTSLIMIT) count_windows = RESULTSLIMIT;
        count = std::vector<uint>(count_windows, 0);
        bytes = std::vector<std::vector<bool>>(
            count_windows, std::vector<bool>(ALPHABET_SIZE, 0));
        bytes_filled = std::vector<uint>(count_windows, 0);
        point_in_data = std::vector<uint>(count_windows, 0);
        for (int i = 1; i < count_windows; ++i)
            point_in_data[i] = point_in_data[i - 1] + G_PERIOD + 1;

        for (int second_part = upper_value_second_part;
             second_part >= lower_value_second_part;
             second_part -= step_second_part) {
            SECOND_PART = second_part;
            resultSize = 0;

            for (int number_window = 0; number_window < count_windows;
                 ++number_window) {
                while (Process(data[point_in_data[number_window]],
                               number_window) > 0) {
                    point_in_data[number_window] += 1;
                }
                point_in_data[number_window] =
                    point_in_data[number_window] - SECOND_PART;
                if (number_window == 0 &&
                    second_part == upper_value_second_part)
                    point_in_data[number_window] += 1;
                count[number_window] = count[number_window] - SECOND_PART - 1;
            }

            ShowTotalStatistics();
        }
    }

    inline bool bt(unsigned int i, uint num_window) {
        return bytes[num_window][i];
    }

    void bts(uint i, uint num_window) {
        if (bytes[num_window][i]) return;
        bytes[num_window][i] = true;
        bytes_filled[num_window]++;
    }

#pragma region ShowStatistic
    void ShowTotalStatistics() {
        uint tmp = 0;
        for (uint i = 0; i < resultSize; i++) {
            tmp = (resultsOfPrediction[i]) ? tmp + 1 : tmp;
        }
        printf("SeconPart=%d g-period=%d - ", SECOND_PART, G_PERIOD);
        printf("Prediction probability %.3f, NumberOfBroken=%d from %d\n",
               1.0f * tmp / resultSize, tmp, resultSize);
    }
#pragma endregion
};
#endif
