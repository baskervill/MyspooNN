#include <bits/stdc++.h>
#include "mnist-cnn-config-1W5A.h"
#include "mnist-cnn-params-1W5A.h"
using namespace std;
#define DIN 14
#define K 3
#define S 1
#define CIN 32
#define COUT 32
#define Ibit 5
#define Wbit 1
#define Mbit 32
#define Abit 5
#define OutP 8
#define InP 8
int dot(long in, int weight){
    int acc = 0;
    for(int p = 0;p < OutP;p++)
    {
        int res = 0;
        bool tmp = weight & (1<<p);
        int tmp1 = (in & ((0xFF)<<(p*8)))>>(p*8);
        if(tmp)
        {
            res = tmp1;
        }
        else res = -tmp1;
        acc += res;
    }    
    return acc;
}

void conv2d(const string* w, const string* a, const string* b, const long* a1, int MatrixH, int MatrixW)
{
    const int NumVecs = DIN * DIN;
    const int InputFold = MatrixH / InP;
    const int OutputFold = MatrixW / OutP;
    const int totalReps = NumVecs * InputFold * OutputFold;
    int wVec = 0;
    int wMat = 0;

    int index = 0;


    for(int rep = 0;rep < totalReps;rep++)
    { 
        index = wVec * OutputFold + wMat;
        int tempVec = a1[0];
        for(int p = 0;p < OutP;p++)
        {
            //calculate
            string tempMat = w[p*36 + index];
            int tmpVal;
            sscanf(tempMat.c_str(), "%x", &tmpVal);
            int tmp1 = tmpVal & 0x00FF;
            int res = dot(tempVec, tmp1);
            
        }

        if(wVec == InputFold - 1)
        {
            //output
            cout<<"done"<<endl;
            //break;
        }
        if(wVec == InputFold - 1)
        {
            wVec = 0;
            if(wMat == OutputFold - 1)
            {
                wMat = 0;
            }
            else
                wMat++;
        }
        else
            wVec++;
    }
    
}

int main()
{ 
    long a[28][28];
    for(int i = 0;i < 28;i++)
        for(int j = 0;j < 28;j++)
            a[i][j] = 511;
    //cout<<a[0][0];
    
    
    const int MatrixH = CIN * K * K;
    const int MatrixW = COUT;
    const int NumVecs = DIN * DIN;
    conv2d(&weights1[0][0], &factorA1[0][0], &factorB1[0][0], &a[0][0], MatrixH, MatrixW);
    
    return 0;
}