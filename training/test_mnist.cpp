#include <bits/stdc++.h>
#include "mnist-cnn-config-1W5A.h"
#include "mnist-cnn-params-1W5A.h"
using namespace std;
#define DIN 16
#define K 3
#define S 1
#define CIN 32
#define COUT 32
#define Ibit 8
#define Wbit 1
#define Mbit 32
#define Abit 8
#define OutP 8
#define InP 8
#define FACTOR_SCALE_BITS 22
#define ScaleBits 18
int dot(long in, char weight){
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
char active(string s1, string s2, int res)
{
    int tmpVal1, tmpVal2;
    sscanf(s1.c_str(), "%x", &tmpVal1);
    sscanf(s2.c_str(), "%x", &tmpVal2);
    char result;
    long temp_result = tmpVal1 * res;
    temp_result += tmpVal2;
    char limit = 1<<Abit - 1;
    bool remains = temp_result & (1<<(FACTOR_SCALE_BITS-1));
    temp_result = temp_result >> FACTOR_SCALE_BITS;
    if(Wbit > 1) temp_result = temp_result >> ScaleBits;
    if(temp_result < 0)
        result = 0;
    else if(temp_result >= limit)
        result = limit;
    else 
        result = temp_result & 0xFF+ remains;

    return result;
}
void conv2d(const string* w, const string* a, const string* b, vector<long>& a1, int MatrixH, int MatrixW, vector<long>& out)
{
    const int NumVecs = DIN * DIN;
    const int InputFold = MatrixH / InP;
    const int OutputFold = MatrixW / OutP;
    const int totalReps = NumVecs * InputFold * OutputFold;
    int wVec = 0;
    int wMat = 0;

    int index = 0;
    int resultVec[OutP];
    long OutBuf = 0;
    long rowstore[InputFold];
    int a_index = 0;
    long tempVec;
    for(int rep = 0;rep < totalReps;rep++)
    { 
        index = wVec * OutputFold + wMat;
        //long tempVec = a1[0];
        
        if(wMat == 0)
        {
            tempVec = a1[a_index++];
            rowstore[wVec] = tempVec;
        }
        else{
            tempVec = rowstore[wVec];
        }
        for(int p = 0;p < OutP;p++)
        {
            //calculate
            int col = ((CIN*K*K)/InP)*(COUT/OutP);
            string tempMat = w[p*col + index];
            int tmpVal;
            sscanf(tempMat.c_str(), "%x", &tmpVal);
            char tmp1 = tmpVal & 0x00FF;
            int res = dot(tempVec, tmp1);
            if(wVec == 0)
                resultVec[p] = res;
            else
                resultVec[p] += res;
            char tmp_out = active(a[p * OutP + wMat], b[p * OutP + wMat], resultVec[p]);
            OutBuf = OutBuf << Abit | tmp_out;
        }

        if(wVec == InputFold - 1)
        {
            out.push_back(OutBuf);
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
void reduce_with(vector<vector<int>>& in, vector<vector<int>>& out)
{
    int InStreamW = CIN * Ibit;
    int OutStreamW = InP * Abit;
    int NumLines = K * K * 14 * 14;
    int parts = InStreamW / OutStreamW;

    for(int rep = 0;rep < NumLines;rep++)
    {
        vector<int> temp_in = in[rep];
        for(int p = 0;p < parts;p++)
        {
            vector<int> temp_out;
            copy(temp_in.begin()+p*OutStreamW, temp_in.begin()+(p+1)*OutStreamW, back_inserter(temp_out));
            out.push_back(temp_out);
        }
    }

}
void SWU_NoP(vector<vector<int>> &in, vector<vector<int>>& out)
{
    int steps = (DIN - K) / S + 1;
    int line_buffer_size = K * DIN;

    //int line_buffer[line_buffer_size][256];
    vector<vector<int>> line_buffer(line_buffer_size, vector<int>(256, 0));

    //int temp_in[256];
    vector<int> temp_in(256, 0);
    int initial_fill = 0;
    int stride = 0;
    int pointer = 0;
    int h = 0;
    for(int rep = 0;rep < DIN;rep++)
    {
        if(h == DIN)
        {
            initial_fill = 0;
            stride = 0;
            pointer = 0;
            h = 0;
        }
        h += 1;

        for(int w = 0;w < DIN;w++)
        {
            temp_in = in[w];

            int line_buffer_pointer = pointer + w;
            if (line_buffer_pointer >= line_buffer_size)
                line_buffer_pointer = line_buffer_pointer - line_buffer_size;
            line_buffer[line_buffer_pointer] = temp_in;
        }

        stride += 1;
        pointer += DIN;
        if (pointer >= line_buffer_size)
        {
            pointer = pointer - line_buffer_size;
            initial_fill = 1;
        }

        if (initial_fill == 1 && stride >= S)
        {
            stride = 0;

            int s = 0;
            int x = 0;
            int y = 0;

            for(int i = 0;i < steps * K * K;i++)
            {
                int read_address = (pointer + s*S) + y * DIN + x;
                if(read_address >= line_buffer_size)
                    read_address = read_address - line_buffer_size;
                vector<int> temp_out = line_buffer[read_address];
                out.push_back(temp_out);
                if(x == K-1)
                {
                    x = 0;
                    if(y == K-1)
                    {
                        y = 0;
                        if(s == steps - 1)
                            s = 0;
                        else
                            s++;
                    }
                    else
                        y++;
                }
                else
                    x++;
            }
        }
    }

}
int main()
{ 
    //long a[14][14];
    vector<long> a(14*14, 1);
    //cout<<a[0][0];
    vector<vector<int>> in(256, vector<int>(256, 1));
    vector<vector<int>> out;
    vector<long> out2;
    SWU_NoP(in, out);
    vector<vector<int>> out1;
    reduce_with(out, out1);
    int tmp = out1.size();
    const int MatrixH = CIN * K * K;
    const int MatrixW = COUT;
    const int NumVecs = DIN * DIN;
    conv2d(&weights1[0][0], &factorA1[0][0], &factorB1[0][0], a, MatrixH, MatrixW, out2);
    
    return 0;
}