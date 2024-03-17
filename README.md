# Multi Threaded Matrix Multiplication Linux
## Description
### Input
+ The input to the program is two matrixes A(xy) and B(yz) that are read from corresponding text files. The output is a matrix C(x*z) written to an output text file.
+ Program is executed as: ./matMultp Mat1 Mat2 MatOut, where Mat1 and Mat2 are the names of the text files to read the first and second matrices, respectively, and MatOut is the prefix of the text files names to write the output matrices (each file represents one method).
+  If the user does not enter this information, the default is a.txt and b.txt, for input matrixes A and B, and c for output matrices (of all three methods) C.
+ The following example should clarify inputs/outputs files.

Arguments
``` 
./matMultp a b c
Input files: 
  a.txt 
  b.txt
Output files: 
  c_per_matrix.txt
  c_per_row.txt
  c_per_element.txt
```
No Arguments
``` 
./matMultp
Input files: 
  a.txt 
  b.txt
Output files: 
  c_per_matrix.txt
  c_per_row.txt
  c_per_element.txt
```
+ Rows and Cols matrices are written in the first line of the file as ”row=x col=y”. The following is an example of the format on an input file.
```
  row=3 col=5
  1 2 3 4 5
  6 7 8 9 10
  11 12 13 14 15
```
