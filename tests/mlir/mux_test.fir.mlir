module {
  firrtl.circuit "mux_2to1" {
    firrtl.module @mux_2to1(in %a: !firrtl.uint<3>, in %b: !firrtl.uint<4>, in %sel: !firrtl.uint<2>, out %out: !firrtl.uint<3>) {
      %0 = firrtl.bits %sel 0 to 0 : (!firrtl.uint<2>) -> !firrtl.uint<1>
      %_sel__0 = firrtl.node interesting_name %0 : !firrtl.uint<1>
      %1 = firrtl.bits %b 3 to 1 : (!firrtl.uint<4>) -> !firrtl.uint<3>
      %_b__1 = firrtl.node interesting_name %1 : !firrtl.uint<3>
      %2 = firrtl.bits %sel 1 to 1 : (!firrtl.uint<2>) -> !firrtl.uint<1>
      %_sel__2 = firrtl.node interesting_name %2 : !firrtl.uint<1>
      %c2_ui3 = firrtl.constant 2 : !firrtl.const.uint<3>
      %3 = firrtl.mux(%_sel__2, %a, %c2_ui3) : (!firrtl.uint<1>, !firrtl.uint<3>, !firrtl.const.uint<3>) -> !firrtl.uint<3>
      %4 = firrtl.mux(%_sel__0, %_b__1, %3) : (!firrtl.uint<1>, !firrtl.uint<3>, !firrtl.uint<3>) -> !firrtl.uint<3>
      %5 = firrtl.bits %4 2 to 0 : (!firrtl.uint<3>) -> !firrtl.uint<3>
      %___comb_multiplexer__0__3 = firrtl.node interesting_name %5 : !firrtl.uint<3>
      firrtl.matchingconnect %out, %___comb_multiplexer__0__3 : !firrtl.uint<3>
    }
  }
}