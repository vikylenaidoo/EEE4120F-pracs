

tic; white = rand(8000*10, 1)*2 - 1; runtime = toc();
disp(strcat("It took: ", num2str(runtime*1000), " ms to run"));

tic; white = createwhiten(10);runtime = toc();
disp(strcat("It took: ", num2str(runtime*1000), " ms to run createwhiten"));