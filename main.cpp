#include "statistics.h"
#include "predict.h"

void ReadFile(char *path){
    FILE *f=fopen(path,"rb");
    if (f==NULL){
        printf("File not found"); exit(1);
    }
    unsigned char ch;
    Predictor *Pred;
    Statistics Stat;

    while(!feof(f)){
        fscanf(f,"%c",&ch);
        Stat.Process(ch);            
    }
    Stat.ShowTotalStatistics();

    for(int firstPart=700;firstPart<=2900;firstPart+=200){
        for (int seconPart=50;seconPart<500;seconPart=seconPart+50){
            Pred=new Predictor(seconPart,firstPart);
            fseek(f,0,0);
            while(!feof(f)){
                fscanf(f,"%c",&ch);            
                Pred->Process(ch);
            }        
            Pred->ShowTotalStatistics();
            delete Pred;
        }    
    }
    fclose(f);
}

void Usage(char *path){
    printf("Programm analyses g-series of rnd.file\n");
    printf("Usage:\n");
    printf("%s file\n",path);
}

int main(int argc,char *argv[]){
    if (argc!=2){
        Usage(argv[0]);
        return 0;
    }
    ReadFile(argv[1]);     

    return 0;
}
