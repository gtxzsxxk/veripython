hw.module @Top(in %clock : !seq.clock, in %i0 : i4, in %i1 : i4, out out : i4) {
  %0 = comb.add %i0, %i1 : i4
  %1 = comb.xor %0, %i0 : i4
  %2 = comb.xor %0, %i1 : i4
  %foo = seq.compreg %1, %clock : i4
  %bar = seq.compreg %2, %clock : i4
  %3 = comb.mul %foo, %bar : i4
  hw.output %3 : i4
}