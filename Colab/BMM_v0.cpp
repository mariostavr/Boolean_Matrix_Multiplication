#include<iostream>
#include<cstdlib>
#include <fstream>
#include <algorithm>
#include <vector>
#include<string.h>
#include <bits/stdc++.h>

using namespace std;

    vector<int> c_coo_row;
    vector<int> c_coo_col;

    clock_t t_start, t_end;
    double time_taken;

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


//================================= Multiply ================================
bool multiply(int i, int j, int* csr_row, int* csr_col, int* csc_row, int* csc_col){

    int nextNnz_row = 0;
    int nextNnz_col = 0;

    while ( (csr_row[i] + nextNnz_row < csr_row[i + 1]) && (csc_col[j] + nextNnz_col < csc_col[j + 1]) ) {
        if ( (csr_col[csr_row[i] + nextNnz_row]) < (csc_row[csc_col[j] + nextNnz_col]) )
            nextNnz_row++;
        else if (csr_col[csr_row[i] + nextNnz_row] > csc_row[csc_col[j] + nextNnz_col])
            nextNnz_col++;
        else
            return true;
    }
    return false;
}//==========================================================================


int main(){

    cout << "Boolean Matrix Multiplication v.0\n=================================\n";

    int a_rows, a_cols, a_nnz;  //..Array A
    int b_rows, b_cols, b_nnz;  //..Array B

    cout << "Importing arrays.\n";
    //============================================================ Import Array from file
    int *data_A = dataArray('A', a_rows, a_cols, a_nnz);
    a_rows = data_A[0]; a_cols = data_A[1]; a_nnz = data_A[2];
    int *a_coo_row = new int[a_nnz], *a_coo_col = new int[a_nnz];
    importArray('A', a_coo_row, a_coo_col, a_nnz);

    int *data_B = dataArray('B', b_rows, b_cols, b_nnz);
    b_rows = data_B[0]; b_cols = data_B[1]; b_nnz = data_B[2];
    int *b_coo_row = new int[b_nnz], *b_coo_col = new int[b_nnz];
    importArray('B', b_coo_row, b_coo_col, b_nnz);

   
    cout << "Converting to CSR.\n";
    //============================================================ Converting CSR
    int* csr_col = new int[a_nnz];
    int* csr_row = new int[a_rows+1];
    convertCSR(csr_row, csr_col, a_coo_row, a_coo_col, a_nnz, a_rows);


    cout << "Converting to CSC.\n";
    //============================================================ Converting CSC
    int* csc_col = new int[b_rows+1];
    int* csc_row = new int[b_nnz];
    convertCSC(csc_row, csc_col, b_coo_row, b_coo_col, b_nnz, b_rows);

 
    cout << "Multiplying.\n";
    //============================================================ Multiplication
    t_start = clock();

    for (int i=0; i<a_rows; i++)
        for (int j=0; j<b_rows; j++)
            if (multiply(i, j, csr_row, csr_col, csc_row, csc_col)){
                c_coo_row.push_back(i);
                c_coo_col.push_back(j);
            }

    t_end = clock();
    time_taken = double(t_end - t_start) / double(CLOCKS_PER_SEC);
    cout << "\n\nTime taken by program is : " << fixed << time_taken << setprecision(5) << " seconds." << endl;

  
    cout << endl;
    return 0;
}
