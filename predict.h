#ifndef PREDICT_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gobject.h"

#define RESULTSLIMIT 10000
#define QUANTIL 6.63

class Predictor: public GObject{
    uint SECOND_PART; 
    uint G_PERIOD;
    bool resultsOfPrediction[RESULTSLIMIT];
    uint resultSize;
    uint count;
    uint inFirstPart,InSecondPart;

public:
    void Init(){
        GObject::Init();
        count=0;
        inFirstPart = InSecondPart = 0;
    }

    Predictor(uint _secondPart,uint _firstPart){
        SECOND_PART=_secondPart;
        G_PERIOD=_firstPart;
        resultSize = 0;
        Init();        
    }

    ~Predictor(){}

    bool ChiSquare(){
        double chi;
        double probabiltyInG=1.0*bytes_filled/ALPHABET_SIZE;        
        double TheoreticalInFirstPart=(probabiltyInG)*SECOND_PART;
        double TheoreticalInSecondPart=(1.0-probabiltyInG)*SECOND_PART;

        chi=(1.0*TheoreticalInFirstPart-inFirstPart)*(1.0*TheoreticalInFirstPart-inFirstPart)/TheoreticalInFirstPart;
        chi=chi+(1.0*TheoreticalInSecondPart-InSecondPart)*(1.0*TheoreticalInSecondPart-InSecondPart)/TheoreticalInSecondPart;        
        return (chi>QUANTIL)?true:false;
    }

    void Process(uint i){   
        count++;
        if (count>G_PERIOD){            
            if (resultSize>=RESULTSLIMIT) return;
            resultsOfPrediction[resultSize++]=ChiSquare();
            count=0;            
            Init();   
            bytes_filled=0;         
        }else{
            if (count>G_PERIOD-SECOND_PART){                
                if (bt(i)){
                    inFirstPart++;
                }else{
                    InSecondPart++;
                }
            }else{
                bts(i % ALPHABET_SIZE);                
            }
        }
    }

    inline bool bt(unsigned int i){
        return bytes[i];
    }

    void bts(uint i){
        if (bytes[i]) return;
        bytes[i] = true;  
        bytes_filled++;                                          
    }

#pragma region ShowStatistic  
    void ShowTotalStatistics(){        
        uint tmp=0;
        for (uint i=0;i<resultSize;i++){            
            tmp=(resultsOfPrediction[i])?tmp+1:tmp;
        }
        printf("SeconPart=%d g-period=%d - ",SECOND_PART,G_PERIOD);
        printf("Prediction probability %.3f, NumberOfBroken=%d from %d\n",1.0f*tmp/resultSize,tmp,resultSize);
    }
#pragma endregion
};
#endif
