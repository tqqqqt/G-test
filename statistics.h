#ifndef STATISTICS_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

#define ALPHABET_SIZE 256
#define STATISTICS_H
#define PERIODSLIMIT 1000000

class Statistics {
    bool bytes[ALPHABET_SIZE];
    uint bytes_filled;
    uint GSize;                   // length of g-period
    uint GPeriods[PERIODSLIMIT];  // all g-periods in processed file
    uint GPeriodsSize;            // array size

   public:
    void Init() {
        for (int i = 0; i < ALPHABET_SIZE; ++i) bytes[i] = 0;
        bytes_filled = 0;
        GSize = 0;
    }

    Statistics() {
        GPeriodsSize = 0;
        Init();
    }

    ~Statistics() {}

    void OnGPeriodFound() {
        if (GPeriodsSize >= PERIODSLIMIT) return;
        GPeriods[GPeriodsSize++] = GSize;
        Init();
    }

    void Process(uint i) {
        GSize++;
        bts(i % ALPHABET_SIZE);
    }

    inline bool bt(unsigned int i) { return bytes[i]; }

    void bts(uint i) {
        if (bt(i)) return;
        bytes_filled++;
        bytes[i] = true;
        if (bytes_filled >= ALPHABET_SIZE) {
            OnGPeriodFound();
        }
    }

#pragma region ShowStatistic
    uint min() {
        uint tmp = GPeriods[0];
        for (uint i = 1; i < GPeriodsSize; i++) {
            if (tmp > GPeriods[i]) {
                tmp = GPeriods[i];
            }
        }
        return tmp;
    }

    uint max() {
        uint tmp = GPeriods[0];
        for (uint i = 1; i < GPeriodsSize; i++) {
            if (tmp < GPeriods[i]) {
                tmp = GPeriods[i];
            }
        }
        return tmp;
    }

    double Average() {
        double tmp = 0;
        for (uint i = 0; i < GPeriodsSize; i++) {
            tmp = tmp + (double)GPeriods[i];
        }
        return tmp / GPeriodsSize;
    }

    double SD() {
        double tmp = 0;
        double avg = Average();

        for (uint i = 0; i < GPeriodsSize; i++) {
            tmp = tmp + (double)(avg - (double)GPeriods[i]) *
                            (avg - (double)GPeriods[i]);
        }
        return sqrt(1.0 / GPeriodsSize * tmp);
    }

    void ShowTotalStatistics() {
        printf("Total g-series=%d\n", GPeriodsSize);
        printf("min=%d\n", min());
        printf("max=%d\n", max());
        printf("avg=%.3f\n", Average());
        printf("SD=%.3f\n", SD());
    }
#pragma endregion
};
#endif
