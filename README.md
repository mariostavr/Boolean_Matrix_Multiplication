# INSTRUCTION FOR GOOGLE COLAB:

Steps to get ready:
1. Go to https://colab.research.google.com/, click on "Upload" and choose the file "BMM.ipynb".
2. Upload in library the code files from folder "Colab".
3. Run each code cells of the tab "Create and Install - Requirments to compile/run codes".
4. When each cell is done and you have uploaded the files you can start compiling and running.

Steps to compile:
1. Double click on file "Generator.m". The Matlab script will be shown. You can change the size of rows.
2. Then you can run the code cell "Generate Arrays From Matlab".
3. Now you can compile and run each code version.

My tests:
1. BMM v.0: Rows = 50000, 100000
2. BMM v.1: Roe = 50000, 100000, 3000000, 6000000
3. MPI BMM v.1: Rows = 3000000, mpiProcessors = 2
4. MPI BMM v.1: Rows = 6000000. mpiProcessors = 3
5. OMP BMM v.0: Rows = 3000000, ompThreads = 2
6. OMP BMM v.0: Rows = 6000000. ompThreads = 3  
7. Block BMM v.0 AND v.1: Rows = 100000, Blocking = 10
8. MPI - OMP Block BMM v.1: Rows = 100000, Blocking = 10, ompThreads = 1, mpiProcessors = 1, 2, 4, 8, 10
9. MPI - OMP Block BMM v.1: Rows = 200000, Blocking = 20, ompThreads = 1, mpiProcessors = 1, 2, 4, 8, 10



In case you want to change the blocking size:
1. Double click on code file and change the "blocking size".

    bBMM_v0.cpp: Line 243.
    
    bBMM_v1.cpp: Line 268.
    
    mpi_omp_bBMM_v1: Line 286
