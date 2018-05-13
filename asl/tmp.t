function f
  params
    _result
    a
    b
  endparams

  vars
    x 1
    y 1
    z 10
  endvars

     %1 = 9
     %2 = 67
     %3 = a + %2
     z[%1] = %3
     %4 = 34
     x = %4
     %5 = 3
     %6 = 56
     %7 = 9
     %8 = %6 + z[%7]
     z[%5] = %8
     %9 = 3
     %10 = z[%9] <= x
     %10 = not %10
     ifFalse %10 goto endif1
     %11 = 78
     x = %11
     writef b
     writeln
  label endif1 :
     %14 = 3
     writei z[%14]
     writeln
     %16 = 1
     _result = %16
     return
endfunction

function fz
  params
    r
  endparams

  label startwhile1 :
     %1 = 0
     %2 = r <= %1
     %2 = not %2
     ifFalse %2 goto endwhile1
     %3 = 1
     %4 = r - %3
     r = %4
     goto startwhile1
  label endwhile1 :
     return
endfunction

function main
  vars
    a 1
  endvars

     %1 = 3
     %2 = 2
     pushparam 
     pushparam %1
     pushparam %2
     call f
     popparam 
     popparam 
     popparam %3
     ifFalse %3 goto endif1
     %4 = 3.7
     %5 = float a
     %6 = %5 +. %4
     %7 = 4
     %8 = float %7
     %9 = %6 +. %8
     writef %9
     writeln
  label endif1 :
     return
endfunction


