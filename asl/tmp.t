function prod_escalar
  params
    _result
    a1
    a2
  endparams

  vars
    i 1
    s 1
  endvars

     %1 = 0
     s = %1
  label startwhile1 :
     %2 = 10
     %3 = i < %2
     ifFalse %3 goto endwhile1
     %5 = a1
     %4 = %5[i]
     %7 = a2
     %6 = %7[i]
     %8 = %4 * %6
     %9 = s + %8
     s = %9
     %10 = 1
     %11 = i + %10
     i = %11
     goto startwhile1
  label endwhile1 :
     _result = s
     return
endfunction

function main
  vars
    i 1
    v1 10
    v2 10
  endvars

     %1 = 0
     i = %1
  label startwhile1 :
     %2 = 10
     %3 = i < %2
     ifFalse %3 goto endwhile1
     %4 = - i
     %5 = v1
     %5[i] = %4
     %6 = i * i
     %7 = v2
     %7[i] = %6
     %8 = 1
     %9 = i + %8
     i = %9
     goto startwhile1
  label endwhile1 :
     pushparam 
     %10 = &v1
     pushparam %10
     %11 = &v2
     pushparam %11
     call prod_escalar
     popparam 
     popparam 
     popparam %12
     writei %12
     writeln
     return
endfunction


