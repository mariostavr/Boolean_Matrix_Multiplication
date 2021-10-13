

 %GENERATING ARRAYS FOR BMM (+) FILTER
%=====================================%

%DATA FOR ARRAYS.
rows = 6000000;
cols = rows;
d = 2; % Approximate number of true elements per row.

%GENERATING RANDOM ARRAYS.
A = sprand( rows, cols, d/rows ) > 0;
B = sprand( rows, cols, d/rows ) > 0;
fprintf('Succesfully generation!\n');

%CALCULATING FILTER
C = (A*B)>0;
F = C.*C;
fprintf('Succesfully filter calculation!\n');

%WRITING DENSE ARRAY INTO A TEXT FILE.
%writematrix(A,'A.txt')
%writematrix(B,'B.txt')

%CONVERTING DENSE ARRAY TO SPARSE.
[a_row a_col] = find(A);
[b_row b_col] = find(B);
[f_row f_col] = find(F);

a_coo = [(a_row-1) (a_col-1)];
a_coo = sortrows(a_coo);
a_nnz = length(a_coo);

b_coo = [(b_row-1) (b_col-1)];
b_nnz = length(b_coo);

f_coo = [(f_row-1) (f_col-1)];
f_coo = sortrows(f_coo);
f_nnz = length(f_coo);


%WRITING DATA INTO TEXT FILES.


data_A = [rows cols a_nnz];
data_B = [rows cols b_nnz];
data_F = [rows cols f_nnz];

file_A = fopen('data_A.txt','w');
fprintf(file_A,'%d %d %d\n',data_A);
fclose(file_A);

file_B = fopen('data_B.txt','w');
fprintf(file_B,'%d %d %d\n',data_B);
fclose(file_B);

file_F = fopen('data_F.txt','w');
fprintf(file_F,'%d %d %d\n',data_F);
fclose(file_F);

%WRITING ARRAYS INTO TEXT.
dlmwrite('Array_A.txt', a_coo, 'delimiter', ' '); 
dlmwrite('Array_B.txt', b_coo, 'delimiter', ' '); 
dlmwrite('Filter.txt', f_coo, 'delimiter', ' ');

fprintf('Program ended.\n');


