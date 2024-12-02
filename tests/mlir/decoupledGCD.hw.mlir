module {
  hw.module @DecoupledGcd(in %clock : !seq.clock, in %reset : i1, out input_ready : i1, in %input_valid : i1, in %input_bits_value1 : i16, in %input_bits_value2 : i16, in %output_ready : i1, out output_valid : i1, out output_bits_value1 : i16, out output_bits_value2 : i16, out output_bits_gcd : i16) {
    %c0_i16 = hw.constant 0 : i16
    %false = hw.constant false
    %true = hw.constant true
    %xInitial = seq.firreg %26 clock %clock {firrtl.random_init_start = 0 : ui64, sv.namehint = "xInitial"} : i16
    %yInitial = seq.firreg %29 clock %clock {firrtl.random_init_start = 16 : ui64, sv.namehint = "yInitial"} : i16
    %x = seq.firreg %21 clock %clock {firrtl.random_init_start = 32 : ui64} : i16
    %y = seq.firreg %23 clock %clock {firrtl.random_init_start = 48 : ui64} : i16
    %busy = seq.firreg %31 clock %clock reset sync %reset, %false {firrtl.random_init_start = 64 : ui64} : i1
    %resultValid = seq.firreg %16 clock %clock reset sync %reset, %false {firrtl.random_init_start = 65 : ui64, sv.namehint = "resultValid"} : i1
    %0 = comb.xor bin %busy, %true {sv.namehint = "_input_ready_T"} : i1
    %1 = comb.icmp bin ugt %x, %y : i16
    %2 = comb.sub bin %x, %y {sv.namehint = "_x_T"} : i16
    %3 = comb.mux bin %1, %2, %x : i16
    %4 = comb.sub bin %y, %x {sv.namehint = "_y_T"} : i16
    %5 = comb.mux bin %1, %y, %4 : i16
    %6 = comb.icmp bin eq %x, %c0_i16 : i16
    %7 = comb.icmp bin eq %y, %c0_i16 : i16
    %8 = comb.or bin %6, %7 : i1
    %9 = comb.mux bin %6, %y, %x {sv.namehint = "output_bits_gcd"} : i16
    %10 = comb.and bin %output_ready, %resultValid : i1
    %11 = comb.and bin %8, %10 : i1
    %12 = comb.xor %11, %true : i1
    %13 = comb.and %12, %busy : i1
    %14 = comb.xor bin %10, %true : i1
    %15 = comb.and bin %busy, %8 : i1
    %16 = comb.mux bin %15, %14, %resultValid : i1
    %17 = comb.xor %busy, %true : i1
    %18 = comb.and %17, %input_valid : i1
    %19 = comb.or %18, %0 {sv.namehint = "input_ready"} : i1
    %20 = comb.mux bin %input_valid, %input_bits_value1, %x : i16
    %21 = comb.mux bin %busy, %3, %20 : i16
    %22 = comb.mux bin %input_valid, %input_bits_value2, %y : i16
    %23 = comb.mux bin %busy, %5, %22 : i16
    %24 = comb.xor %input_valid, %true : i1
    %25 = comb.or %busy, %24 : i1
    %26 = comb.mux bin %25, %xInitial, %input_bits_value1 : i16
    %27 = comb.xor %input_valid, %true : i1
    %28 = comb.or %busy, %27 : i1
    %29 = comb.mux bin %28, %yInitial, %input_bits_value2 : i16
    %30 = comb.or %input_valid, %busy : i1
    %31 = comb.mux bin %busy, %13, %30 : i1
    hw.output %19, %resultValid, %xInitial, %yInitial, %9 : i1, i1, i16, i16, i16
  }
  om.class @DecoupledGcd_Class(%basepath: !om.basepath) {
    om.class.fields
  }
}