#include<iostream>
#include<cstdlib>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <vector>
#include<string.h>
#include <bits/stdc++.h>

using namespace std;

    vector<int> fc_coo_row;
    vector<int> fc_coo_col;

    clock_t t_start, t_end;
    double time_taken;

//=========================== Return Array's Data ==========================
int* dataArray(char name, int rows, int cols, int nnz){

    static int data[3]; //..Contains [(rows) (cols) (nnz)]

    if (name == 'A'){
        std::ifstream input("../Arrays/data_A.txt");
        if (!input)
            std::cerr << "Could not open the file!\n";
        else
            input >> rows >> cols >> nnz;

    }
    else if(name == 'B'){
        std::ifstream input("../Arrays/data_B.txt");
        if (!input)
            std::cerr << "Could not open the file!\n";
        else
            input >> rows >> cols >> nnz;
    }
    else if(name == 'F'){
        std::ifstream input("../Arrays/data_F.txt");
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


//============================== Import Array ===============================
void importArray(char name, int* rows, int* cols, int nnz){

    if (name == 'A'){
        std::ifstream input("../Arrays/Array_A.txt");
        if (!input)
            std::cerr << "Could not open the file!" << std::endl;
        else
            for (int i=0; i<nnz; i++)
                input >> rows[i] >> cols[i];

    }
    else if (name == 'B'){
        std::ifstream input("../Arrays/Array_B.txt");
        if (!input)
            std::cerr << "Could not open the file!" << std::endl;
        else
            for (int i=0; i<nnz; i++)
                input >> rows[i] >> cols[i];
    }
    else if(name == 'F'){
        std::ifstream input("../Arrays/Filter.txt");
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


int main(){

    cout << "Boolean Matrix Multiplication v.1\n=================================";

    int a_rows, a_cols, a_nnz;  //..Array A
    int b_rows, b_cols, b_nnz;  //..Array B
    int f_rows, f_cols, f_nnz;  //..Filter

    //============================================================ Import Array from file
    int *data_A = dataArray('A', a_rows, a_cols, a_nnz);
    a_rows = data_A[0]; a_cols = data_A[1]; a_nnz = data_A[2];
    int *a_coo_row = new int[a_nnz], *a_coo_col = new int[a_nnz];
    importArray('A', a_coo_row, a_coo_col, a_nnz);

    int *data_B = dataArray('B', b_rows, b_cols, b_nnz);
    b_rows = data_B[0]; b_cols = data_B[1]; b_nnz = data_B[2];
    int *b_coo_row = new int[b_nnz], *b_coo_col = new int[b_nnz];
    importArray('B', b_coo_row, b_coo_col, b_nnz);

    int *data_F = dataArray('F', f_rows, f_cols, f_nnz);
    f_rows = data_F[0]; f_cols = data_F[1]; f_nnz = data_F[2];
    int *f_coo_row = new int[f_nnz], *f_coo_col = new int[f_nnz];
    importArray('F', f_coo_row, f_coo_col, f_nnz);

    //=========================================== Printing
    cout << "\n\nCOO A:\t" ;
    for (int i=0; i<a_nnz; i++)
        cout << "(" << a_coo_row[i] << "," << a_coo_col[i] << ")  ";

    cout << "\nCOO B:\t" ;
    for (int i=0; i<b_nnz; i++)
        cout << "(" << b_coo_row[i] << "," << b_coo_col[i] << ")  ";

    cout << "\nFilter:\t" ;
    for (int i=0; i<f_nnz; i++)
        cout << "(" << f_coo_row[i] << "," << f_coo_col[i] << ")  ";


    //============================================================ Converting CSR
    int *f_csr_row = new int[f_rows+1];
    int *f_csr_col = new int[f_nnz];
    convertCSR(f_csr_row, f_csr_col, f_coo_row, f_coo_col, f_nnz, f_rows);

    int* csr_col = new int[a_nnz];
    int* csr_row = new int[a_rows+1];
    convertCSR(csr_row, csr_col, a_coo_row, a_coo_col, a_nnz, a_rows);

    //=========================================== Printing
    cout << "\n\nFILTER CSR COL: " ;
    for (int i=0; i<f_nnz; i++)
        cout << f_csr_col[i] << " ";

    cout << "\nFILTER CSR ROW: " ;
    for (int i=0; i<f_rows+1; i++)
        cout << f_csr_row[i] << " ";

    cout << "\n\nCSR COL: " ;
    for (int i=0; i<a_nnz; i++)
        cout << csr_col[i] << " ";

    cout << "\nCSR ROW: " ;
    for (int i=0; i<a_rows+1; i++)
        cout << csr_row[i] << " ";


    //============================================================ Converting CSC
    int* csc_col = new int[b_rows+1];
    int* csc_row = new int[b_nnz];
    convertCSC(csc_row, csc_col, b_coo_row, b_coo_col, b_nnz, b_rows);

    //=========================================== Printing
    cout << "\n\nCSC ROW: " ;
    for (int i=0; i<b_nnz; i++)
        cout << csc_row[i] << " ";

    cout << "\nCSC COL: " ;
    for (int i=0; i<b_rows+1; i++)
        cout << csc_col[i] << " ";


    //============================================================ Filter Multiplication
    t_start = clock();

    for (int i=0; i<f_rows; i++)
        for (int j=f_csr_row[i]; j<f_csr_row[i+1]; j++)
            if (multiply(i, f_csr_col[j], csr_row, csr_col, csc_row, csc_col)){
                fc_coo_row.push_back(i);
                fc_coo_col.push_back(f_csr_col[j]);
            }

    t_end = clock();
    time_taken = double(t_end - t_start) / double(CLOCKS_PER_SEC);
    cout << "\n\nTime taken by Filtered program is : " << fixed << time_taken << setprecision(5) << " seconds.";

    //=========================================== Printing

    cout << "\n\nFiltered Results:";
    cout << "\nCOO C: " ;
    for (int i=0; i<fc_coo_row.size(); i++)
        cout << "(" << fc_coo_row.at(i) << "," << fc_coo_col.at(i) <<  ") ";

    cout << endl;
    return 0;
}
