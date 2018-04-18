file = fopen('matrix_verysmall.txt','w');
size = 8;
matrix = randi([-9 9], size);
d = det(matrix);

assert(abs(d) < 2^63 - 1)

formatString = '\n';
for i = 1:size
    formatString = ['%3d ', formatString];
end

fprintf(file, '%d\n\n', size);
fprintf(file, formatString, matrix);
fprintf(file, '\n// det = %d', d);