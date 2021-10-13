#include<iostream>
#include<cstdlib>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <vector>
#include<string.h>
#include <bits/stdc++.h>
using namespace std;

    clock_t t_start, t_end;
    double time_taken;

    vector<int> c_coo_row;
    vector<int> c_coo_col;
    vector<int> bc_coo_row;
    vector<int> bc_coo_col;

//=========================== Return Array's Data ==========================
int* dataArray(char name, int rows, int cols, int nnz){

    static int data[3]; //..Contains [(rows) (cols) (nnz)]

    if (name == 'A'){
        std::ifstream input("Arrays/data_A.txt");
        if (!input)
            std::cerr << "Could not open the file!\n";
        else
            input >> rows >> cols >> nnz;

    }
    else if(name == 'B'){
        std::ifstream input("Arrays/data_B.txt");
        if (!input)
            std::cerr << "Could not open the file!\n";
        else
            input >> rows >> cols >> nnz;
    }
    else
        cout << "File doesn't exist.\n";

    data[0] = rows; data[1] = cols; data[2] = nnz;
    return data;
}//==========================================================================


//============================== Import Array ==============================
void importArray(char name, int* rows, int* cols, int nnz){

    if (name == 'A'){
        std::ifstream input("Arrays/Array_A.txt");
        if (!input)
            std::cerr << "Could not open the file!" << std::endl;
        else
            for (int i=0; i<nnz; i++)
                input >> rows[i] >> cols[i];

    }
    else if (name == 'B'){
        std::ifstream input("Arrays/Array_B.txt");
        if (!input)
            std::cerr << "Could not open the file!" << std::endl;
        else
            for (int i=0; i<nnz; i++)
                input >> rows[i] >> cols[i];
    }
    else
        cout << "File doesn't exist.\n";
}//==========================================================================


//============================= Convert to CSR ==============================
void convertCSR(int* csr_row, int* csr_col, int* coo_row, int* coo_col, int nnz, int rows){

    for (int i=0; i<rows+1; i++)
        csr_row[i]=0;

    for (int i=0; i<nnz; i++)
        csr_col[i]=0;

    for(int i = 0; i < nnz; i++){
        csr_row[coo_row[i]+1]++;
        csr_col[i] = coo_col[i];
    }

    for (int i=0; i<rows; i++)
        csr_row[i+1] += csr_row[i];
}//==========================================================================


//============================= Convert to CSC ==============================
void convertCSC(int* csc_row, int* csc_col, int* coo_row, int* coo_col, int nnz, int rows){

    for (int i=0; i<nnz; i++)
        csc_row[i]=0;
    for (int i=0; i<rows+1; i++)
        csc_col[i]=0;

    convertCSR(csc_col, csc_row, coo_col, coo_row, nnz, rows);
}//==========================================================================


//============================== DATA FOR BLOCKING ==========================
struct data_BS{
    int blocking, blocks, block_rows, block_cols, block_nnz;
    int *nnz_block;
};

data_BS blockSparse(int rows, int cols, int blocking, int *csr_row, int *csr_col){

    data_BS dataBS;

    int blocks = (rows/blocking) * (cols/blocking);
    int block_rows = rows/blocking;
    int block_cols = cols/blocking;
    int block_nnz = 0;
    int *nnz_block = new int[blocks];

    for (int i=0; i<blocks; i++)
        nnz_block[i] = 0;

    for (int i=0; i<rows; i++)
        for (int j=csr_row[i]; j<csr_row[i+1]; j++)
            nnz_block[(i/blocking) * block_cols + (csr_col[j]/blocking)]++;

    for (int i=0; i<block_rows; i++)
        for (int j=0; j<block_cols; j++)
        if (nnz_block[i*block_cols+j] != 0)
            block_nnz++;

    dataBS.blocking = blocking, dataBS.blocks=blocks, dataBS.block_rows=block_rows, dataBS.block_cols=block_cols, dataBS.block_nnz=block_nnz;
    dataBS.nnz_block = nnz_block;

    return dataBS;
}//==========================================================================


//======================= Convert to Blocking Sparse ========================
void convertBS(int rows, int cols, data_BS dataBS, int* b_csr_row, int* b_csr_col, int* csr_row, int* csr_col){

    int *b_coo_row = new int[dataBS.block_nnz];
    int *b_coo_col = new int[dataBS.block_nnz];

    int counter=0;
    for (int i=0; i<dataBS.block_rows; i++)
        for (int j=0; j<dataBS.block_cols; j++)
            if (dataBS.nnz_block[i*dataBS.block_cols+j] != 0){
                b_coo_row[counter] = i;
                b_coo_col[counter] = j;
                counter++;
        }

    convertCSR(b_csr_row, b_csr_col, b_coo_row, b_coo_col, dataBS.block_nnz, dataBS.block_rows);
}//==========================================================================


//================================ MULTIPLY =================================
bool multiply(int i, int j, int* csr_row, int* csr_col, int* csc_row, int* csc_col){

    int nextNnz_row = 0;
    int nextNnz_col = 0;

    while (csr_row[i] + nextNnz_row < csr_row[i + 1] && csc_col[j] + nextNnz_col < csc_col[j + 1]) {

        if (csr_col[csr_row[i] + nextNnz_row] < csc_row[csc_col[j] + nextNnz_col])
            nextNnz_row++;
        else if (csr_col[csr_row[i] + nextNnz_row] > csc_row[csc_col[j] + nextNnz_col])
            nextNnz_col++;
        else
            return true;
    }
    return false;
}//==========================================================================


