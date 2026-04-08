#include "predict.h"
#include "statistics.h"

void ReadFile(char *path) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        printf("File not found");
        exit(1);
    }
    unsigned char ch;
    Predictor Pred;
    Statistics Stat;

    while (!feof(f)) {
        fscanf(f, "%c", &ch);
        Stat.Process(ch);
        Pred.AddSymbol(ch);
    }
    fclose(f);
    Stat.ShowTotalStatistics();

    Pred.SetSecondPart(50, 450, 50);

    for (int first_part = 700; first_part <= 2900; first_part += 200) {
        Pred.SetPeriod(first_part);
        Pred.MainProcess();
    }
}

void Usage(char *path) {
    printf("Programm analyses g-series of rnd.file\n");
    printf("Usage:\n");
    printf("%s file\n", path);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        Usage(argv[0]);
        return 0;
    }
    ReadFile(argv[1]);

    return 0;
}
