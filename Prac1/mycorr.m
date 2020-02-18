

%input two matrices x and y; output correlation coefficient r
% I am assuming that the inputs are both 1D matrices (vectors) of the same length
function r = mycorr(x, y)
  n = size(x);
  n = n(2);
  x_mean = mean(x);
  y_mean = mean(y);
  
  x_diff = x-x_mean;
  y_diff = y-y_mean;
  
  numerator = sum((x_diff).*(y_diff));
  denominator = (sqrt(sumsq(x_diff))).*(sqrt(sumsq(y_diff)));
  r = numerator./denominator;

  return;  

endfunction