//============================ Blocks Multiply ==============================
bool b_multiply(int blocking, int i, int j, int* b_csr_row, int* b_csr_col,
                int* b_csc_row, int* b_csc_col,int* csr_row, int* csr_col, int* csc_row, int* csc_col){

    int nextNnz_row = 0;
    int nextNnz_col = 0;

    while (b_csr_row[i] + nextNnz_row < b_csr_row[i + 1] && b_csc_col[j] + nextNnz_col < b_csc_col[j + 1]) {

        if (b_csr_col[b_csr_row[i] + nextNnz_row] < b_csc_row[b_csc_col[j] + nextNnz_col])
            nextNnz_row++;
        else if (b_csr_col[b_csr_row[i] + nextNnz_row] > b_csc_row[b_csc_col[j] + nextNnz_col])
            nextNnz_col++;
        else {
            //============================================================ Multiplying - Iteration through inner Block
            for (int k=blocking*i; k<(blocking*i)+blocking; k++)
                for (int h=blocking*j; h<(blocking*j)+blocking; h++)
                    if (multiply(k, h, csr_row, csr_col, csc_row, csc_col)){
                        c_coo_row.push_back(k);
                        c_coo_col.push_back(h);

                    }
            return true;
        }
    }
    return false;
}//==========================================================================



int main(){

    cout << "BLOCK BMM v.0\n==============\n";

    int a_rows, a_cols, a_nnz;  //..Array A
    int b_rows, b_cols, b_nnz;  //..Array B

    //============================================================ Import Array from file
    int *data_A = dataArray('A', a_rows, a_cols, a_nnz);
    a_rows = data_A[0]; a_cols = data_A[1]; a_nnz = data_A[2];
    int *a_coo_row = new int[a_nnz], *a_coo_col = new int[a_nnz];
    importArray('A', a_coo_row, a_coo_col, a_nnz);

    int *data_B = dataArray('B', b_rows, b_cols, b_nnz);
    b_rows = data_B[0]; b_cols = data_B[1]; b_nnz = data_B[2];
    int *b_coo_row = new int[b_nnz], *b_coo_col = new int[b_nnz];
    importArray('B', b_coo_row, b_coo_col, b_nnz);


    //============================================================ Converting CSR
    int* csr_col = new int[a_nnz];
    int* csr_row = new int[a_rows+1];

    cout << "\nConverting to CSR...";
    convertCSR(csr_row, csr_col, a_coo_row, a_coo_col, a_nnz, a_rows);
    cout << "  Succesfully convertion to CSR!\n";


    //============================================================ Converting CSC
    int* csc_col = new int[b_rows+1];
    int* csc_row = new int[b_nnz];

    cout << "\nConverting to CSC...";
    convertCSC(csc_row, csc_col, b_coo_row, b_coo_col, b_nnz, b_rows);
    cout << "  Succesfully convertion to CSC!\n";


    //============================================================ B
    int blocking = 10;

    //============================================================ Converting BCSR
    cout << "\nConverting to B - Sparse - CSR...";
    data_BS bcsr = blockSparse(a_rows, a_cols, blocking, csr_row, csr_col);
    int *b_csr_row = new int[bcsr.block_rows + 1];
    int *b_csr_col = new int[bcsr.block_nnz];
    convertBS(a_rows, a_cols, bcsr, b_csr_row, b_csr_col, csr_row, csr_col);
    cout << "  Succesfully convertion to B - Sparse - CSR!\n";

    //============================================================ Converting BCSC
    cout << "\nConverting to B - Sparse - CSC...";
    data_BS bcsc = blockSparse(b_rows, b_cols, blocking, csc_col, csc_row);
    int *b_csc_col = new int[bcsc.block_rows + 1];
    int *b_csc_row = new int[bcsc.block_nnz];
    convertBS(b_rows, b_cols, bcsc, b_csc_col, b_csc_row, csc_col, csc_row);
    cout << "  Succesfully convertion to B - Sparse - CSC!\n";


    //============================================================ Block Multiplication
    cout << "\n\nMultiplying BCSR - BCSC...\n\n";
    t_start = clock();


    //============================================================ Multiplying - Iteration through Blocks
    for (int i=0; i<bcsr.block_rows; i++)
        for (int j=0; j<bcsc.block_rows; j++)
            if ((b_multiply(blocking,i, j, b_csr_row, b_csr_col, b_csc_row, b_csc_col, csr_row, csr_col, csc_row, csc_col))){
                bc_coo_row.push_back(i);
                bc_coo_col.push_back(j);

            }


    t_end = clock();
    time_taken = double(t_end - t_start) / double(CLOCKS_PER_SEC);
    cout << "\nSuccesfully multiplication!\n\n=>";
    cout << "Time taken by program B_BMM is: " << fixed << time_taken << setprecision(5) << " seconds.";

    cout << endl;
    return 0;
}
