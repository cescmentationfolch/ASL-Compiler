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
     %4 = z
     %4[%1] = %3
     %5 = 34
     x = %5
     %6 = 3
     %7 = 56
     %8 = 9
     %10 = z
     %9 = %10[%8]
     %11 = %7 + %9
     %12 = z
     %12[%6] = %11
     %13 = 3
     %15 = z
     %14 = %15[%13]
     %16 = %14 <= x
     %16 = not %16
     ifFalse %16 goto else1
     %17 = 78
     x = %17
     writef b
     writeln
     goto endif1
  label else1 :
     %19 = 99
     x = %19
  label endif1 :
     %20 = 3
     %22 = z
     %21 = %22[%20]
     writei %21
     writeln
     %24 = 1
     _result = %24
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
     %3 = float %2
     pushparam %3
     call f
     popparam 
     popparam 
     popparam %4
     ifFalse %4 goto endif1
     %5 = 3.7
     %6 = float a
     %7 = %6 +. %5
     %8 = 4
     %9 = float %8
     %10 = %7 +. %9
     writef %10
     writeln
  label endif1 :
     return
endfunction


