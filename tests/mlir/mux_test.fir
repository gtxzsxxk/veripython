FIRRTL version 3.3.0
circuit MuxTest :
  module MuxTest :
    input clock : Clock
    input reset : UInt<1>
    input a : UInt<3>
    input b : UInt<4>
    input sel : UInt<2>
    output out : UInt<3>

    node _out_T = bits(sel, 0, 0)
    node _out_T_1 = bits(b, 3, 1)
    node _out_T_2 = bits(sel, 1, 1)
    node _out_T_3 = mux(_out_T_2, a, UInt<2>(0h2))
    node _out_T_4 = mux(_out_T, _out_T_1, _out_T_3)
    connect out, _out_T_4
