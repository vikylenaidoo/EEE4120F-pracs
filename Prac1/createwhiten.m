#  White Noise Generator Script
#generates a white noise signal, one sample at a time, comprising N duration in seconds

function whiten = createwhiten(n)
  whiten = [];
  for i = 1:n*8000
    r = rand()*2-1;
    %whiten = vertcat(whiten, r);
    whiten = [whiten;r];
  endfor  
 
 return;

endfunction