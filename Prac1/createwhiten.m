








function whiten = createwhiten(n)
  whiten = [];
  for i = 1:n*48000
    r = rand()*2-1;
    whiten = vertcat(whiten, r);
  endfor  
 
 return;

endfunction