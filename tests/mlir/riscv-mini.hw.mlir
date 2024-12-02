module {
  hw.module private @CSR(in %clock : !seq.clock, in %reset : i1, in %io_stall : i1, in %io_cmd : i3, in %io_in : i32, out io_out : i32, in %io_pc : i32, in %io_addr : i32, in %io_inst : i32, in %io_illegal : i1, in %io_st_type : i2, in %io_ld_type : i3, in %io_pc_check : i1, out io_expt : i1, out io_evec : i32, out io_epc : i32, in %io_host_fromhost_valid : i1, in %io_host_fromhost_bits : i32, out io_host_tohost : i32) {
    %true = hw.constant true
    %c0_i3 = hw.constant 0 : i3
    %c1_i3 = hw.constant 1 : i3
    %c0_i2 = hw.constant 0 : i2
    %c-8_i4 = hw.constant -8 : i4
    %false = hw.constant false
    %c1_i32 = hw.constant 1 : i32
    %c0_i24 = hw.constant 0 : i24
    %c256_i32 = hw.constant 256 : i32
    %c0_i26 = hw.constant 0 : i26
    %c0_i28 = hw.constant 0 : i28
    %c0_i6 = hw.constant 0 : i6
    %c-1_i32 = hw.constant -1 : i32
    %c0_i5 = hw.constant 0 : i5
    %c-1_i2 = hw.constant -1 : i2
    %c-1024_i12 = hw.constant -1024 : i12
    %c-1023_i12 = hw.constant -1023 : i12
    %c-1022_i12 = hw.constant -1022 : i12
    %c-896_i12 = hw.constant -896 : i12
    %c-895_i12 = hw.constant -895 : i12
    %c-894_i12 = hw.constant -894 : i12
    %c-1792_i12 = hw.constant -1792 : i12
    %c-1791_i12 = hw.constant -1791 : i12
    %c-1790_i12 = hw.constant -1790 : i12
    %c-1664_i12 = hw.constant -1664 : i12
    %c-1663_i12 = hw.constant -1663 : i12
    %c-1662_i12 = hw.constant -1662 : i12
    %c-256_i12 = hw.constant -256 : i12
    %c-255_i12 = hw.constant -255 : i12
    %c-240_i12 = hw.constant -240 : i12
    %c-4_i3 = hw.constant -4 : i3
    %c769_i12 = hw.constant 769 : i12
    %c770_i12 = hw.constant 770 : i12
    %c2_i3 = hw.constant 2 : i3
    %c3_i3 = hw.constant 3 : i3
    %c1_i2 = hw.constant 1 : i2
    %c-2_i2 = hw.constant -2 : i2
    %c19_i32 = hw.constant 19 : i32
    %c768_i12 = hw.constant 768 : i12
    %c836_i12 = hw.constant 836 : i12
    %c772_i12 = hw.constant 772 : i12
    %c1793_i12 = hw.constant 1793 : i12
    %c1857_i12 = hw.constant 1857 : i12
    %c801_i12 = hw.constant 801 : i12
    %c832_i12 = hw.constant 832 : i12
    %c833_i12 = hw.constant 833 : i12
    %c834_i12 = hw.constant 834 : i12
    %c-2147483633_i32 = hw.constant -2147483633 : i32
    %c835_i12 = hw.constant 835 : i12
    %c1920_i12 = hw.constant 1920 : i12
    %c1921_i12 = hw.constant 1921 : i12
    %c1048832_i32 = hw.constant 1048832 : i32
    %c0_i32 = hw.constant 0 : i32
    %c6_i4 = hw.constant 6 : i4
    %c4_i4 = hw.constant 4 : i4
    %c0_i4 = hw.constant 0 : i4
    %0 = comb.extract %io_inst from 20 {sv.namehint = "csr_addr"} : (i32) -> i12
    %1 = comb.extract %io_inst from 15 {sv.namehint = "rs1_addr"} : (i32) -> i5
    %time = seq.firreg %288 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 0 : ui64} : i32
    %timeh = seq.firreg %307 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 32 : ui64} : i32
    %cycle = seq.firreg %279 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 64 : ui64} : i32
    %cycleh = seq.firreg %298 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 96 : ui64} : i32
    %instret = seq.firreg %293 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 128 : ui64} : i32
    %instreth = seq.firreg %311 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 160 : ui64} : i32
    %PRV = seq.firreg %198 clock %clock reset sync %reset, %c-1_i2 {firrtl.random_init_start = 192 : ui64} : i2
    %PRV1 = seq.firreg %188 clock %clock reset sync %reset, %c-1_i2 {firrtl.random_init_start = 194 : ui64} : i2
    %IE = seq.firreg %204 clock %clock reset sync %reset, %false {firrtl.random_init_start = 196 : ui64} : i1
    %IE1 = seq.firreg %193 clock %clock reset sync %reset, %false {firrtl.random_init_start = 197 : ui64} : i1
    %2 = comb.concat %c0_i26, %PRV1, %IE1, %PRV, %IE {sv.namehint = "mstatus"} : i26, i2, i1, i2, i1
    %MTIP = seq.firreg %211 clock %clock reset sync %reset, %false {firrtl.random_init_start = 198 : ui64} : i1
    %MTIE = seq.firreg %223 clock %clock reset sync %reset, %false {firrtl.random_init_start = 199 : ui64} : i1
    %MSIP = seq.firreg %216 clock %clock reset sync %reset, %false {firrtl.random_init_start = 200 : ui64} : i1
    %MSIE = seq.firreg %228 clock %clock reset sync %reset, %false {firrtl.random_init_start = 201 : ui64} : i1
    %3 = comb.concat %c0_i24, %MTIP, %c0_i3, %MSIP, %c0_i3 {sv.namehint = "mip"} : i24, i1, i3, i1, i3
    %4 = comb.concat %c0_i24, %MTIE, %c0_i3, %MSIE, %c0_i3 {sv.namehint = "mie"} : i24, i1, i3, i1, i3
    %mtimecmp = seq.firreg %235 clock %clock {firrtl.random_init_start = 202 : ui64} : i32
    %mscratch = seq.firreg %240 clock %clock {firrtl.random_init_start = 234 : ui64} : i32
    %mepc = seq.firreg %249 clock %clock {firrtl.random_init_start = 266 : ui64, sv.namehint = "mepc"} : i32
    %mcause = seq.firreg %257 clock %clock {firrtl.random_init_start = 298 : ui64} : i32
    %mbadaddr = seq.firreg %264 clock %clock {firrtl.random_init_start = 330 : ui64} : i32
    %mtohost = seq.firreg %269 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 362 : ui64, sv.namehint = "mtohost"} : i32
    %mfromhost = seq.firreg %274 clock %clock {firrtl.random_init_start = 394 : ui64} : i32
    %5 = comb.mux bin %io_host_fromhost_valid, %io_host_fromhost_bits, %mfromhost : i32
    %6 = comb.icmp bin eq %0, %c-1024_i12 {sv.namehint = "_io_out_T_1"} : i12
    %7 = comb.icmp bin eq %0, %c-1023_i12 {sv.namehint = "_io_out_T_3"} : i12
    %8 = comb.icmp bin eq %0, %c-1022_i12 {sv.namehint = "_io_out_T_5"} : i12
    %9 = comb.icmp bin eq %0, %c-896_i12 {sv.namehint = "_io_out_T_7"} : i12
    %10 = comb.icmp bin eq %0, %c-895_i12 {sv.namehint = "_io_out_T_9"} : i12
    %11 = comb.icmp bin eq %0, %c-894_i12 {sv.namehint = "_io_out_T_11"} : i12
    %12 = comb.icmp bin eq %0, %c-1792_i12 {sv.namehint = "_io_out_T_13"} : i12
    %13 = comb.icmp bin eq %0, %c-1791_i12 {sv.namehint = "_io_out_T_15"} : i12
    %14 = comb.icmp bin eq %0, %c-1790_i12 {sv.namehint = "_io_out_T_17"} : i12
    %15 = comb.icmp bin eq %0, %c-1664_i12 {sv.namehint = "_io_out_T_19"} : i12
    %16 = comb.icmp bin eq %0, %c-1663_i12 {sv.namehint = "_io_out_T_21"} : i12
    %17 = comb.icmp bin eq %0, %c-1662_i12 {sv.namehint = "_io_out_T_23"} : i12
    %18 = comb.icmp bin eq %0, %c-256_i12 {sv.namehint = "_io_out_T_25"} : i12
    %19 = comb.icmp bin eq %0, %c-255_i12 {sv.namehint = "_io_out_T_27"} : i12
    %20 = comb.icmp bin eq %0, %c-240_i12 {sv.namehint = "_io_out_T_29"} : i12
    %21 = comb.icmp bin eq %0, %c769_i12 {sv.namehint = "_io_out_T_31"} : i12
    %22 = comb.icmp bin eq %0, %c770_i12 {sv.namehint = "_io_out_T_33"} : i12
    %23 = comb.icmp bin eq %0, %c772_i12 {sv.namehint = "_io_out_T_35"} : i12
    %24 = comb.icmp bin eq %0, %c801_i12 {sv.namehint = "_io_out_T_37"} : i12
    %25 = comb.icmp bin eq %0, %c1793_i12 {sv.namehint = "_io_out_T_39"} : i12
    %26 = comb.icmp bin eq %0, %c1857_i12 {sv.namehint = "_io_out_T_41"} : i12
    %27 = comb.icmp bin eq %0, %c832_i12 {sv.namehint = "_io_out_T_43"} : i12
    %28 = comb.icmp bin eq %0, %c833_i12 {sv.namehint = "_io_out_T_45"} : i12
    %29 = comb.icmp bin eq %0, %c834_i12 {sv.namehint = "_io_out_T_47"} : i12
    %30 = comb.icmp bin eq %0, %c835_i12 {sv.namehint = "_io_out_T_49"} : i12
    %31 = comb.icmp bin eq %0, %c836_i12 {sv.namehint = "_io_out_T_51"} : i12
    %32 = comb.icmp bin eq %0, %c1920_i12 {sv.namehint = "_io_out_T_53"} : i12
    %33 = comb.icmp bin eq %0, %c1921_i12 {sv.namehint = "_io_out_T_55"} : i12
    %34 = comb.icmp bin eq %0, %c768_i12 {sv.namehint = "_io_out_T_57"} : i12
    %35 = comb.mux bin %34, %2, %c0_i32 {sv.namehint = "_io_out_T_58"} : i32
    %36 = comb.mux bin %33, %mfromhost, %35 {sv.namehint = "_io_out_T_59"} : i32
    %37 = comb.mux bin %32, %mtohost, %36 {sv.namehint = "_io_out_T_60"} : i32
    %38 = comb.mux bin %31, %3, %37 {sv.namehint = "_io_out_T_61"} : i32
    %39 = comb.mux bin %30, %mbadaddr, %38 {sv.namehint = "_io_out_T_62"} : i32
    %40 = comb.mux bin %29, %mcause, %39 {sv.namehint = "_io_out_T_63"} : i32
    %41 = comb.mux bin %28, %mepc, %40 {sv.namehint = "_io_out_T_64"} : i32
    %42 = comb.mux bin %27, %mscratch, %41 {sv.namehint = "_io_out_T_65"} : i32
    %43 = comb.mux bin %26, %timeh, %42 {sv.namehint = "_io_out_T_66"} : i32
    %44 = comb.mux bin %25, %time, %43 {sv.namehint = "_io_out_T_67"} : i32
    %45 = comb.mux bin %24, %mtimecmp, %44 {sv.namehint = "_io_out_T_68"} : i32
    %46 = comb.mux bin %23, %4, %45 {sv.namehint = "_io_out_T_69"} : i32
    %47 = comb.mux bin %22, %c0_i32, %46 {sv.namehint = "_io_out_T_70"} : i32
    %48 = comb.mux bin %21, %c256_i32, %47 {sv.namehint = "_io_out_T_71"} : i32
    %49 = comb.or bin %19, %20 : i1
    %50 = comb.mux bin %49, %c0_i32, %48 {sv.namehint = "_io_out_T_73"} : i32
    %51 = comb.mux bin %18, %c1048832_i32, %50 {sv.namehint = "_io_out_T_74"} : i32
    %52 = comb.mux bin %17, %instreth, %51 {sv.namehint = "_io_out_T_75"} : i32
    %53 = comb.mux bin %16, %timeh, %52 {sv.namehint = "_io_out_T_76"} : i32
    %54 = comb.mux bin %15, %cycleh, %53 {sv.namehint = "_io_out_T_77"} : i32
    %55 = comb.mux bin %14, %instret, %54 {sv.namehint = "_io_out_T_78"} : i32
    %56 = comb.mux bin %13, %time, %55 {sv.namehint = "_io_out_T_79"} : i32
    %57 = comb.mux bin %12, %cycle, %56 {sv.namehint = "_io_out_T_80"} : i32
    %58 = comb.mux bin %11, %instreth, %57 {sv.namehint = "_io_out_T_81"} : i32
    %59 = comb.mux bin %10, %timeh, %58 {sv.namehint = "_io_out_T_82"} : i32
    %60 = comb.mux bin %9, %cycleh, %59 {sv.namehint = "_io_out_T_83"} : i32
    %61 = comb.mux bin %8, %instret, %60 {sv.namehint = "_io_out_T_84"} : i32
    %62 = comb.mux bin %7, %time, %61 {sv.namehint = "_io_out_T_85"} : i32
    %63 = comb.mux bin %6, %cycle, %62 {sv.namehint = "io_out"} : i32
    %64 = comb.extract %io_inst from 28 {sv.namehint = "_privValid_T"} : (i32) -> i2
    %65 = comb.icmp bin eq %io_cmd, %c-4_i3 {sv.namehint = "privInst"} : i3
    %66 = comb.extract %io_inst from 20 {sv.namehint = "_isEret_T"} : (i32) -> i1
    %67 = comb.xor bin %66, %true {sv.namehint = "_isEcall_T_1"} : i1
    %68 = comb.extract %io_inst from 28 {sv.namehint = "_isEret_T_3"} : (i32) -> i1
    %69 = comb.xor bin %68, %true {sv.namehint = "_isEcall_T_4"} : i1
    %70 = comb.and bin %65, %67, %69 {sv.namehint = "isEcall"} : i1
    %71 = comb.xor bin %68, %true {sv.namehint = "_isEbreak_T_3"} : i1
    %72 = comb.and bin %65, %66, %71 {sv.namehint = "isEbreak"} : i1
    %73 = comb.xor bin %66, %true {sv.namehint = "_isEret_T_1"} : i1
    %74 = comb.and bin %65, %73, %68 {sv.namehint = "isEret"} : i1
    %75 = comb.icmp bin eq %0, %c-1024_i12 {sv.namehint = "_csrValid_T_1"} : i12
    %76 = comb.icmp bin eq %0, %c-1023_i12 {sv.namehint = "_csrValid_T_3"} : i12
    %77 = comb.icmp bin eq %0, %c-1022_i12 {sv.namehint = "_csrValid_T_5"} : i12
    %78 = comb.icmp bin eq %0, %c-896_i12 {sv.namehint = "_csrValid_T_7"} : i12
    %79 = comb.icmp bin eq %0, %c-895_i12 {sv.namehint = "_csrValid_T_9"} : i12
    %80 = comb.icmp bin eq %0, %c-894_i12 {sv.namehint = "_csrValid_T_11"} : i12
    %81 = comb.icmp bin eq %0, %c-1792_i12 {sv.namehint = "_csrValid_T_13"} : i12
    %82 = comb.icmp bin eq %0, %c-1791_i12 {sv.namehint = "_csrValid_T_15"} : i12
    %83 = comb.icmp bin eq %0, %c-1790_i12 {sv.namehint = "_csrValid_T_17"} : i12
    %84 = comb.icmp bin eq %0, %c-1664_i12 {sv.namehint = "_csrValid_T_19"} : i12
    %85 = comb.icmp bin eq %0, %c-1663_i12 {sv.namehint = "_csrValid_T_21"} : i12
    %86 = comb.icmp bin eq %0, %c-1662_i12 {sv.namehint = "_csrValid_T_23"} : i12
    %87 = comb.icmp bin eq %0, %c-256_i12 {sv.namehint = "_csrValid_T_25"} : i12
    %88 = comb.icmp bin eq %0, %c-255_i12 {sv.namehint = "_csrValid_T_27"} : i12
    %89 = comb.icmp bin eq %0, %c-240_i12 {sv.namehint = "_csrValid_T_29"} : i12
    %90 = comb.icmp bin eq %0, %c769_i12 {sv.namehint = "_csrValid_T_31"} : i12
    %91 = comb.icmp bin eq %0, %c770_i12 {sv.namehint = "_csrValid_T_33"} : i12
    %92 = comb.icmp bin eq %0, %c772_i12 {sv.namehint = "_csrValid_T_35"} : i12
    %93 = comb.icmp bin eq %0, %c801_i12 {sv.namehint = "_csrValid_T_37"} : i12
    %94 = comb.icmp bin eq %0, %c1793_i12 {sv.namehint = "_csrValid_T_39"} : i12
    %95 = comb.icmp bin eq %0, %c1857_i12 {sv.namehint = "_csrValid_T_41"} : i12
    %96 = comb.icmp bin eq %0, %c832_i12 {sv.namehint = "_csrValid_T_43"} : i12
    %97 = comb.icmp bin eq %0, %c833_i12 {sv.namehint = "_csrValid_T_45"} : i12
    %98 = comb.icmp bin eq %0, %c834_i12 {sv.namehint = "_csrValid_T_47"} : i12
    %99 = comb.icmp bin eq %0, %c835_i12 {sv.namehint = "_csrValid_T_49"} : i12
    %100 = comb.icmp bin eq %0, %c836_i12 {sv.namehint = "_csrValid_T_51"} : i12
    %101 = comb.icmp bin eq %0, %c1920_i12 {sv.namehint = "_csrValid_T_53"} : i12
    %102 = comb.icmp bin eq %0, %c1921_i12 {sv.namehint = "_csrValid_T_55"} : i12
    %103 = comb.icmp bin eq %0, %c768_i12 {sv.namehint = "_csrValid_T_57"} : i12
    %104 = comb.or bin %75, %76, %77, %78, %79, %80, %81, %82, %83, %84, %85, %86, %87, %88, %89, %90, %91, %92, %93, %94, %95, %96, %97, %98, %99, %100, %101, %102, %103 {sv.namehint = "csrValid"} : i1
    %105 = comb.extract %io_inst from 30 {sv.namehint = "_csrRO_T"} : (i32) -> i2
    %106 = comb.icmp bin eq %105, %c-1_i2 {sv.namehint = "_csrRO_T_1"} : i2
    %107 = comb.icmp bin eq %0, %c769_i12 {sv.namehint = "_csrRO_T_2"} : i12
    %108 = comb.icmp bin eq %0, %c770_i12 {sv.namehint = "_csrRO_T_4"} : i12
    %109 = comb.or bin %106, %107, %108 {sv.namehint = "csrRO"} : i1
    %110 = comb.icmp bin eq %io_cmd, %c1_i3 {sv.namehint = "_wen_T"} : i3
    %111 = comb.extract %io_cmd from 1 {sv.namehint = "_wen_T_1"} : (i3) -> i1
    %112 = comb.icmp bin ne %1, %c0_i5 {sv.namehint = "_wen_T_2"} : i5
    %113 = comb.and bin %111, %112 {sv.namehint = "_wen_T_3"} : i1
    %114 = comb.or bin %110, %113 {sv.namehint = "wen"} : i1
    %115 = comb.or bin %63, %io_in {sv.namehint = "_wdata_T"} : i32
    %116 = comb.xor bin %io_in, %c-1_i32 {sv.namehint = "_wdata_T_1"} : i32
    %117 = comb.and bin %63, %116 {sv.namehint = "_wdata_T_2"} : i32
    %118 = comb.icmp bin eq %io_cmd, %c1_i3 {sv.namehint = "_wdata_T_3"} : i3
    %119 = comb.mux bin %118, %io_in, %c0_i32 {sv.namehint = "_wdata_T_4"} : i32
    %120 = comb.icmp bin eq %io_cmd, %c2_i3 {sv.namehint = "_wdata_T_5"} : i3
    %121 = comb.mux bin %120, %115, %119 {sv.namehint = "_wdata_T_6"} : i32
    %122 = comb.icmp bin eq %io_cmd, %c3_i3 {sv.namehint = "_wdata_T_7"} : i3
    %123 = comb.mux bin %122, %117, %121 {sv.namehint = "wdata"} : i32
    %124 = comb.extract %io_addr from 1 {sv.namehint = "_iaddrInvalid_T"} : (i32) -> i1
    %125 = comb.and bin %io_pc_check, %124 {sv.namehint = "iaddrInvalid"} : i1
    %126 = comb.extract %io_addr from 0 {sv.namehint = "_saddrInvalid_T"} : (i32) -> i2
    %127 = comb.icmp bin ne %126, %c0_i2 {sv.namehint = "_laddrInvalid_T_1"} : i2
    %128 = comb.extract %io_addr from 0 {sv.namehint = "_saddrInvalid_T_2"} : (i32) -> i1
    %129 = comb.icmp bin eq %io_ld_type, %c1_i3 {sv.namehint = "_laddrInvalid_T_4"} : i3
    %130 = comb.and %129, %127 {sv.namehint = "_laddrInvalid_T_5"} : i1
    %131 = comb.icmp bin eq %io_ld_type, %c2_i3 {sv.namehint = "_laddrInvalid_T_6"} : i3
    %132 = comb.icmp bin eq %io_ld_type, %c-4_i3 {sv.namehint = "_laddrInvalid_T_8"} : i3
    %133 = comb.or bin %132, %131 : i1
    %134 = comb.mux bin %133, %128, %130 {sv.namehint = "laddrInvalid"} : i1
    %135 = comb.icmp bin ne %126, %c0_i2 {sv.namehint = "_saddrInvalid_T_1"} : i2
    %136 = comb.icmp bin eq %io_st_type, %c1_i2 {sv.namehint = "_saddrInvalid_T_3"} : i2
    %137 = comb.and %136, %135 {sv.namehint = "_saddrInvalid_T_4"} : i1
    %138 = comb.icmp bin eq %io_st_type, %c-2_i2 {sv.namehint = "_saddrInvalid_T_5"} : i2
    %139 = comb.mux bin %138, %128, %137 {sv.namehint = "saddrInvalid"} : i1
    %140 = comb.extract %io_cmd from 0 {sv.namehint = "_io_expt_T_3"} : (i3) -> i2
    %141 = comb.icmp bin ne %140, %c0_i2 {sv.namehint = "_io_expt_T_4"} : i2
    %142 = comb.xor bin %104, %true {sv.namehint = "_io_expt_T_5"} : i1
    %143 = comb.icmp bin ugt %64, %PRV {sv.namehint = "_io_expt_T_12"} : i2
    %144 = comb.or bin %142, %143 {sv.namehint = "_io_expt_T_7"} : i1
    %145 = comb.and bin %141, %144 {sv.namehint = "_io_expt_T_8"} : i1
    %146 = comb.and bin %114, %109 {sv.namehint = "_io_expt_T_10"} : i1
    %147 = comb.and bin %65, %143 {sv.namehint = "_io_expt_T_13"} : i1
    %148 = comb.or bin %io_illegal, %125, %134, %139, %145, %146, %147, %70, %72 {sv.namehint = "io_expt"} : i1
    %149 = comb.concat %c0_i24, %PRV, %c0_i6 : i24, i2, i6
    %150 = comb.add %149, %c256_i32 {sv.namehint = "_io_evec_T_1"} : i32
    %151 = comb.add %time, %c1_i32 {sv.namehint = "_time_T"} : i32
    %152 = comb.icmp bin eq %time, %c-1_i32 : i32
    %153 = comb.add %timeh, %c1_i32 {sv.namehint = "_timeh_T"} : i32
    %154 = comb.mux bin %152, %153, %timeh : i32
    %155 = comb.add %cycle, %c1_i32 {sv.namehint = "_cycle_T"} : i32
    %156 = comb.icmp bin eq %cycle, %c-1_i32 : i32
    %157 = comb.add %cycleh, %c1_i32 {sv.namehint = "_cycleh_T"} : i32
    %158 = comb.mux bin %156, %157, %cycleh : i32
    %159 = comb.icmp bin ne %io_inst, %c19_i32 {sv.namehint = "_isInstRet_T"} : i32
    %160 = comb.xor bin %148, %true {sv.namehint = "_isInstRet_T_1"} : i1
    %161 = comb.or bin %160, %70, %72 {sv.namehint = "_isInstRet_T_3"} : i1
    %162 = comb.xor bin %io_stall, %true {sv.namehint = "_isInstRet_T_5"} : i1
    %163 = comb.and bin %159, %161, %162 {sv.namehint = "isInstRet"} : i1
    %164 = comb.add %instret, %c1_i32 {sv.namehint = "_instret_T"} : i32
    %165 = comb.mux bin %163, %164, %instret : i32
    %166 = comb.icmp bin eq %instret, %c-1_i32 : i32
    %167 = comb.and bin %163, %166 : i1
    %168 = comb.add %instreth, %c1_i32 {sv.namehint = "_instreth_T"} : i32
    %169 = comb.mux bin %167, %168, %instreth : i32
    %170 = comb.extract %io_pc from 2 {sv.namehint = "_mepc_T"} : (i32) -> i30
    %171 = comb.concat %170, %c0_i2 {sv.namehint = "_mepc_T_1"} : i30, i2
    %172 = comb.concat %c0_i2, %PRV : i2, i2
    %173 = comb.add %172, %c-8_i4 {sv.namehint = "_mcause_T"} : i4
    %174 = comb.concat %c1_i3, %72 : i3, i1
    %175 = comb.mux bin %70, %173, %174 {sv.namehint = "_mcause_T_3"} : i4
    %176 = comb.mux bin %139, %c6_i4, %175 {sv.namehint = "_mcause_T_4"} : i4
    %177 = comb.mux bin %134, %c4_i4, %176 {sv.namehint = "_mcause_T_5"} : i4
    %178 = comb.mux bin %125, %c0_i4, %177 {sv.namehint = "_mcause_T_6"} : i4
    %179 = comb.concat %c0_i28, %178 : i28, i4
    %180 = comb.or bin %125, %134, %139 : i1
    %181 = comb.mux bin %180, %io_addr, %mbadaddr : i32
    %182 = comb.icmp bin eq %0, %c768_i12 : i12
    %183 = comb.extract %123 from 4 {sv.namehint = "_PRV1_T"} : (i32) -> i2
    %184 = comb.and bin %114, %182 : i1
    %185 = comb.mux bin %184, %183, %PRV1 : i2
    %186 = comb.mux bin %74, %c0_i2, %185 : i2
    %187 = comb.mux bin %148, %PRV, %186 : i2
    %188 = comb.mux bin %io_stall, %PRV1, %187 : i2
    %189 = comb.extract %123 from 3 {sv.namehint = "_IE1_T"} : (i32) -> i1
    %190 = comb.mux bin %184, %189, %IE1 : i1
    %191 = comb.or %74, %190 : i1
    %192 = comb.mux bin %148, %IE, %191 : i1
    %193 = comb.mux bin %io_stall, %IE1, %192 : i1
    %194 = comb.extract %123 from 1 {sv.namehint = "_PRV_T"} : (i32) -> i2
    %195 = comb.mux bin %184, %194, %PRV : i2
    %196 = comb.mux bin %74, %PRV1, %195 : i2
    %197 = comb.mux bin %148, %c-1_i2, %196 : i2
    %198 = comb.mux bin %io_stall, %PRV, %197 : i2
    %199 = comb.extract %123 from 0 {sv.namehint = "_IE_T"} : (i32) -> i1
    %200 = comb.mux bin %184, %199, %IE : i1
    %201 = comb.mux bin %74, %IE1, %200 : i1
    %202 = comb.xor %148, %true : i1
    %203 = comb.and %202, %201 : i1
    %204 = comb.mux bin %io_stall, %IE, %203 : i1
    %205 = comb.icmp bin eq %0, %c836_i12 : i12
    %206 = comb.extract %123 from 7 {sv.namehint = "_MTIP_T"} : (i32) -> i1
    %207 = comb.xor %205, %true : i1
    %208 = comb.xor %114, %true : i1
    %209 = comb.or bin %148, %74 : i1
    %210 = comb.or %io_stall, %209, %208, %182, %207 : i1
    %211 = comb.mux bin %210, %MTIP, %206 : i1
    %212 = comb.extract %123 from 3 {sv.namehint = "_MSIP_T"} : (i32) -> i1
    %213 = comb.xor %205, %true : i1
    %214 = comb.xor %114, %true : i1
    %215 = comb.or %io_stall, %209, %214, %182, %213 : i1
    %216 = comb.mux bin %215, %MSIP, %212 : i1
    %217 = comb.icmp bin eq %0, %c772_i12 : i12
    %218 = comb.extract %123 from 7 {sv.namehint = "_MTIE_T"} : (i32) -> i1
    %219 = comb.or bin %182, %205 : i1
    %220 = comb.xor %217, %true : i1
    %221 = comb.xor %114, %true : i1
    %222 = comb.or %io_stall, %209, %221, %219, %220 : i1
    %223 = comb.mux bin %222, %MTIE, %218 : i1
    %224 = comb.extract %123 from 3 {sv.namehint = "_MSIE_T"} : (i32) -> i1
    %225 = comb.xor %217, %true : i1
    %226 = comb.xor %114, %true : i1
    %227 = comb.or %io_stall, %209, %226, %219, %225 : i1
    %228 = comb.mux bin %227, %MSIE, %224 : i1
    %229 = comb.icmp bin eq %0, %c1793_i12 : i12
    %230 = comb.icmp bin eq %0, %c1857_i12 : i12
    %231 = comb.icmp bin eq %0, %c801_i12 : i12
    %232 = comb.xor %231, %true : i1
    %233 = comb.xor %114, %true : i1
    %234 = comb.or %io_stall, %209, %233, %182, %205, %217, %229, %230, %232 : i1
    %235 = comb.mux bin %234, %mtimecmp, %123 : i32
    %236 = comb.icmp bin eq %0, %c832_i12 : i12
    %237 = comb.xor %236, %true : i1
    %238 = comb.xor %114, %true : i1
    %239 = comb.or %io_stall, %209, %238, %182, %205, %217, %229, %230, %231, %237 : i1
    %240 = comb.mux bin %239, %mscratch, %123 : i32
    %241 = comb.icmp bin eq %0, %c833_i12 : i12
    %242 = comb.extract %123 from 2 : (i32) -> i30
    %243 = comb.concat %242, %c0_i2 : i30, i2
    %244 = comb.xor %241, %true : i1
    %245 = comb.xor %114, %true : i1
    %246 = comb.or %74, %245, %182, %205, %217, %229, %230, %231, %236, %244 : i1
    %247 = comb.mux bin %246, %mepc, %243 : i32
    %248 = comb.mux bin %148, %171, %247 : i32
    %249 = comb.mux bin %io_stall, %mepc, %248 : i32
    %250 = comb.icmp bin eq %0, %c834_i12 : i12
    %251 = comb.and bin %123, %c-2147483633_i32 {sv.namehint = "_mcause_T_7"} : i32
    %252 = comb.xor %250, %true : i1
    %253 = comb.xor %114, %true : i1
    %254 = comb.or %74, %253, %182, %205, %217, %229, %230, %231, %236, %241, %252 : i1
    %255 = comb.mux bin %254, %mcause, %251 : i32
    %256 = comb.mux bin %148, %179, %255 : i32
    %257 = comb.mux bin %io_stall, %mcause, %256 : i32
    %258 = comb.icmp bin eq %0, %c835_i12 : i12
    %259 = comb.xor %258, %true : i1
    %260 = comb.xor %114, %true : i1
    %261 = comb.or %74, %260, %182, %205, %217, %229, %230, %231, %236, %241, %250, %259 : i1
    %262 = comb.mux bin %261, %mbadaddr, %123 : i32
    %263 = comb.mux bin %148, %181, %262 : i32
    %264 = comb.mux bin %io_stall, %mbadaddr, %263 : i32
    %265 = comb.icmp bin eq %0, %c1920_i12 : i12
    %266 = comb.xor %265, %true : i1
    %267 = comb.xor %114, %true : i1
    %268 = comb.or %io_stall, %209, %267, %182, %205, %217, %229, %230, %231, %236, %241, %250, %258, %266 : i1
    %269 = comb.mux bin %268, %mtohost, %123 : i32
    %270 = comb.icmp bin eq %0, %c1921_i12 : i12
    %271 = comb.xor %270, %true : i1
    %272 = comb.xor %114, %true : i1
    %273 = comb.or %io_stall, %209, %272, %182, %205, %217, %229, %230, %231, %236, %241, %250, %258, %265, %271 : i1
    %274 = comb.mux bin %273, %5, %123 : i32
    %275 = comb.icmp bin eq %0, %c-1792_i12 : i12
    %276 = comb.xor %275, %true : i1
    %277 = comb.xor %114, %true : i1
    %278 = comb.or %io_stall, %209, %277, %182, %205, %217, %229, %230, %231, %236, %241, %250, %258, %265, %270, %276 : i1
    %279 = comb.mux bin %278, %155, %123 : i32
    %280 = comb.icmp bin eq %0, %c-1791_i12 : i12
    %281 = comb.xor %280, %true : i1
    %282 = comb.or %230, %231, %236, %241, %250, %258, %265, %270, %275, %281 : i1
    %283 = comb.xor %282, %true : i1
    %284 = comb.or %229, %283 : i1
    %285 = comb.xor %284, %true : i1
    %286 = comb.xor %114, %true : i1
    %287 = comb.or %io_stall, %209, %286, %182, %205, %217, %285 : i1
    %288 = comb.mux bin %287, %151, %123 : i32
    %289 = comb.icmp bin eq %0, %c-1790_i12 : i12
    %290 = comb.xor %289, %true : i1
    %291 = comb.xor %114, %true : i1
    %292 = comb.or %io_stall, %209, %291, %182, %205, %217, %229, %230, %231, %236, %241, %250, %258, %265, %270, %275, %280, %290 : i1
    %293 = comb.mux bin %292, %165, %123 : i32
    %294 = comb.icmp bin eq %0, %c-1664_i12 : i12
    %295 = comb.xor %294, %true : i1
    %296 = comb.xor %114, %true : i1
    %297 = comb.or %io_stall, %209, %296, %182, %205, %217, %229, %230, %231, %236, %241, %250, %258, %265, %270, %275, %280, %289, %295 : i1
    %298 = comb.mux bin %297, %158, %123 : i32
    %299 = comb.icmp bin eq %0, %c-1663_i12 : i12
    %300 = comb.xor %299, %true : i1
    %301 = comb.or %231, %236, %241, %250, %258, %265, %270, %275, %280, %289, %294, %300 : i1
    %302 = comb.xor %301, %true : i1
    %303 = comb.or %230, %302 : i1
    %304 = comb.xor %303, %true : i1
    %305 = comb.xor %114, %true : i1
    %306 = comb.or %io_stall, %209, %305, %182, %205, %217, %229, %304 : i1
    %307 = comb.mux bin %306, %154, %123 : i32
    %308 = comb.icmp bin ne %0, %c-1662_i12 : i12
    %309 = comb.xor %114, %true : i1
    %310 = comb.or %io_stall, %209, %309, %182, %205, %217, %229, %230, %231, %236, %241, %250, %258, %265, %270, %275, %280, %289, %294, %299, %308 : i1
    %311 = comb.mux bin %310, %169, %123 : i32
    hw.output %63, %148, %150, %mepc, %mtohost : i32, i1, i32, i32, i32
  }
  hw.module private @RegFile(in %clock : !seq.clock, in %io_raddr1 : i5, in %io_raddr2 : i5, out io_rdata1 : i32, out io_rdata2 : i32, in %io_wen : i1, in %io_waddr : i5, in %io_wdata : i32) {
    %c0_i5 = hw.constant 0 : i5
    %c0_i32 = hw.constant 0 : i32
    %regs = seq.firmem 0, 1, undefined, port_order {prefix = ""} : <32 x 32>
    seq.firmem.write_port %regs[%io_waddr] = %io_wdata, clock %clock enable %7 : <32 x 32>
    %0 = seq.firmem.read_port %regs[%io_raddr1], clock %clock : <32 x 32>
    %1 = seq.firmem.read_port %regs[%io_raddr2], clock %clock : <32 x 32>
    %2 = comb.icmp bin ne %io_raddr1, %c0_i5 {sv.namehint = "_io_rdata1_T"} : i5
    %3 = comb.mux bin %2, %0, %c0_i32 {sv.namehint = "io_rdata1"} : i32
    %4 = comb.icmp bin ne %io_raddr2, %c0_i5 {sv.namehint = "_io_rdata2_T"} : i5
    %5 = comb.mux bin %4, %1, %c0_i32 {sv.namehint = "io_rdata2"} : i32
    %6 = comb.icmp bin ne %io_waddr, %c0_i5 : i5
    %7 = comb.and bin %io_wen, %6 : i1
    hw.output %3, %5 : i32, i32
  }
  hw.module private @AluArea(in %io_A : i32, in %io_B : i32, in %io_alu_op : i4, out io_out : i32, out io_sum : i32) {
    %false = hw.constant false
    %c-174763_i19 = hw.constant -174763 : i19
    %c0_i2 = hw.constant 0 : i2
    %c-13_i6 = hw.constant -13 : i6
    %c0_i4 = hw.constant 0 : i4
    %c5_i4 = hw.constant 5 : i4
    %c85_i8 = hw.constant 85 : i8
    %c3_i4 = hw.constant 3 : i4
    %c51_i8 = hw.constant 51 : i8
    %c0_i32 = hw.constant 0 : i32
    %c0_i31 = hw.constant 0 : i31
    %c0_i28 = hw.constant 0 : i28
    %c1_i4 = hw.constant 1 : i4
    %c7_i4 = hw.constant 7 : i4
    %c-7_i4 = hw.constant -7 : i4
    %c-8_i4 = hw.constant -8 : i4
    %c6_i4 = hw.constant 6 : i4
    %c2_i4 = hw.constant 2 : i4
    %c4_i4 = hw.constant 4 : i4
    %c-6_i4 = hw.constant -6 : i4
    %0 = comb.extract %io_alu_op from 0 {sv.namehint = "_shiftr_T"} : (i4) -> i1
    %1 = comb.sub %c0_i32, %io_B {sv.namehint = "_sum_T_2"} : i32
    %2 = comb.mux bin %0, %1, %io_B {sv.namehint = "_sum_T_3"} : i32
    %3 = comb.add %io_A, %2 {sv.namehint = "_sum_T_4"} : i32
    %4 = comb.extract %io_A from 31 {sv.namehint = "_cmp_T_6"} : (i32) -> i1
    %5 = comb.extract %io_B from 31 {sv.namehint = "_cmp_T_5"} : (i32) -> i1
    %6 = comb.icmp bin eq %4, %5 {sv.namehint = "_cmp_T_2"} : i1
    %7 = comb.extract %3 from 31 {sv.namehint = "_cmp_T_3"} : (i32) -> i1
    %8 = comb.extract %io_alu_op from 1 {sv.namehint = "_cmp_T_4"} : (i4) -> i1
    %9 = comb.mux bin %8, %5, %4 {sv.namehint = "_cmp_T_7"} : i1
    %10 = comb.mux bin %6, %7, %9 {sv.namehint = "cmp"} : i1
    %11 = comb.extract %io_B from 0 {sv.namehint = "shamt"} : (i32) -> i5
    %12 = comb.extract %io_alu_op from 3 {sv.namehint = "_shin_T"} : (i4) -> i1
    %13 = comb.extract %io_A from 8 : (i32) -> i4
    %14 = comb.extract %io_A from 16 : (i32) -> i4
    %15 = comb.extract %io_A from 12 : (i32) -> i4
    %16 = comb.concat %15, %14 : i4, i4
    %17 = comb.extract %io_A from 14 : (i32) -> i2
    %18 = comb.concat %13, %17 : i4, i2
    %19 = comb.and %18, %c-13_i6 : i6
    %20 = comb.extract %io_A from 4 : (i32) -> i2
    %21 = comb.extract %io_A from 8 : (i32) -> i2
    %22 = comb.and %16, %c51_i8 : i8
    %23 = comb.extract %io_A from 20 : (i32) -> i2
    %24 = comb.extract %io_A from 24 : (i32) -> i2
    %25 = comb.extract %io_A from 6 : (i32) -> i2
    %26 = comb.concat %25, %21 : i2, i2
    %27 = comb.concat %19, %c0_i2 : i6, i2
    %28 = comb.or %27, %22 : i8
    %29 = comb.extract %io_A from 18 : (i32) -> i2
    %30 = comb.extract %io_A from 22 : (i32) -> i2
    %31 = comb.concat %30, %24 : i2, i2
    %32 = comb.extract %io_A from 31 : (i32) -> i1
    %33 = comb.extract %io_A from 23 : (i32) -> i1
    %34 = comb.concat %20, %25, %21, %28, %29, %23, %33 : i2, i2, i2, i8, i2, i2, i1
    %35 = comb.and %34, %c-174763_i19 : i19
    %36 = comb.extract %io_A from 0 : (i32) -> i1
    %37 = comb.extract %io_A from 2 : (i32) -> i1
    %38 = comb.extract %io_A from 4 : (i32) -> i1
    %39 = comb.and %26, %c5_i4 : i4
    %40 = comb.and %28, %c85_i8 : i8
    %41 = comb.extract %io_A from 18 : (i32) -> i1
    %42 = comb.extract %io_A from 20 : (i32) -> i1
    %43 = comb.and %31, %c5_i4 : i4
    %44 = comb.extract %io_A from 26 : (i32) -> i1
    %45 = comb.extract %io_A from 28 : (i32) -> i1
    %46 = comb.extract %io_A from 30 : (i32) -> i1
    %47 = comb.extract %io_A from 1 : (i32) -> i1
    %48 = comb.extract %io_A from 3 : (i32) -> i1
    %49 = comb.extract %35 from 15 : (i19) -> i4
    %50 = comb.or %49, %39 : i4
    %51 = comb.extract %35 from 7 : (i19) -> i8
    %52 = comb.or %51, %40 : i8
    %53 = comb.extract %28 from 1 : (i8) -> i1
    %54 = comb.extract %35 from 5 : (i19) -> i1
    %55 = comb.or %54, %41 : i1
    %56 = comb.extract %io_A from 19 : (i32) -> i1
    %57 = comb.extract %35 from 0 : (i19) -> i3
    %58 = comb.concat %57, %false : i3, i1
    %59 = comb.or %58, %43 : i4
    %60 = comb.extract %io_A from 25 : (i32) -> i1
    %61 = comb.extract %io_A from 27 : (i32) -> i1
    %62 = comb.extract %io_A from 29 : (i32) -> i1
    %63 = comb.concat %36, %47, %37, %48, %38, %50, %52, %53, %55, %56, %42, %59, %60, %44, %61, %45, %62, %46, %32 : i1, i1, i1, i1, i1, i4, i8, i1, i1, i1, i1, i4, i1, i1, i1, i1, i1, i1, i1
    %64 = comb.mux bin %12, %io_A, %63 {sv.namehint = "shin"} : i32
    %65 = comb.extract %64 from 31 {sv.namehint = "_shiftr_T_1"} : (i32) -> i1
    %66 = comb.and bin %0, %65 {sv.namehint = "_shiftr_T_2"} : i1
    %67 = comb.concat %66, %64 {sv.namehint = "_shiftr_T_3"} : i1, i32
    %68 = comb.concat %c0_i28, %11 : i28, i5
    %69 = comb.shrs bin %67, %68 {sv.namehint = "_shiftr_T_5"} : i33
    %70 = comb.extract %69 from 0 {sv.namehint = "shiftr"} : (i33) -> i32
    %71 = comb.extract %69 from 8 : (i33) -> i4
    %72 = comb.extract %69 from 16 : (i33) -> i4
    %73 = comb.extract %69 from 12 : (i33) -> i4
    %74 = comb.concat %73, %72 : i4, i4
    %75 = comb.extract %69 from 14 : (i33) -> i2
    %76 = comb.concat %71, %75 : i4, i2
    %77 = comb.and %76, %c-13_i6 : i6
    %78 = comb.extract %69 from 4 : (i33) -> i2
    %79 = comb.extract %69 from 8 : (i33) -> i2
    %80 = comb.and %74, %c51_i8 : i8
    %81 = comb.extract %69 from 20 : (i33) -> i2
    %82 = comb.extract %69 from 24 : (i33) -> i2
    %83 = comb.extract %69 from 6 : (i33) -> i2
    %84 = comb.concat %83, %79 : i2, i2
    %85 = comb.concat %77, %c0_i2 : i6, i2
    %86 = comb.or %85, %80 : i8
    %87 = comb.extract %69 from 18 : (i33) -> i2
    %88 = comb.extract %69 from 22 : (i33) -> i2
    %89 = comb.concat %88, %82 : i2, i2
    %90 = comb.extract %69 from 31 : (i33) -> i1
    %91 = comb.extract %69 from 23 : (i33) -> i1
    %92 = comb.concat %78, %83, %79, %86, %87, %81, %91 : i2, i2, i2, i8, i2, i2, i1
    %93 = comb.and %92, %c-174763_i19 : i19
    %94 = comb.extract %69 from 0 : (i33) -> i1
    %95 = comb.extract %69 from 2 : (i33) -> i1
    %96 = comb.extract %69 from 4 : (i33) -> i1
    %97 = comb.and %84, %c5_i4 : i4
    %98 = comb.and %86, %c85_i8 : i8
    %99 = comb.extract %69 from 18 : (i33) -> i1
    %100 = comb.extract %69 from 20 : (i33) -> i1
    %101 = comb.and %89, %c5_i4 : i4
    %102 = comb.extract %69 from 26 : (i33) -> i1
    %103 = comb.extract %69 from 28 : (i33) -> i1
    %104 = comb.extract %69 from 30 : (i33) -> i1
    %105 = comb.extract %69 from 1 : (i33) -> i1
    %106 = comb.extract %69 from 3 : (i33) -> i1
    %107 = comb.extract %93 from 15 : (i19) -> i4
    %108 = comb.or %107, %97 : i4
    %109 = comb.extract %93 from 7 : (i19) -> i8
    %110 = comb.or %109, %98 : i8
    %111 = comb.extract %86 from 1 : (i8) -> i1
    %112 = comb.extract %93 from 5 : (i19) -> i1
    %113 = comb.or %112, %99 : i1
    %114 = comb.extract %69 from 19 : (i33) -> i1
    %115 = comb.extract %93 from 0 : (i19) -> i3
    %116 = comb.concat %115, %false : i3, i1
    %117 = comb.or %116, %101 : i4
    %118 = comb.extract %69 from 25 : (i33) -> i1
    %119 = comb.extract %69 from 27 : (i33) -> i1
    %120 = comb.extract %69 from 29 : (i33) -> i1
    %121 = comb.concat %94, %105, %95, %106, %96, %108, %110, %111, %113, %114, %100, %117, %118, %102, %119, %103, %120, %104, %90 : i1, i1, i1, i1, i1, i4, i8, i1, i1, i1, i1, i4, i1, i1, i1, i1, i1, i1, i1
    %122 = comb.icmp bin eq %io_alu_op, %c0_i4 {sv.namehint = "_out_T"} : i4
    %123 = comb.icmp bin eq %io_alu_op, %c1_i4 {sv.namehint = "_out_T_1"} : i4
    %124 = comb.or bin %122, %123 {sv.namehint = "_out_T_2"} : i1
    %125 = comb.icmp bin eq %io_alu_op, %c5_i4 {sv.namehint = "_out_T_3"} : i4
    %126 = comb.icmp bin eq %io_alu_op, %c7_i4 {sv.namehint = "_out_T_4"} : i4
    %127 = comb.or bin %125, %126 {sv.namehint = "_out_T_5"} : i1
    %128 = comb.icmp bin eq %io_alu_op, %c-7_i4 {sv.namehint = "_out_T_6"} : i4
    %129 = comb.icmp bin eq %io_alu_op, %c-8_i4 {sv.namehint = "_out_T_7"} : i4
    %130 = comb.or bin %128, %129 {sv.namehint = "_out_T_8"} : i1
    %131 = comb.icmp bin eq %io_alu_op, %c6_i4 {sv.namehint = "_out_T_9"} : i4
    %132 = comb.icmp bin eq %io_alu_op, %c2_i4 {sv.namehint = "_out_T_10"} : i4
    %133 = comb.and bin %io_A, %io_B {sv.namehint = "_out_T_11"} : i32
    %134 = comb.icmp bin eq %io_alu_op, %c3_i4 {sv.namehint = "_out_T_12"} : i4
    %135 = comb.or bin %io_A, %io_B {sv.namehint = "_out_T_13"} : i32
    %136 = comb.icmp bin eq %io_alu_op, %c4_i4 {sv.namehint = "_out_T_14"} : i4
    %137 = comb.xor bin %io_A, %io_B {sv.namehint = "_out_T_15"} : i32
    %138 = comb.icmp bin eq %io_alu_op, %c-6_i4 {sv.namehint = "_out_T_16"} : i4
    %139 = comb.mux bin %138, %io_A, %io_B {sv.namehint = "_out_T_17"} : i32
    %140 = comb.mux bin %136, %137, %139 {sv.namehint = "_out_T_18"} : i32
    %141 = comb.mux bin %134, %135, %140 {sv.namehint = "_out_T_19"} : i32
    %142 = comb.mux bin %132, %133, %141 {sv.namehint = "_out_T_20"} : i32
    %143 = comb.mux bin %131, %121, %142 {sv.namehint = "_out_T_21"} : i32
    %144 = comb.mux bin %130, %70, %143 {sv.namehint = "_out_T_22"} : i32
    %145 = comb.concat %c0_i31, %10 : i31, i1
    %146 = comb.mux bin %127, %145, %144 {sv.namehint = "_out_T_23"} : i32
    %147 = comb.mux bin %124, %3, %146 {sv.namehint = "out"} : i32
    hw.output %147, %3 : i32, i32
  }
  hw.module private @ImmGenWire(in %io_inst : i32, in %io_sel : i3, out io_out : i32) {
    %c0_i27 = hw.constant 0 : i27
    %false = hw.constant false
    %c0_i12 = hw.constant 0 : i12
    %c1_i3 = hw.constant 1 : i3
    %c2_i3 = hw.constant 2 : i3
    %c-3_i3 = hw.constant -3 : i3
    %c3_i3 = hw.constant 3 : i3
    %c-4_i3 = hw.constant -4 : i3
    %c-2_i3 = hw.constant -2 : i3
    %0 = comb.extract %io_inst from 20 {sv.namehint = "_Iimm_T"} : (i32) -> i12
    %1 = comb.extract %io_inst from 25 {sv.namehint = "_Simm_T"} : (i32) -> i7
    %2 = comb.extract %io_inst from 7 {sv.namehint = "_Simm_T_1"} : (i32) -> i5
    %3 = comb.concat %1, %2 {sv.namehint = "_Simm_T_2"} : i7, i5
    %4 = comb.extract %io_inst from 31 {sv.namehint = "_Jimm_T"} : (i32) -> i1
    %5 = comb.extract %io_inst from 7 {sv.namehint = "_Bimm_T_1"} : (i32) -> i1
    %6 = comb.extract %io_inst from 25 {sv.namehint = "_Jimm_T_3"} : (i32) -> i6
    %7 = comb.extract %io_inst from 8 {sv.namehint = "_Bimm_T_3"} : (i32) -> i4
    %8 = comb.concat %4, %5, %6, %7, %false {sv.namehint = "_Bimm_T_4"} : i1, i1, i6, i4, i1
    %9 = comb.extract %io_inst from 12 {sv.namehint = "_Uimm_T"} : (i32) -> i20
    %10 = comb.concat %9, %c0_i12 {sv.namehint = "_Uimm_T_1"} : i20, i12
    %11 = comb.extract %io_inst from 12 {sv.namehint = "_Jimm_T_1"} : (i32) -> i8
    %12 = comb.extract %io_inst from 20 {sv.namehint = "_Jimm_T_2"} : (i32) -> i1
    %13 = comb.extract %io_inst from 21 {sv.namehint = "Jimm_lo_hi"} : (i32) -> i10
    %14 = comb.extract %io_inst from 15 {sv.namehint = "_Zimm_T"} : (i32) -> i5
    %15 = comb.extract %io_inst from 21 : (i32) -> i11
    %16 = comb.concat %15, %false {sv.namehint = "_io_out_T"} : i11, i1
    %17 = comb.icmp bin eq %io_sel, %c1_i3 {sv.namehint = "_io_out_T_2"} : i3
    %18 = comb.mux bin %17, %0, %16 {sv.namehint = "_io_out_T_3"} : i12
    %19 = comb.icmp bin eq %io_sel, %c2_i3 {sv.namehint = "_io_out_T_4"} : i3
    %20 = comb.mux bin %19, %3, %18 {sv.namehint = "_io_out_T_5"} : i12
    %21 = comb.icmp bin eq %io_sel, %c-3_i3 {sv.namehint = "_io_out_T_6"} : i3
    %22 = comb.extract %20 from 11 : (i12) -> i1
    %23 = comb.concat %22, %20 : i1, i12
    %24 = comb.mux bin %21, %8, %23 {sv.namehint = "_io_out_T_7"} : i13
    %25 = comb.icmp bin eq %io_sel, %c3_i3 {sv.namehint = "_io_out_T_8"} : i3
    %26 = comb.extract %24 from 12 : (i13) -> i1
    %27 = comb.replicate %26 : (i1) -> i19
    %28 = comb.concat %27, %24 : i19, i13
    %29 = comb.mux bin %25, %10, %28 {sv.namehint = "_io_out_T_9"} : i32
    %30 = comb.icmp bin eq %io_sel, %c-4_i3 {sv.namehint = "_io_out_T_10"} : i3
    %31 = comb.replicate %4 : (i1) -> i12
    %32 = comb.concat %31, %11, %12, %13, %false : i12, i8, i1, i10, i1
    %33 = comb.mux bin %30, %32, %29 {sv.namehint = "_io_out_T_11"} : i32
    %34 = comb.icmp bin eq %io_sel, %c-2_i3 {sv.namehint = "_io_out_T_12"} : i3
    %35 = comb.concat %c0_i27, %14 : i27, i5
    %36 = comb.mux bin %34, %35, %33 {sv.namehint = "io_out"} : i32
    hw.output %36 : i32
  }
  hw.module private @BrCondArea(in %io_rs1 : i32, in %io_rs2 : i32, in %io_br_type : i3, out io_taken : i1) {
    %true = hw.constant true
    %c0_i32 = hw.constant 0 : i32
    %c3_i3 = hw.constant 3 : i3
    %c-2_i3 = hw.constant -2 : i3
    %c2_i3 = hw.constant 2 : i3
    %c-3_i3 = hw.constant -3 : i3
    %c1_i3 = hw.constant 1 : i3
    %c-4_i3 = hw.constant -4 : i3
    %0 = comb.sub %io_rs1, %io_rs2 {sv.namehint = "_diff_T"} : i32
    %1 = comb.icmp bin ne %0, %c0_i32 {sv.namehint = "neq"} : i32
    %2 = comb.xor bin %1, %true {sv.namehint = "eq"} : i1
    %3 = comb.extract %io_rs1 from 31 {sv.namehint = "_lt_T_1"} : (i32) -> i1
    %4 = comb.extract %io_rs2 from 31 {sv.namehint = "_ltu_T_1"} : (i32) -> i1
    %5 = comb.icmp bin eq %3, %4 {sv.namehint = "isSameSign"} : i1
    %6 = comb.extract %0 from 31 {sv.namehint = "_ltu_T"} : (i32) -> i1
    %7 = comb.mux bin %5, %6, %3 {sv.namehint = "lt"} : i1
    %8 = comb.mux bin %5, %6, %4 {sv.namehint = "ltu"} : i1
    %9 = comb.xor bin %7, %true {sv.namehint = "ge"} : i1
    %10 = comb.xor bin %8, %true {sv.namehint = "geu"} : i1
    %11 = comb.icmp bin eq %io_br_type, %c3_i3 {sv.namehint = "_io_taken_T"} : i3
    %12 = comb.and bin %11, %2 {sv.namehint = "_io_taken_T_1"} : i1
    %13 = comb.icmp bin eq %io_br_type, %c-2_i3 {sv.namehint = "_io_taken_T_2"} : i3
    %14 = comb.and bin %13, %1 {sv.namehint = "_io_taken_T_3"} : i1
    %15 = comb.icmp bin eq %io_br_type, %c2_i3 {sv.namehint = "_io_taken_T_5"} : i3
    %16 = comb.and bin %15, %7 {sv.namehint = "_io_taken_T_6"} : i1
    %17 = comb.icmp bin eq %io_br_type, %c-3_i3 {sv.namehint = "_io_taken_T_8"} : i3
    %18 = comb.and bin %17, %9 {sv.namehint = "_io_taken_T_9"} : i1
    %19 = comb.icmp bin eq %io_br_type, %c1_i3 {sv.namehint = "_io_taken_T_11"} : i3
    %20 = comb.and bin %19, %8 {sv.namehint = "_io_taken_T_12"} : i1
    %21 = comb.icmp bin eq %io_br_type, %c-4_i3 {sv.namehint = "_io_taken_T_14"} : i3
    %22 = comb.and bin %21, %10 {sv.namehint = "_io_taken_T_15"} : i1
    %23 = comb.or bin %12, %14, %16, %18, %20, %22 {sv.namehint = "io_taken"} : i1
    hw.output %23 : i1
  }
  hw.module private @Datapath(in %clock : !seq.clock, in %reset : i1, in %io_host_fromhost_valid : i1, in %io_host_fromhost_bits : i32, out io_host_tohost : i32, out io_icache_req_valid : i1, out io_icache_req_bits_addr : i32, in %io_icache_resp_valid : i1, in %io_icache_resp_bits_data : i32, out io_dcache_abort : i1, out io_dcache_req_valid : i1, out io_dcache_req_bits_addr : i32, out io_dcache_req_bits_data : i32, out io_dcache_req_bits_mask : i4, in %io_dcache_resp_valid : i1, in %io_dcache_resp_bits_data : i32, out io_ctrl_inst : i32, in %io_ctrl_pc_sel : i2, in %io_ctrl_inst_kill : i1, in %io_ctrl_A_sel : i1, in %io_ctrl_B_sel : i1, in %io_ctrl_imm_sel : i3, in %io_ctrl_alu_op : i4, in %io_ctrl_br_type : i3, in %io_ctrl_st_type : i2, in %io_ctrl_ld_type : i3, in %io_ctrl_wb_sel : i2, in %io_ctrl_wb_en : i1, in %io_ctrl_csr_cmd : i3, in %io_ctrl_illegal : i1) {
    %false = hw.constant false
    %c4_i32 = hw.constant 4 : i32
    %c0_i25 = hw.constant 0 : i25
    %c0_i17 = hw.constant 0 : i17
    %c0_i27 = hw.constant 0 : i27
    %true = hw.constant true
    %c0_i282 = hw.constant 0 : i282
    %c0_i3 = hw.constant 0 : i3
    %c0_i2 = hw.constant 0 : i2
    %c4_i33 = hw.constant 4 : i33
    %c1_i4 = hw.constant 1 : i4
    %c3_i5 = hw.constant 3 : i5
    %c508_i33 = hw.constant 508 : i33
    %c0_i255 = hw.constant 0 : i255
    %c0_i5 = hw.constant 0 : i5
    %c-4_i3 = hw.constant -4 : i3
    %c-1_i2 = hw.constant -1 : i2
    %c1_i2 = hw.constant 1 : i2
    %c-2_i2 = hw.constant -2 : i2
    %c-2_i3 = hw.constant -2 : i3
    %c2_i3 = hw.constant 2 : i3
    %c3_i3 = hw.constant 3 : i3
    %c-3_i3 = hw.constant -3 : i3
    %c19_i32 = hw.constant 19 : i32
    %c0_i32 = hw.constant 0 : i32
    %csr.io_out, %csr.io_expt, %csr.io_evec, %csr.io_epc, %csr.io_host_tohost = hw.instance "csr" @CSR(clock: %clock: !seq.clock, reset: %reset: i1, io_stall: %2: i1, io_cmd: %csr_cmd: i3, io_in: %ew_reg_csr_in: i32, io_pc: %ew_reg_pc: i32, io_addr: %ew_reg_alu: i32, io_inst: %ew_reg_inst: i32, io_illegal: %illegal: i1, io_st_type: %st_type: i2, io_ld_type: %ld_type: i3, io_pc_check: %pc_check: i1, io_host_fromhost_valid: %io_host_fromhost_valid: i1, io_host_fromhost_bits: %io_host_fromhost_bits: i32) -> (io_out: i32, io_expt: i1, io_evec: i32, io_epc: i32, io_host_tohost: i32) {sv.namehint = "io_host_tohost"}
    %regFile.io_rdata1, %regFile.io_rdata2 = hw.instance "regFile" @RegFile(clock: %clock: !seq.clock, io_raddr1: %24: i5, io_raddr2: %25: i5, io_wen: %129: i1, io_waddr: %26: i5, io_wdata: %130: i32) -> (io_rdata1: i32, io_rdata2: i32)
    %alu.io_out, %alu.io_sum = hw.instance "alu" @AluArea(io_A: %36: i32, io_B: %37: i32, io_alu_op: %io_ctrl_alu_op: i4) -> (io_out: i32, io_sum: i32)
    %immGen.io_out = hw.instance "immGen" @ImmGenWire(io_inst: %fe_reg_inst: i32, io_sel: %io_ctrl_imm_sel: i3) -> (io_out: i32)
    %brCond.io_taken = hw.instance "brCond" @BrCondArea(io_rs1: %33: i32, io_rs2: %35: i32, io_br_type: %io_ctrl_br_type: i3) -> (io_taken: i1)
    %fe_reg_inst = seq.firreg %23 clock %clock reset sync %reset, %c19_i32 {firrtl.random_init_start = 0 : ui64, sv.namehint = "fe_reg_inst"} : i32
    %fe_reg_pc = seq.firreg %22 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 32 : ui64} : i32
    %ew_reg_inst = seq.firreg %73 clock %clock reset sync %reset, %c19_i32 {firrtl.random_init_start = 64 : ui64, sv.namehint = "ew_reg_inst"} : i32
    %ew_reg_pc = seq.firreg %70 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 96 : ui64, sv.namehint = "ew_reg_pc"} : i32
    %ew_reg_alu = seq.firreg %76 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 128 : ui64, sv.namehint = "ew_reg_alu"} : i32
    %ew_reg_csr_in = seq.firreg %81 clock %clock reset sync %reset, %c0_i32 {firrtl.random_init_start = 160 : ui64, sv.namehint = "ew_reg_csr_in"} : i32
    %st_type = seq.firreg %83 clock %clock {firrtl.random_init_start = 192 : ui64, sv.namehint = "st_type"} : i2
    %ld_type = seq.firreg %85 clock %clock {firrtl.random_init_start = 194 : ui64, sv.namehint = "ld_type"} : i3
    %wb_sel = seq.firreg %88 clock %clock {firrtl.random_init_start = 197 : ui64} : i2
    %wb_en = seq.firreg %91 clock %clock {firrtl.random_init_start = 199 : ui64} : i1
    %csr_cmd = seq.firreg %93 clock %clock {firrtl.random_init_start = 200 : ui64, sv.namehint = "csr_cmd"} : i3
    %illegal = seq.firreg %96 clock %clock {firrtl.random_init_start = 203 : ui64, sv.namehint = "illegal"} : i1
    %pc_check = seq.firreg %99 clock %clock {firrtl.random_init_start = 204 : ui64, sv.namehint = "pc_check"} : i1
    %started = seq.firreg %reset clock %clock {firrtl.random_init_start = 205 : ui64} : i1
    %0 = comb.xor bin %io_icache_resp_valid, %true {sv.namehint = "_stall_T"} : i1
    %1 = comb.xor bin %io_dcache_resp_valid, %true {sv.namehint = "_stall_T_1"} : i1
    %2 = comb.or bin %0, %1 {sv.namehint = "stall"} : i1
    %pc = seq.firreg %16 clock %clock reset sync %reset, %c508_i33 {firrtl.random_init_start = 206 : ui64} : i33
    %3 = comb.add %pc, %c4_i33 {sv.namehint = "_next_pc_T"} : i33
    %4 = comb.icmp bin eq %io_ctrl_pc_sel, %c-1_i2 {sv.namehint = "_next_pc_T_2"} : i2
    %5 = comb.icmp bin eq %io_ctrl_pc_sel, %c1_i2 {sv.namehint = "_pc_check_T"} : i2
    %6 = comb.or bin %5, %brCond.io_taken {sv.namehint = "_next_pc_T_4"} : i1
    %7 = comb.extract %alu.io_sum from 1 : (i32) -> i31
    %8 = comb.concat %false, %7, %false {sv.namehint = "_next_pc_T_6"} : i1, i31, i1
    %9 = comb.icmp bin eq %io_ctrl_pc_sel, %c-2_i2 {sv.namehint = "_next_pc_T_7"} : i2
    %10 = comb.mux bin %9, %pc, %3 {sv.namehint = "_next_pc_T_8"} : i33
    %11 = comb.mux bin %6, %8, %10 {sv.namehint = "_next_pc_T_9"} : i33
    %12 = comb.concat %false, %csr.io_epc : i1, i32
    %13 = comb.mux bin %4, %12, %11 {sv.namehint = "_next_pc_T_10"} : i33
    %14 = comb.concat %false, %csr.io_evec : i1, i32
    %15 = comb.mux bin %csr.io_expt, %14, %13 {sv.namehint = "_next_pc_T_11"} : i33
    %16 = comb.mux bin %2, %pc, %15 {sv.namehint = "next_pc"} : i33
    %17 = comb.or bin %started, %io_ctrl_inst_kill, %brCond.io_taken, %csr.io_expt {sv.namehint = "_inst_T_2"} : i1
    %18 = comb.mux bin %17, %c19_i32, %io_icache_resp_bits_data {sv.namehint = "inst"} : i32
    %19 = comb.extract %16 from 0 {sv.namehint = "io_icache_req_bits_addr"} : (i33) -> i32
    %20 = comb.xor bin %2, %true {sv.namehint = "io_icache_req_valid"} : i1
    %21 = comb.extract %pc from 0 : (i33) -> i32
    %22 = comb.mux bin %2, %fe_reg_pc, %21 : i32
    %23 = comb.mux bin %2, %fe_reg_inst, %18 : i32
    %24 = comb.extract %fe_reg_inst from 15 {sv.namehint = "rs1_addr"} : (i32) -> i5
    %25 = comb.extract %fe_reg_inst from 20 {sv.namehint = "rs2_addr"} : (i32) -> i5
    %26 = comb.extract %ew_reg_inst from 7 {sv.namehint = "wb_rd_addr"} : (i32) -> i5
    %27 = comb.icmp bin ne %24, %c0_i5 {sv.namehint = "_rs1hazard_T"} : i5
    %28 = comb.icmp bin eq %24, %26 {sv.namehint = "_rs1hazard_T_2"} : i5
    %29 = comb.icmp bin ne %25, %c0_i5 {sv.namehint = "_rs2hazard_T"} : i5
    %30 = comb.icmp bin eq %25, %26 {sv.namehint = "_rs2hazard_T_2"} : i5
    %31 = comb.icmp bin eq %wb_sel, %c0_i2 {sv.namehint = "_rs2_T"} : i2
    %32 = comb.and bin %31, %wb_en, %27, %28 {sv.namehint = "_rs1_T_1"} : i1
    %33 = comb.mux bin %32, %ew_reg_alu, %regFile.io_rdata1 {sv.namehint = "rs1"} : i32
    %34 = comb.and bin %31, %wb_en, %29, %30 {sv.namehint = "_rs2_T_1"} : i1
    %35 = comb.mux bin %34, %ew_reg_alu, %regFile.io_rdata2 {sv.namehint = "rs2"} : i32
    %36 = comb.mux bin %io_ctrl_A_sel, %33, %fe_reg_pc {sv.namehint = "_alu_io_A_T_1"} : i32
    %37 = comb.mux bin %io_ctrl_B_sel, %35, %immGen.io_out {sv.namehint = "_alu_io_B_T_1"} : i32
    %38 = comb.extract %ew_reg_alu from 2 : (i32) -> i30
    %39 = comb.extract %alu.io_sum from 2 : (i32) -> i30
    %40 = comb.mux %2, %38, %39 {sv.namehint = "_daddr_T"} : i30
    %41 = comb.extract %alu.io_sum from 0 : (i32) -> i2
    %42 = comb.icmp bin ne %io_ctrl_st_type, %c0_i2 {sv.namehint = "_io_dcache_req_valid_T_1"} : i2
    %43 = comb.icmp bin ne %io_ctrl_ld_type, %c0_i3 {sv.namehint = "_io_dcache_req_valid_T_2"} : i3
    %44 = comb.or bin %42, %43 {sv.namehint = "_io_dcache_req_valid_T_3"} : i1
    %45 = comb.and bin %20, %44 {sv.namehint = "io_dcache_req_valid"} : i1
    %46 = comb.concat %40, %c0_i2 {sv.namehint = "io_dcache_req_bits_addr"} : i30, i2
    %47 = comb.concat %c0_i255, %35 : i255, i32
    %48 = comb.concat %c0_i282, %41, %c0_i3 : i282, i2, i3
    %49 = comb.shl bin %47, %48 {sv.namehint = "_io_dcache_req_bits_data_T"} : i287
    %50 = comb.extract %49 from 0 {sv.namehint = "io_dcache_req_bits_data"} : (i287) -> i32
    %51 = comb.mux bin %2, %st_type, %io_ctrl_st_type {sv.namehint = "_io_dcache_req_bits_mask_T"} : i2
    %52 = comb.extract %alu.io_sum from 0 {sv.namehint = "_io_dcache_req_bits_mask_T_3"} : (i32) -> i2
    %53 = comb.concat %c0_i3, %52 : i3, i2
    %54 = comb.shl bin %c3_i5, %53 {sv.namehint = "_io_dcache_req_bits_mask_T_2"} : i5
    %55 = comb.concat %c0_i2, %52 : i2, i2
    %56 = comb.shl bin %c1_i4, %55 {sv.namehint = "_io_dcache_req_bits_mask_T_4"} : i4
    %57 = comb.icmp bin eq %51, %c1_i2 {sv.namehint = "_io_dcache_req_bits_mask_T_5"} : i2
    %58 = comb.replicate %57 {sv.namehint = "_io_dcache_req_bits_mask_T_6"} : (i1) -> i4
    %59 = comb.icmp bin eq %51, %c-2_i2 {sv.namehint = "_io_dcache_req_bits_mask_T_7"} : i2
    %60 = comb.extract %54 from 0 : (i5) -> i4
    %61 = comb.mux %59, %60, %58 {sv.namehint = "_io_dcache_req_bits_mask_T_8"} : i4
    %62 = comb.icmp bin eq %51, %c-1_i2 {sv.namehint = "_io_dcache_req_bits_mask_T_9"} : i2
    %63 = comb.mux %62, %56, %61 {sv.namehint = "_io_dcache_req_bits_mask_T_10"} : i4
    %64 = comb.and bin %20, %csr.io_expt : i1
    %65 = comb.or bin %reset, %64 : i1
    %66 = comb.xor bin %csr.io_expt, %true : i1
    %67 = comb.and bin %20, %66 : i1
    %68 = comb.xor %67, %true : i1
    %69 = comb.or %65, %68 : i1
    %70 = comb.mux bin %69, %ew_reg_pc, %fe_reg_pc : i32
    %71 = comb.xor %67, %true : i1
    %72 = comb.or %65, %71 : i1
    %73 = comb.mux bin %72, %ew_reg_inst, %fe_reg_inst : i32
    %74 = comb.xor %67, %true : i1
    %75 = comb.or %65, %74 : i1
    %76 = comb.mux bin %75, %ew_reg_alu, %alu.io_out : i32
    %77 = comb.icmp bin eq %io_ctrl_imm_sel, %c-2_i3 {sv.namehint = "_ew_reg_csr_in_T"} : i3
    %78 = comb.mux bin %77, %immGen.io_out, %33 {sv.namehint = "_ew_reg_csr_in_T_1"} : i32
    %79 = comb.xor %67, %true : i1
    %80 = comb.or %65, %79 : i1
    %81 = comb.mux bin %80, %ew_reg_csr_in, %78 : i32
    %82 = comb.mux bin %67, %io_ctrl_st_type, %st_type : i2
    %83 = comb.mux bin %65, %c0_i2, %82 : i2
    %84 = comb.mux bin %67, %io_ctrl_ld_type, %ld_type : i3
    %85 = comb.mux bin %65, %c0_i3, %84 : i3
    %86 = comb.xor %67, %true : i1
    %87 = comb.or %65, %86 : i1
    %88 = comb.mux bin %87, %wb_sel, %io_ctrl_wb_sel : i2
    %89 = comb.mux bin %67, %io_ctrl_wb_en, %wb_en : i1
    %90 = comb.xor %65, %true : i1
    %91 = comb.and %90, %89 : i1
    %92 = comb.mux bin %67, %io_ctrl_csr_cmd, %csr_cmd : i3
    %93 = comb.mux bin %65, %c0_i3, %92 : i3
    %94 = comb.mux bin %67, %io_ctrl_illegal, %illegal : i1
    %95 = comb.xor %65, %true : i1
    %96 = comb.and %95, %94 : i1
    %97 = comb.mux bin %67, %5, %pc_check : i1
    %98 = comb.xor %65, %true : i1
    %99 = comb.and %98, %97 : i1
    %100 = comb.extract %ew_reg_alu from 0 : (i32) -> i2
    %101 = comb.concat %c0_i27, %100, %c0_i3 : i27, i2, i3
    %102 = comb.shru bin %io_dcache_resp_bits_data, %101 {sv.namehint = "lshift"} : i32
    %103 = comb.concat %false, %io_dcache_resp_bits_data : i1, i32
    %104 = comb.extract %102 from 0 {sv.namehint = "_load_T_5"} : (i32) -> i16
    %105 = comb.extract %102 from 0 {sv.namehint = "_load_T_7"} : (i32) -> i8
    %106 = comb.icmp bin eq %ld_type, %c2_i3 {sv.namehint = "_load_T_9"} : i3
    %107 = comb.extract %102 from 15 : (i32) -> i1
    %108 = comb.replicate %107 : (i1) -> i17
    %109 = comb.concat %108, %104 : i17, i16
    %110 = comb.mux bin %106, %109, %103 {sv.namehint = "_load_T_10"} : i33
    %111 = comb.icmp bin eq %ld_type, %c3_i3 {sv.namehint = "_load_T_11"} : i3
    %112 = comb.extract %102 from 7 : (i32) -> i1
    %113 = comb.replicate %112 : (i1) -> i25
    %114 = comb.concat %113, %105 : i25, i8
    %115 = comb.mux bin %111, %114, %110 {sv.namehint = "_load_T_12"} : i33
    %116 = comb.icmp bin eq %ld_type, %c-4_i3 {sv.namehint = "_load_T_13"} : i3
    %117 = comb.concat %c0_i17, %104 : i17, i16
    %118 = comb.mux bin %116, %117, %115 {sv.namehint = "_load_T_14"} : i33
    %119 = comb.icmp bin eq %ld_type, %c-3_i3 {sv.namehint = "_load_T_15"} : i3
    %120 = comb.concat %c0_i25, %105 : i25, i8
    %121 = comb.mux bin %119, %120, %118 {sv.namehint = "load"} : i33
    %122 = comb.concat %false, %ew_reg_alu : i1, i32
    %123 = comb.add %ew_reg_pc, %c4_i32 {sv.namehint = "_regWrite_T_1"} : i32
    %124 = comb.concat %false, %123 : i1, i32
    %125 = comb.concat %false, %csr.io_out : i1, i32
    %126 = hw.array_create %125, %124, %121, %122 : i33
    %127 = hw.array_get %126[%wb_sel] {sv.namehint = "_regWrite_T_10"} : !hw.array<4xi33>, i2
    %128 = comb.xor bin %csr.io_expt, %true {sv.namehint = "_regFile_io_wen_T_2"} : i1
    %129 = comb.and bin %wb_en, %20, %128 {sv.namehint = "_regFile_io_wen_T_3"} : i1
    %130 = comb.extract %127 from 0 : (i33) -> i32
    hw.output %csr.io_host_tohost, %20, %19, %csr.io_expt, %45, %46, %50, %63, %fe_reg_inst : i32, i1, i32, i1, i1, i32, i32, i4, i32
  }
  hw.module private @Control(in %io_inst : i32, out io_pc_sel : i2, out io_inst_kill : i1, out io_A_sel : i1, out io_B_sel : i1, out io_imm_sel : i3, out io_alu_op : i4, out io_br_type : i3, out io_st_type : i2, out io_ld_type : i3, out io_wb_sel : i2, out io_wb_en : i1, out io_csr_cmd : i3, out io_illegal : i1) {
    %true = hw.constant true
    %c0_i2 = hw.constant 0 : i2
    %false = hw.constant false
    %c-13_i10 = hw.constant -13 : i10
    %c-141_i10 = hw.constant -141 : i10
    %c-269_i10 = hw.constant -269 : i10
    %c499_i10 = hw.constant 499 : i10
    %c371_i10 = hw.constant 371 : i10
    %c243_i10 = hw.constant 243 : i10
    %c15_i24 = hw.constant 15 : i24
    %c947_i17 = hw.constant 947 : i17
    %c819_i17 = hw.constant 819 : i17
    %c33459_i17 = hw.constant 33459 : i17
    %c691_i17 = hw.constant 691 : i17
    %c563_i17 = hw.constant 563 : i17
    %c435_i17 = hw.constant 435 : i17
    %c307_i17 = hw.constant 307 : i17
    %c179_i17 = hw.constant 179 : i17
    %c32819_i17 = hw.constant 32819 : i17
    %c51_i17 = hw.constant 51 : i17
    %c33427_i17 = hw.constant 33427 : i17
    %c659_i17 = hw.constant 659 : i17
    %c147_i17 = hw.constant 147 : i17
    %c-109_i10 = hw.constant -109 : i10
    %c-237_i10 = hw.constant -237 : i10
    %c-493_i10 = hw.constant -493 : i10
    %c403_i10 = hw.constant 403 : i10
    %c275_i10 = hw.constant 275 : i10
    %c19_i10 = hw.constant 19 : i10
    %c291_i10 = hw.constant 291 : i10
    %c163_i10 = hw.constant 163 : i10
    %c35_i10 = hw.constant 35 : i10
    %c-381_i10 = hw.constant -381 : i10
    %c-509_i10 = hw.constant -509 : i10
    %c259_i10 = hw.constant 259 : i10
    %c131_i10 = hw.constant 131 : i10
    %c3_i10 = hw.constant 3 : i10
    %c-29_i10 = hw.constant -29 : i10
    %c-157_i10 = hw.constant -157 : i10
    %c-285_i10 = hw.constant -285 : i10
    %c-413_i10 = hw.constant -413 : i10
    %c227_i10 = hw.constant 227 : i10
    %c99_i10 = hw.constant 99 : i10
    %c103_i10 = hw.constant 103 : i10
    %c-17_i7 = hw.constant -17 : i7
    %c23_i7 = hw.constant 23 : i7
    %c55_i7 = hw.constant 55 : i7
    %c270532723_i32 = hw.constant 270532723 : i32
    %c268435571_i32 = hw.constant 268435571 : i32
    %c1048691_i32 = hw.constant 1048691 : i32
    %c115_i32 = hw.constant 115 : i32
    %c4111_i32 = hw.constant 4111 : i32
    %c-1_i2 = hw.constant -1 : i2
    %c-2_i2 = hw.constant -2 : i2
    %c1_i2 = hw.constant 1 : i2
    %c-2_i3 = hw.constant -2 : i3
    %c1_i3 = hw.constant 1 : i3
    %c2_i3 = hw.constant 2 : i3
    %c-3_i3 = hw.constant -3 : i3
    %c-4_i3 = hw.constant -4 : i3
    %c3_i3 = hw.constant 3 : i3
    %c-6_i4 = hw.constant -6 : i4
    %c2_i4 = hw.constant 2 : i4
    %c3_i4 = hw.constant 3 : i4
    %c-7_i4 = hw.constant -7 : i4
    %c-8_i4 = hw.constant -8 : i4
    %c4_i4 = hw.constant 4 : i4
    %c7_i4 = hw.constant 7 : i4
    %c5_i4 = hw.constant 5 : i4
    %c6_i4 = hw.constant 6 : i4
    %c1_i4 = hw.constant 1 : i4
    %c0_i4 = hw.constant 0 : i4
    %c-5_i4 = hw.constant -5 : i4
    %c-1_i4 = hw.constant -1 : i4
    %c0_i3 = hw.constant 0 : i3
    %0 = comb.extract %io_inst from 0 : (i32) -> i7
    %1 = comb.icmp bin eq %0, %c55_i7 {sv.namehint = "_ctrlSignals_T_1"} : i7
    %2 = comb.icmp bin eq %0, %c23_i7 {sv.namehint = "_ctrlSignals_T_3"} : i7
    %3 = comb.icmp bin eq %0, %c-17_i7 {sv.namehint = "_ctrlSignals_T_5"} : i7
    %4 = comb.extract %io_inst from 12 : (i32) -> i3
    %5 = comb.extract %io_inst from 0 : (i32) -> i7
    %6 = comb.concat %4, %5 : i3, i7
    %7 = comb.icmp bin eq %6, %c103_i10 {sv.namehint = "_ctrlSignals_T_7"} : i10
    %8 = comb.extract %io_inst from 12 : (i32) -> i3
    %9 = comb.extract %io_inst from 0 : (i32) -> i7
    %10 = comb.concat %8, %9 : i3, i7
    %11 = comb.icmp bin eq %10, %c99_i10 {sv.namehint = "_ctrlSignals_T_9"} : i10
    %12 = comb.extract %io_inst from 12 : (i32) -> i3
    %13 = comb.extract %io_inst from 0 : (i32) -> i7
    %14 = comb.concat %12, %13 : i3, i7
    %15 = comb.icmp bin eq %14, %c227_i10 {sv.namehint = "_ctrlSignals_T_11"} : i10
    %16 = comb.extract %io_inst from 12 : (i32) -> i3
    %17 = comb.extract %io_inst from 0 : (i32) -> i7
    %18 = comb.concat %16, %17 : i3, i7
    %19 = comb.icmp bin eq %18, %c-413_i10 {sv.namehint = "_ctrlSignals_T_13"} : i10
    %20 = comb.extract %io_inst from 12 : (i32) -> i3
    %21 = comb.extract %io_inst from 0 : (i32) -> i7
    %22 = comb.concat %20, %21 : i3, i7
    %23 = comb.icmp bin eq %22, %c-285_i10 {sv.namehint = "_ctrlSignals_T_15"} : i10
    %24 = comb.extract %io_inst from 12 : (i32) -> i3
    %25 = comb.extract %io_inst from 0 : (i32) -> i7
    %26 = comb.concat %24, %25 : i3, i7
    %27 = comb.icmp bin eq %26, %c-157_i10 {sv.namehint = "_ctrlSignals_T_17"} : i10
    %28 = comb.extract %io_inst from 12 : (i32) -> i3
    %29 = comb.extract %io_inst from 0 : (i32) -> i7
    %30 = comb.concat %28, %29 : i3, i7
    %31 = comb.icmp bin eq %30, %c-29_i10 {sv.namehint = "_ctrlSignals_T_19"} : i10
    %32 = comb.extract %io_inst from 12 : (i32) -> i3
    %33 = comb.extract %io_inst from 0 : (i32) -> i7
    %34 = comb.concat %32, %33 : i3, i7
    %35 = comb.icmp bin eq %34, %c3_i10 {sv.namehint = "_ctrlSignals_T_21"} : i10
    %36 = comb.extract %io_inst from 12 : (i32) -> i3
    %37 = comb.extract %io_inst from 0 : (i32) -> i7
    %38 = comb.concat %36, %37 : i3, i7
    %39 = comb.icmp bin eq %38, %c131_i10 {sv.namehint = "_ctrlSignals_T_23"} : i10
    %40 = comb.extract %io_inst from 12 : (i32) -> i3
    %41 = comb.extract %io_inst from 0 : (i32) -> i7
    %42 = comb.concat %40, %41 : i3, i7
    %43 = comb.icmp bin eq %42, %c259_i10 {sv.namehint = "_ctrlSignals_T_25"} : i10
    %44 = comb.extract %io_inst from 12 : (i32) -> i3
    %45 = comb.extract %io_inst from 0 : (i32) -> i7
    %46 = comb.concat %44, %45 : i3, i7
    %47 = comb.icmp bin eq %46, %c-509_i10 {sv.namehint = "_ctrlSignals_T_27"} : i10
    %48 = comb.extract %io_inst from 12 : (i32) -> i3
    %49 = comb.extract %io_inst from 0 : (i32) -> i7
    %50 = comb.concat %48, %49 : i3, i7
    %51 = comb.icmp bin eq %50, %c-381_i10 {sv.namehint = "_ctrlSignals_T_29"} : i10
    %52 = comb.extract %io_inst from 12 : (i32) -> i3
    %53 = comb.extract %io_inst from 0 : (i32) -> i7
    %54 = comb.concat %52, %53 : i3, i7
    %55 = comb.icmp bin eq %54, %c35_i10 {sv.namehint = "_ctrlSignals_T_31"} : i10
    %56 = comb.extract %io_inst from 12 : (i32) -> i3
    %57 = comb.extract %io_inst from 0 : (i32) -> i7
    %58 = comb.concat %56, %57 : i3, i7
    %59 = comb.icmp bin eq %58, %c163_i10 {sv.namehint = "_ctrlSignals_T_33"} : i10
    %60 = comb.extract %io_inst from 12 : (i32) -> i3
    %61 = comb.extract %io_inst from 0 : (i32) -> i7
    %62 = comb.concat %60, %61 : i3, i7
    %63 = comb.icmp bin eq %62, %c291_i10 {sv.namehint = "_ctrlSignals_T_35"} : i10
    %64 = comb.extract %io_inst from 12 : (i32) -> i3
    %65 = comb.extract %io_inst from 0 : (i32) -> i7
    %66 = comb.concat %64, %65 : i3, i7
    %67 = comb.icmp bin eq %66, %c19_i10 {sv.namehint = "_ctrlSignals_T_37"} : i10
    %68 = comb.extract %io_inst from 12 : (i32) -> i3
    %69 = comb.extract %io_inst from 0 : (i32) -> i7
    %70 = comb.concat %68, %69 : i3, i7
    %71 = comb.icmp bin eq %70, %c275_i10 {sv.namehint = "_ctrlSignals_T_39"} : i10
    %72 = comb.extract %io_inst from 12 : (i32) -> i3
    %73 = comb.extract %io_inst from 0 : (i32) -> i7
    %74 = comb.concat %72, %73 : i3, i7
    %75 = comb.icmp bin eq %74, %c403_i10 {sv.namehint = "_ctrlSignals_T_41"} : i10
    %76 = comb.extract %io_inst from 12 : (i32) -> i3
    %77 = comb.extract %io_inst from 0 : (i32) -> i7
    %78 = comb.concat %76, %77 : i3, i7
    %79 = comb.icmp bin eq %78, %c-493_i10 {sv.namehint = "_ctrlSignals_T_43"} : i10
    %80 = comb.extract %io_inst from 12 : (i32) -> i3
    %81 = comb.extract %io_inst from 0 : (i32) -> i7
    %82 = comb.concat %80, %81 : i3, i7
    %83 = comb.icmp bin eq %82, %c-237_i10 {sv.namehint = "_ctrlSignals_T_45"} : i10
    %84 = comb.extract %io_inst from 12 : (i32) -> i3
    %85 = comb.extract %io_inst from 0 : (i32) -> i7
    %86 = comb.concat %84, %85 : i3, i7
    %87 = comb.icmp bin eq %86, %c-109_i10 {sv.namehint = "_ctrlSignals_T_47"} : i10
    %88 = comb.extract %io_inst from 25 : (i32) -> i7
    %89 = comb.extract %io_inst from 12 : (i32) -> i3
    %90 = comb.extract %io_inst from 0 : (i32) -> i7
    %91 = comb.concat %88, %89, %90 : i7, i3, i7
    %92 = comb.icmp bin eq %91, %c147_i17 {sv.namehint = "_ctrlSignals_T_49"} : i17
    %93 = comb.extract %io_inst from 25 : (i32) -> i7
    %94 = comb.extract %io_inst from 12 : (i32) -> i3
    %95 = comb.extract %io_inst from 0 : (i32) -> i7
    %96 = comb.concat %93, %94, %95 : i7, i3, i7
    %97 = comb.icmp bin eq %96, %c659_i17 {sv.namehint = "_ctrlSignals_T_51"} : i17
    %98 = comb.extract %io_inst from 25 : (i32) -> i7
    %99 = comb.extract %io_inst from 12 : (i32) -> i3
    %100 = comb.extract %io_inst from 0 : (i32) -> i7
    %101 = comb.concat %98, %99, %100 : i7, i3, i7
    %102 = comb.icmp bin eq %101, %c33427_i17 {sv.namehint = "_ctrlSignals_T_53"} : i17
    %103 = comb.extract %io_inst from 25 : (i32) -> i7
    %104 = comb.extract %io_inst from 12 : (i32) -> i3
    %105 = comb.extract %io_inst from 0 : (i32) -> i7
    %106 = comb.concat %103, %104, %105 : i7, i3, i7
    %107 = comb.icmp bin eq %106, %c51_i17 {sv.namehint = "_ctrlSignals_T_55"} : i17
    %108 = comb.extract %io_inst from 25 : (i32) -> i7
    %109 = comb.extract %io_inst from 12 : (i32) -> i3
    %110 = comb.extract %io_inst from 0 : (i32) -> i7
    %111 = comb.concat %108, %109, %110 : i7, i3, i7
    %112 = comb.icmp bin eq %111, %c32819_i17 {sv.namehint = "_ctrlSignals_T_57"} : i17
    %113 = comb.extract %io_inst from 25 : (i32) -> i7
    %114 = comb.extract %io_inst from 12 : (i32) -> i3
    %115 = comb.extract %io_inst from 0 : (i32) -> i7
    %116 = comb.concat %113, %114, %115 : i7, i3, i7
    %117 = comb.icmp bin eq %116, %c179_i17 {sv.namehint = "_ctrlSignals_T_59"} : i17
    %118 = comb.extract %io_inst from 25 : (i32) -> i7
    %119 = comb.extract %io_inst from 12 : (i32) -> i3
    %120 = comb.extract %io_inst from 0 : (i32) -> i7
    %121 = comb.concat %118, %119, %120 : i7, i3, i7
    %122 = comb.icmp bin eq %121, %c307_i17 {sv.namehint = "_ctrlSignals_T_61"} : i17
    %123 = comb.extract %io_inst from 25 : (i32) -> i7
    %124 = comb.extract %io_inst from 12 : (i32) -> i3
    %125 = comb.extract %io_inst from 0 : (i32) -> i7
    %126 = comb.concat %123, %124, %125 : i7, i3, i7
    %127 = comb.icmp bin eq %126, %c435_i17 {sv.namehint = "_ctrlSignals_T_63"} : i17
    %128 = comb.extract %io_inst from 25 : (i32) -> i7
    %129 = comb.extract %io_inst from 12 : (i32) -> i3
    %130 = comb.extract %io_inst from 0 : (i32) -> i7
    %131 = comb.concat %128, %129, %130 : i7, i3, i7
    %132 = comb.icmp bin eq %131, %c563_i17 {sv.namehint = "_ctrlSignals_T_65"} : i17
    %133 = comb.extract %io_inst from 25 : (i32) -> i7
    %134 = comb.extract %io_inst from 12 : (i32) -> i3
    %135 = comb.extract %io_inst from 0 : (i32) -> i7
    %136 = comb.concat %133, %134, %135 : i7, i3, i7
    %137 = comb.icmp bin eq %136, %c691_i17 {sv.namehint = "_ctrlSignals_T_67"} : i17
    %138 = comb.extract %io_inst from 25 : (i32) -> i7
    %139 = comb.extract %io_inst from 12 : (i32) -> i3
    %140 = comb.extract %io_inst from 0 : (i32) -> i7
    %141 = comb.concat %138, %139, %140 : i7, i3, i7
    %142 = comb.icmp bin eq %141, %c33459_i17 {sv.namehint = "_ctrlSignals_T_69"} : i17
    %143 = comb.extract %io_inst from 25 : (i32) -> i7
    %144 = comb.extract %io_inst from 12 : (i32) -> i3
    %145 = comb.extract %io_inst from 0 : (i32) -> i7
    %146 = comb.concat %143, %144, %145 : i7, i3, i7
    %147 = comb.icmp bin eq %146, %c819_i17 {sv.namehint = "_ctrlSignals_T_71"} : i17
    %148 = comb.extract %io_inst from 25 : (i32) -> i7
    %149 = comb.extract %io_inst from 12 : (i32) -> i3
    %150 = comb.extract %io_inst from 0 : (i32) -> i7
    %151 = comb.concat %148, %149, %150 : i7, i3, i7
    %152 = comb.icmp bin eq %151, %c947_i17 {sv.namehint = "_ctrlSignals_T_206"} : i17
    %153 = comb.extract %io_inst from 28 : (i32) -> i4
    %154 = comb.extract %io_inst from 0 : (i32) -> i20
    %155 = comb.concat %153, %154 : i4, i20
    %156 = comb.icmp bin eq %155, %c15_i24 {sv.namehint = "_ctrlSignals_T_75"} : i24
    %157 = comb.icmp bin eq %io_inst, %c4111_i32 {sv.namehint = "_ctrlSignals_T_77"} : i32
    %158 = comb.extract %io_inst from 12 : (i32) -> i3
    %159 = comb.extract %io_inst from 0 : (i32) -> i7
    %160 = comb.concat %158, %159 : i3, i7
    %161 = comb.icmp bin eq %160, %c243_i10 {sv.namehint = "_ctrlSignals_T_79"} : i10
    %162 = comb.extract %io_inst from 12 : (i32) -> i3
    %163 = comb.extract %io_inst from 0 : (i32) -> i7
    %164 = comb.concat %162, %163 : i3, i7
    %165 = comb.icmp bin eq %164, %c371_i10 {sv.namehint = "_ctrlSignals_T_81"} : i10
    %166 = comb.extract %io_inst from 12 : (i32) -> i3
    %167 = comb.extract %io_inst from 0 : (i32) -> i7
    %168 = comb.concat %166, %167 : i3, i7
    %169 = comb.icmp bin eq %168, %c499_i10 {sv.namehint = "_ctrlSignals_T_153"} : i10
    %170 = comb.extract %io_inst from 12 : (i32) -> i3
    %171 = comb.extract %io_inst from 0 : (i32) -> i7
    %172 = comb.concat %170, %171 : i3, i7
    %173 = comb.icmp bin eq %172, %c-269_i10 {sv.namehint = "_ctrlSignals_T_85"} : i10
    %174 = comb.extract %io_inst from 12 : (i32) -> i3
    %175 = comb.extract %io_inst from 0 : (i32) -> i7
    %176 = comb.concat %174, %175 : i3, i7
    %177 = comb.icmp bin eq %176, %c-141_i10 {sv.namehint = "_ctrlSignals_T_87"} : i10
    %178 = comb.extract %io_inst from 12 : (i32) -> i3
    %179 = comb.extract %io_inst from 0 : (i32) -> i7
    %180 = comb.concat %178, %179 : i3, i7
    %181 = comb.icmp bin eq %180, %c-13_i10 {sv.namehint = "_ctrlSignals_T_582"} : i10
    %182 = comb.icmp bin eq %io_inst, %c115_i32 {sv.namehint = "_ctrlSignals_T_91"} : i32
    %183 = comb.icmp bin eq %io_inst, %c1048691_i32 {sv.namehint = "_ctrlSignals_T_93"} : i32
    %184 = comb.icmp bin eq %io_inst, %c268435571_i32 {sv.namehint = "_ctrlSignals_T_387"} : i32
    %185 = comb.replicate %184 {sv.namehint = "_ctrlSignals_T_99"} : (i1) -> i2
    %186 = comb.or bin %182, %183 : i1
    %187 = comb.mux bin %186, %c0_i2, %185 {sv.namehint = "_ctrlSignals_T_101"} : i2
    %188 = comb.or bin %173, %177, %181 : i1
    %189 = comb.or bin %157, %161, %165, %169, %188 : i1
    %190 = comb.mux bin %189, %c-2_i2, %187 {sv.namehint = "_ctrlSignals_T_108"} : i2
    %191 = comb.or bin %55, %59, %63, %67, %71, %75, %79, %83, %87, %92, %97, %102, %107, %112, %117, %122, %127, %132, %137, %142, %147, %152, %156 : i1
    %192 = comb.mux bin %191, %c0_i2, %190 {sv.namehint = "_ctrlSignals_T_131"} : i2
    %193 = comb.or bin %35, %39, %43, %47, %51 : i1
    %194 = comb.mux bin %193, %c-2_i2, %192 {sv.namehint = "_ctrlSignals_T_136"} : i2
    %195 = comb.or bin %11, %15, %19, %23, %27, %31 : i1
    %196 = comb.mux bin %195, %c0_i2, %194 {sv.namehint = "_ctrlSignals_T_142"} : i2
    %197 = comb.or bin %3, %7 : i1
    %198 = comb.mux bin %197, %c1_i2, %196 {sv.namehint = "_ctrlSignals_T_144"} : i2
    %199 = comb.or bin %1, %2 : i1
    %200 = comb.mux bin %199, %c0_i2, %198 {sv.namehint = "ctrlSignals_0"} : i2
    %201 = comb.or %161, %165, %169 {sv.namehint = "_ctrlSignals_T_155"} : i1
    %202 = comb.or bin %156, %157 : i1
    %203 = comb.xor %202, %true : i1
    %204 = comb.and %203, %201 {sv.namehint = "_ctrlSignals_T_157"} : i1
    %205 = comb.or bin %67, %71, %75, %79, %83, %87, %92, %97, %102, %107, %112, %117, %122, %127, %132, %137, %142, %147, %152 : i1
    %206 = comb.or %35, %39, %43, %47, %51, %55, %59, %63, %205, %204 {sv.namehint = "_ctrlSignals_T_184"} : i1
    %207 = comb.xor %195, %true : i1
    %208 = comb.and %207, %206 {sv.namehint = "_ctrlSignals_T_190"} : i1
    %209 = comb.or %7, %208 {sv.namehint = "_ctrlSignals_T_191"} : i1
    %210 = comb.or bin %1, %2, %3 : i1
    %211 = comb.xor %210, %true : i1
    %212 = comb.and %211, %209 {sv.namehint = "ctrlSignals_1"} : i1
    %213 = comb.or %107, %112, %117, %122, %127, %132, %137, %142, %147, %152 {sv.namehint = "_ctrlSignals_T_215"} : i1
    %214 = comb.or bin %67, %71, %75, %79, %83, %87, %92, %97, %102 : i1
    %215 = comb.or bin %1, %2, %3, %7, %11, %15, %19, %23, %27, %31, %35, %39, %43, %47, %51, %55, %59, %63, %214 : i1
    %216 = comb.xor %215, %true : i1
    %217 = comb.and %216, %213 {sv.namehint = "ctrlSignals_2"} : i1
    %218 = comb.or bin %161, %165, %169 : i1
    %219 = comb.xor %188, %true : i1
    %220 = comb.or %107, %112, %117, %122, %127, %132, %137, %142, %147, %152, %156, %157, %218, %219 : i1
    %221 = comb.mux bin %220, %c0_i3, %c-2_i3 {sv.namehint = "_ctrlSignals_T_263"} : i3
    %222 = comb.mux bin %214, %c1_i3, %221 {sv.namehint = "_ctrlSignals_T_272"} : i3
    %223 = comb.or bin %55, %59, %63 : i1
    %224 = comb.mux bin %223, %c2_i3, %222 {sv.namehint = "_ctrlSignals_T_275"} : i3
    %225 = comb.mux bin %193, %c1_i3, %224 {sv.namehint = "_ctrlSignals_T_280"} : i3
    %226 = comb.mux bin %195, %c-3_i3, %225 {sv.namehint = "_ctrlSignals_T_286"} : i3
    %227 = comb.mux bin %7, %c1_i3, %226 {sv.namehint = "_ctrlSignals_T_287"} : i3
    %228 = comb.mux bin %3, %c-4_i3, %227 {sv.namehint = "_ctrlSignals_T_288"} : i3
    %229 = comb.mux bin %199, %c3_i3, %228 {sv.namehint = "ctrlSignals_3"} : i3
    %230 = comb.xor %218, %true : i1
    %231 = comb.or %202, %230 : i1
    %232 = comb.mux bin %231, %c-1_i4, %c-6_i4 {sv.namehint = "_ctrlSignals_T_301"} : i4
    %233 = comb.mux bin %152, %c2_i4, %232 {sv.namehint = "_ctrlSignals_T_302"} : i4
    %234 = comb.mux bin %147, %c3_i4, %233 {sv.namehint = "_ctrlSignals_T_303"} : i4
    %235 = comb.mux bin %142, %c-7_i4, %234 {sv.namehint = "_ctrlSignals_T_304"} : i4
    %236 = comb.mux bin %137, %c-8_i4, %235 {sv.namehint = "_ctrlSignals_T_305"} : i4
    %237 = comb.mux bin %132, %c4_i4, %236 {sv.namehint = "_ctrlSignals_T_306"} : i4
    %238 = comb.mux bin %127, %c7_i4, %237 {sv.namehint = "_ctrlSignals_T_307"} : i4
    %239 = comb.mux bin %122, %c5_i4, %238 {sv.namehint = "_ctrlSignals_T_308"} : i4
    %240 = comb.mux bin %117, %c6_i4, %239 {sv.namehint = "_ctrlSignals_T_309"} : i4
    %241 = comb.mux bin %112, %c1_i4, %240 {sv.namehint = "_ctrlSignals_T_310"} : i4
    %242 = comb.mux bin %107, %c0_i4, %241 {sv.namehint = "_ctrlSignals_T_311"} : i4
    %243 = comb.mux bin %102, %c-7_i4, %242 {sv.namehint = "_ctrlSignals_T_312"} : i4
    %244 = comb.mux bin %97, %c-8_i4, %243 {sv.namehint = "_ctrlSignals_T_313"} : i4
    %245 = comb.mux bin %92, %c6_i4, %244 {sv.namehint = "_ctrlSignals_T_314"} : i4
    %246 = comb.mux bin %87, %c2_i4, %245 {sv.namehint = "_ctrlSignals_T_315"} : i4
    %247 = comb.mux bin %83, %c3_i4, %246 {sv.namehint = "_ctrlSignals_T_316"} : i4
    %248 = comb.mux bin %79, %c4_i4, %247 {sv.namehint = "_ctrlSignals_T_317"} : i4
    %249 = comb.mux bin %75, %c7_i4, %248 {sv.namehint = "_ctrlSignals_T_318"} : i4
    %250 = comb.mux bin %71, %c5_i4, %249 {sv.namehint = "_ctrlSignals_T_319"} : i4
    %251 = comb.or bin %2, %3, %7, %11, %15, %19, %23, %27, %31, %35, %39, %43, %47, %51, %55, %59, %63, %67 : i1
    %252 = comb.mux bin %251, %c0_i4, %250 {sv.namehint = "_ctrlSignals_T_337"} : i4
    %253 = comb.mux bin %1, %c-5_i4, %252 {sv.namehint = "ctrlSignals_4"} : i4
    %254 = comb.concat %31, %c0_i2 {sv.namehint = "_ctrlSignals_T_377"} : i1, i2
    %255 = comb.mux bin %27, %c1_i3, %254 {sv.namehint = "_ctrlSignals_T_378"} : i3
    %256 = comb.mux bin %23, %c-3_i3, %255 {sv.namehint = "_ctrlSignals_T_379"} : i3
    %257 = comb.mux bin %19, %c2_i3, %256 {sv.namehint = "_ctrlSignals_T_380"} : i3
    %258 = comb.mux bin %15, %c-2_i3, %257 {sv.namehint = "_ctrlSignals_T_381"} : i3
    %259 = comb.mux bin %11, %c3_i3, %258 {sv.namehint = "_ctrlSignals_T_382"} : i3
    %260 = comb.or bin %1, %2, %197 : i1
    %261 = comb.mux bin %260, %c0_i3, %259 {sv.namehint = "ctrlSignals_5"} : i3
    %262 = comb.xor %186, %true : i1
    %263 = comb.and %262, %184 {sv.namehint = "_ctrlSignals_T_389"} : i1
    %264 = comb.or %189, %263 {sv.namehint = "_ctrlSignals_T_396"} : i1
    %265 = comb.xor %191, %true : i1
    %266 = comb.and %265, %264 {sv.namehint = "_ctrlSignals_T_419"} : i1
    %267 = comb.or %193, %266 {sv.namehint = "_ctrlSignals_T_424"} : i1
    %268 = comb.xor %195, %true : i1
    %269 = comb.and %268, %267 {sv.namehint = "_ctrlSignals_T_430"} : i1
    %270 = comb.or %197, %269 {sv.namehint = "_ctrlSignals_T_432"} : i1
    %271 = comb.xor %199, %true : i1
    %272 = comb.and %271, %270 {sv.namehint = "ctrlSignals_6"} : i1
    %273 = comb.concat %false, %63 {sv.namehint = "_ctrlSignals_T_465"} : i1, i1
    %274 = comb.mux bin %59, %c-2_i2, %273 {sv.namehint = "_ctrlSignals_T_466"} : i2
    %275 = comb.mux bin %55, %c-1_i2, %274 {sv.namehint = "_ctrlSignals_T_467"} : i2
    %276 = comb.or bin %1, %2, %3, %7, %11, %15, %19, %23, %27, %31, %193 : i1
    %277 = comb.mux bin %276, %c0_i2, %275 {sv.namehint = "ctrlSignals_7"} : i2
    %278 = comb.concat %51, %c0_i2 {sv.namehint = "_ctrlSignals_T_516"} : i1, i2
    %279 = comb.mux bin %47, %c-3_i3, %278 {sv.namehint = "_ctrlSignals_T_517"} : i3
    %280 = comb.mux bin %43, %c1_i3, %279 {sv.namehint = "_ctrlSignals_T_518"} : i3
    %281 = comb.mux bin %39, %c2_i3, %280 {sv.namehint = "_ctrlSignals_T_519"} : i3
    %282 = comb.mux bin %35, %c3_i3, %281 {sv.namehint = "_ctrlSignals_T_520"} : i3
    %283 = comb.or bin %1, %2, %3, %7, %195 : i1
    %284 = comb.mux bin %283, %c0_i3, %282 {sv.namehint = "ctrlSignals_8"} : i3
    %285 = comb.or bin %182, %183, %184 : i1
    %286 = comb.or bin %161, %165, %169, %173, %177, %181, %285 : i1
    %287 = comb.replicate %286 {sv.namehint = "_ctrlSignals_T_539"} : (i1) -> i2
    %288 = comb.or bin %55, %59, %63, %67, %71, %75, %79, %83, %87, %92, %97, %102, %107, %112, %117, %122, %127, %132, %137, %142, %147, %152, %202 : i1
    %289 = comb.mux bin %288, %c0_i2, %287 {sv.namehint = "_ctrlSignals_T_563"} : i2
    %290 = comb.mux bin %193, %c1_i2, %289 {sv.namehint = "_ctrlSignals_T_568"} : i2
    %291 = comb.mux bin %195, %c0_i2, %290 {sv.namehint = "_ctrlSignals_T_574"} : i2
    %292 = comb.mux bin %197, %c-2_i2, %291 {sv.namehint = "_ctrlSignals_T_576"} : i2
    %293 = comb.mux bin %199, %c0_i2, %292 {sv.namehint = "ctrlSignals_9"} : i2
    %294 = comb.or %161, %165, %169, %173, %177, %181 {sv.namehint = "_ctrlSignals_T_587"} : i1
    %295 = comb.xor %202, %true : i1
    %296 = comb.and %295, %294 {sv.namehint = "_ctrlSignals_T_589"} : i1
    %297 = comb.or %205, %296 {sv.namehint = "_ctrlSignals_T_608"} : i1
    %298 = comb.xor %223, %true : i1
    %299 = comb.and %298, %297 {sv.namehint = "_ctrlSignals_T_611"} : i1
    %300 = comb.or %193, %299 {sv.namehint = "_ctrlSignals_T_616"} : i1
    %301 = comb.xor %195, %true : i1
    %302 = comb.and %301, %300 {sv.namehint = "_ctrlSignals_T_622"} : i1
    %303 = comb.or %260, %302 {sv.namehint = "ctrlSignals_10"} : i1
    %304 = comb.concat %285, %c0_i2 {sv.namehint = "_ctrlSignals_T_629"} : i1, i2
    %305 = comb.mux bin %181, %c3_i3, %304 {sv.namehint = "_ctrlSignals_T_630"} : i3
    %306 = comb.mux bin %177, %c2_i3, %305 {sv.namehint = "_ctrlSignals_T_631"} : i3
    %307 = comb.mux bin %173, %c1_i3, %306 {sv.namehint = "_ctrlSignals_T_632"} : i3
    %308 = comb.mux bin %169, %c3_i3, %307 {sv.namehint = "_ctrlSignals_T_633"} : i3
    %309 = comb.mux bin %165, %c2_i3, %308 {sv.namehint = "_ctrlSignals_T_634"} : i3
    %310 = comb.mux bin %161, %c1_i3, %309 {sv.namehint = "_ctrlSignals_T_635"} : i3
    %311 = comb.or bin %1, %2, %3, %7, %11, %15, %19, %23, %27, %31, %35, %39, %43, %47, %51, %288 : i1
    %312 = comb.mux bin %311, %c0_i3, %310 {sv.namehint = "ctrlSignals_11"} : i3
    %313 = comb.icmp bin ne %io_inst, %c270532723_i32 {sv.namehint = "_ctrlSignals_T_674"} : i32
    %314 = comb.or bin %1, %2, %3, %7, %11, %15, %19, %23, %27, %31, %35, %39, %43, %47, %51, %55, %59, %63, %67, %71, %75, %79, %83, %87, %92, %97, %102, %107, %112, %117, %122, %127, %132, %137, %142, %147, %152, %156, %157, %286 : i1
    %315 = comb.xor %314, %true : i1
    %316 = comb.and %315, %313 {sv.namehint = "io_illegal"} : i1
    hw.output %200, %272, %212, %217, %229, %253, %261, %277, %284, %293, %303, %312, %316 : i2, i1, i1, i1, i3, i4, i3, i2, i3, i2, i1, i3, i1
  }
  hw.module private @Core(in %clock : !seq.clock, in %reset : i1, in %io_host_fromhost_valid : i1, in %io_host_fromhost_bits : i32, out io_host_tohost : i32, out io_icache_req_valid : i1, out io_icache_req_bits_addr : i32, in %io_icache_resp_valid : i1, in %io_icache_resp_bits_data : i32, out io_dcache_abort : i1, out io_dcache_req_valid : i1, out io_dcache_req_bits_addr : i32, out io_dcache_req_bits_data : i32, out io_dcache_req_bits_mask : i4, in %io_dcache_resp_valid : i1, in %io_dcache_resp_bits_data : i32) {
    %dpath.io_host_tohost, %dpath.io_icache_req_valid, %dpath.io_icache_req_bits_addr, %dpath.io_dcache_abort, %dpath.io_dcache_req_valid, %dpath.io_dcache_req_bits_addr, %dpath.io_dcache_req_bits_data, %dpath.io_dcache_req_bits_mask, %dpath.io_ctrl_inst = hw.instance "dpath" @Datapath(clock: %clock: !seq.clock, reset: %reset: i1, io_host_fromhost_valid: %io_host_fromhost_valid: i1, io_host_fromhost_bits: %io_host_fromhost_bits: i32, io_icache_resp_valid: %io_icache_resp_valid: i1, io_icache_resp_bits_data: %io_icache_resp_bits_data: i32, io_dcache_resp_valid: %io_dcache_resp_valid: i1, io_dcache_resp_bits_data: %io_dcache_resp_bits_data: i32, io_ctrl_pc_sel: %ctrl.io_pc_sel: i2, io_ctrl_inst_kill: %ctrl.io_inst_kill: i1, io_ctrl_A_sel: %ctrl.io_A_sel: i1, io_ctrl_B_sel: %ctrl.io_B_sel: i1, io_ctrl_imm_sel: %ctrl.io_imm_sel: i3, io_ctrl_alu_op: %ctrl.io_alu_op: i4, io_ctrl_br_type: %ctrl.io_br_type: i3, io_ctrl_st_type: %ctrl.io_st_type: i2, io_ctrl_ld_type: %ctrl.io_ld_type: i3, io_ctrl_wb_sel: %ctrl.io_wb_sel: i2, io_ctrl_wb_en: %ctrl.io_wb_en: i1, io_ctrl_csr_cmd: %ctrl.io_csr_cmd: i3, io_ctrl_illegal: %ctrl.io_illegal: i1) -> (io_host_tohost: i32, io_icache_req_valid: i1, io_icache_req_bits_addr: i32, io_dcache_abort: i1, io_dcache_req_valid: i1, io_dcache_req_bits_addr: i32, io_dcache_req_bits_data: i32, io_dcache_req_bits_mask: i4, io_ctrl_inst: i32) {sv.namehint = "io_host_tohost"}
    %ctrl.io_pc_sel, %ctrl.io_inst_kill, %ctrl.io_A_sel, %ctrl.io_B_sel, %ctrl.io_imm_sel, %ctrl.io_alu_op, %ctrl.io_br_type, %ctrl.io_st_type, %ctrl.io_ld_type, %ctrl.io_wb_sel, %ctrl.io_wb_en, %ctrl.io_csr_cmd, %ctrl.io_illegal = hw.instance "ctrl" @Control(io_inst: %dpath.io_ctrl_inst: i32) -> (io_pc_sel: i2, io_inst_kill: i1, io_A_sel: i1, io_B_sel: i1, io_imm_sel: i3, io_alu_op: i4, io_br_type: i3, io_st_type: i2, io_ld_type: i3, io_wb_sel: i2, io_wb_en: i1, io_csr_cmd: i3, io_illegal: i1)
    hw.output %dpath.io_host_tohost, %dpath.io_icache_req_valid, %dpath.io_icache_req_bits_addr, %dpath.io_dcache_abort, %dpath.io_dcache_req_valid, %dpath.io_dcache_req_bits_addr, %dpath.io_dcache_req_bits_data, %dpath.io_dcache_req_bits_mask : i32, i1, i32, i1, i1, i32, i32, i4
  }
  hw.module private @Cache(in %clock : !seq.clock, in %reset : i1, in %io_cpu_abort : i1, in %io_cpu_req_valid : i1, in %io_cpu_req_bits_addr : i32, in %io_cpu_req_bits_data : i32, in %io_cpu_req_bits_mask : i4, out io_cpu_resp_valid : i1, out io_cpu_resp_bits_data : i32, in %io_nasti_aw_ready : i1, out io_nasti_aw_valid : i1, out io_nasti_aw_bits_addr : i32, in %io_nasti_w_ready : i1, out io_nasti_w_valid : i1, out io_nasti_w_bits_data : i64, out io_nasti_w_bits_last : i1, out io_nasti_b_ready : i1, in %io_nasti_b_valid : i1, in %io_nasti_ar_ready : i1, out io_nasti_ar_valid : i1, out io_nasti_ar_bits_addr : i32, out io_nasti_r_ready : i1, in %io_nasti_r_valid : i1, in %io_nasti_r_bits_data : i64) {
    %false = hw.constant false
    %true = hw.constant true
    %c-1_i16 = hw.constant -1 : i16
    %c0_i4 = hw.constant 0 : i4
    %c1_i256 = hw.constant 1 : i256
    %c2_i3 = hw.constant 2 : i3
    %c1_i3 = hw.constant 1 : i3
    %c-1_i256 = hw.constant -1 : i256
    %c0_i15 = hw.constant 0 : i15
    %c0_i248 = hw.constant 0 : i248
    %c0_i256 = hw.constant 0 : i256
    %c0_i2 = hw.constant 0 : i2
    %c3_i3 = hw.constant 3 : i3
    %c1_i2 = hw.constant 1 : i2
    %c0_i3 = hw.constant 0 : i3
    %c-2_i2 = hw.constant -2 : i2
    %c-4_i3 = hw.constant -4 : i3
    %c-3_i3 = hw.constant -3 : i3
    %c-2_i3 = hw.constant -2 : i3
    %state = seq.firreg %161 clock %clock reset sync %reset, %c0_i3 {firrtl.random_init_start = 0 : ui64} : i3
    %v = seq.firreg %84 clock %clock reset sync %reset, %c0_i256 {firrtl.random_init_start = 3 : ui64} : i256
    %d = seq.firreg %90 clock %clock reset sync %reset, %c0_i256 {firrtl.random_init_start = 259 : ui64} : i256
    %metaMem_tag = seq.firmem 1, 1, undefined, port_order {prefix = ""} : <256 x 20>
    %0 = seq.firmem.read_write_port %metaMem_tag[%1] = %45 if %40, clock %clock enable %2 {sv.namehint = "metaMem_rmeta_data_tag"} : <256 x 20>
    %1 = comb.mux bin %91, %46, %44 : i8
    %2 = comb.or bin %43, %91 : i1
    %dataMem_0 = seq.firmem 1, 1, undefined, port_order {prefix = ""} : <256 x 32, mask 4>
    %3 = seq.firmem.read_write_port %dataMem_0[%4] = %6 if %40, clock %clock enable %5 mask %7 : <256 x 32, mask 4>, i4
    %4 = comb.mux bin %40, %46, %44 : i8
    %5 = comb.or bin %43, %40 : i1
    %6 = comb.extract %80 from 0 : (i128) -> i32
    %7 = comb.extract %76 from 0 : (i16) -> i4
    %dataMem_1 = seq.firmem 1, 1, undefined, port_order {prefix = ""} : <256 x 32, mask 4>
    %8 = seq.firmem.read_write_port %dataMem_1[%9] = %11 if %40, clock %clock enable %10 mask %12 : <256 x 32, mask 4>, i4
    %9 = comb.mux bin %40, %46, %44 : i8
    %10 = comb.or bin %43, %40 : i1
    %11 = comb.extract %80 from 32 : (i128) -> i32
    %12 = comb.extract %76 from 4 : (i16) -> i4
    %dataMem_2 = seq.firmem 1, 1, undefined, port_order {prefix = ""} : <256 x 32, mask 4>
    %13 = seq.firmem.read_write_port %dataMem_2[%14] = %16 if %40, clock %clock enable %15 mask %17 : <256 x 32, mask 4>, i4
    %14 = comb.mux bin %40, %46, %44 : i8
    %15 = comb.or bin %43, %40 : i1
    %16 = comb.extract %80 from 64 : (i128) -> i32
    %17 = comb.extract %76 from 8 : (i16) -> i4
    %dataMem_3 = seq.firmem 1, 1, undefined, port_order {prefix = ""} : <256 x 32, mask 4>
    %18 = seq.firmem.read_write_port %dataMem_3[%19] = %21 if %40, clock %clock enable %20 mask %22 : <256 x 32, mask 4>, i4
    %19 = comb.mux bin %40, %46, %44 : i8
    %20 = comb.or bin %43, %40 : i1
    %21 = comb.extract %80 from 96 : (i128) -> i32
    %22 = comb.extract %76 from 12 : (i16) -> i4
    %addr_reg = seq.firreg %69 clock %clock {firrtl.random_init_start = 515 : ui64} : i32
    %cpu_data = seq.firreg %70 clock %clock {firrtl.random_init_start = 547 : ui64} : i32
    %cpu_mask = seq.firreg %71 clock %clock {firrtl.random_init_start = 579 : ui64} : i4
    %23 = comb.and bin %35, %io_nasti_r_valid : i1
    %wrap_wrap = seq.firreg %25 clock %clock reset sync %reset, %false {firrtl.random_init_start = 583 : ui64} : i1
    %24 = comb.add %wrap_wrap, %true {sv.namehint = "_wrap_value_T"} : i1
    %25 = comb.mux bin %23, %24, %wrap_wrap : i1
    %26 = comb.and %23, %wrap_wrap {sv.namehint = "read_wrap_out"} : i1
    %27 = comb.and bin %io_nasti_w_ready, %140 : i1
    %write_count = seq.firreg %29 clock %clock reset sync %reset, %false {firrtl.random_init_start = 584 : ui64} : i1
    %28 = comb.add %write_count, %true {sv.namehint = "_wrap_value_T_2"} : i1
    %29 = comb.mux bin %27, %28, %write_count : i1
    %30 = comb.and %27, %write_count {sv.namehint = "write_wrap_out"} : i1
    %31 = comb.icmp bin ne %state, %c0_i3 : i3
    %32 = comb.xor bin %31, %true {sv.namehint = "is_idle"} : i1
    %33 = comb.icmp bin eq %state, %c1_i3 {sv.namehint = "is_read"} : i3
    %34 = comb.icmp bin eq %state, %c2_i3 {sv.namehint = "is_write"} : i3
    %35 = comb.icmp bin eq %state, %c-2_i3 {sv.namehint = "io_nasti_r_ready"} : i3
    %36 = comb.and bin %35, %26 {sv.namehint = "is_alloc"} : i1
    %is_alloc_reg = seq.firreg %36 clock %clock {firrtl.random_init_start = 585 : ui64} : i1
    %37 = comb.or bin %57, %is_alloc_reg {sv.namehint = "_wen_T"} : i1
    %38 = comb.xor bin %io_cpu_abort, %true {sv.namehint = "_wen_T_2"} : i1
    %39 = comb.and bin %34, %37, %38 {sv.namehint = "_wen_T_3"} : i1
    %40 = comb.or bin %39, %36 {sv.namehint = "wen"} : i1
    %41 = comb.xor bin %40, %true {sv.namehint = "_ren_T"} : i1
    %42 = comb.or bin %32, %33 {sv.namehint = "_ren_T_1"} : i1
    %43 = comb.and bin %41, %42, %io_cpu_req_valid {sv.namehint = "ren"} : i1
    %ren_reg = seq.firreg %43 clock %clock {firrtl.random_init_start = 586 : ui64} : i1
    %44 = comb.extract %io_cpu_req_bits_addr from 4 {sv.namehint = "idx"} : (i32) -> i8
    %45 = comb.extract %addr_reg from 12 {sv.namehint = "tag_reg"} : (i32) -> i20
    %46 = comb.extract %addr_reg from 4 {sv.namehint = "writeAddr"} : (i32) -> i8
    %47 = comb.extract %addr_reg from 2 {sv.namehint = "off_reg"} : (i32) -> i2
    %48 = comb.concat %18, %13, %8, %3 {sv.namehint = "rdata"} : i32, i32, i32, i32
    %rdata_buf = seq.firreg %49 clock %clock {firrtl.random_init_start = 587 : ui64} : i128
    %49 = comb.mux bin %ren_reg, %48, %rdata_buf : i128
    %refill_buf_0 = seq.firreg %96 clock %clock {firrtl.random_init_start = 715 : ui64} : i64
    %refill_buf_1 = seq.firreg %98 clock %clock {firrtl.random_init_start = 779 : ui64} : i64
    %50 = comb.concat %refill_buf_1, %refill_buf_0 {sv.namehint = "_read_T"} : i64, i64
    %51 = comb.mux bin %ren_reg, %48, %rdata_buf {sv.namehint = "_read_T_1"} : i128
    %52 = comb.mux bin %is_alloc_reg, %50, %51 {sv.namehint = "read"} : i128
    %53 = comb.concat %c0_i248, %46 : i248, i8
    %54 = comb.shru bin %v, %53 {sv.namehint = "_is_dirty_T"} : i256
    %55 = comb.extract %54 from 0 {sv.namehint = "_hit_T_1"} : (i256) -> i1
    %56 = comb.icmp bin eq %0, %45 {sv.namehint = "_hit_T_2"} : i20
    %57 = comb.and bin %55, %56 {sv.namehint = "hit"} : i1
    %58 = comb.extract %52 from 0 {sv.namehint = "_io_cpu_resp_bits_data_T"} : (i128) -> i32
    %59 = comb.extract %52 from 32 {sv.namehint = "_io_cpu_resp_bits_data_T_1"} : (i128) -> i32
    %60 = comb.extract %52 from 64 {sv.namehint = "_io_cpu_resp_bits_data_T_2"} : (i128) -> i32
    %61 = comb.extract %52 from 96 {sv.namehint = "_io_cpu_resp_bits_data_T_3"} : (i128) -> i32
    %62 = hw.array_create %61, %60, %59, %58 : i32
    %63 = hw.array_get %62[%47] {sv.namehint = "io_cpu_resp_bits_data"} : !hw.array<4xi32>, i2
    %64 = comb.and bin %33, %57 {sv.namehint = "_io_cpu_resp_valid_T"} : i1
    %65 = comb.icmp bin ne %cpu_mask, %c0_i4 {sv.namehint = "_state_T_4"} : i4
    %66 = comb.xor bin %65, %true {sv.namehint = "_io_cpu_resp_valid_T_3"} : i1
    %67 = comb.and bin %is_alloc_reg, %66 {sv.namehint = "_io_cpu_resp_valid_T_4"} : i1
    %68 = comb.or bin %32, %64, %67 {sv.namehint = "io_cpu_resp_valid"} : i1
    %69 = comb.mux bin %68, %io_cpu_req_bits_addr, %addr_reg : i32
    %70 = comb.mux bin %68, %io_cpu_req_bits_data, %cpu_data : i32
    %71 = comb.mux bin %68, %io_cpu_req_bits_mask, %cpu_mask : i4
    %72 = comb.concat %c0_i15, %cpu_mask : i15, i4
    %73 = comb.concat %c0_i15, %47, %c0_i2 : i15, i2, i2
    %74 = comb.shl bin %72, %73 {sv.namehint = "_wmask_T_2"} : i19
    %75 = comb.extract %74 from 0 : (i19) -> i16
    %76 = comb.mux %36, %c-1_i16, %75 {sv.namehint = "wmask"} : i16
    %77 = comb.replicate %cpu_data {sv.namehint = "_wdata_T_1"} : (i32) -> i64
    %78 = comb.replicate %77 {sv.namehint = "_wdata_T_2"} : (i64) -> i128
    %79 = comb.concat %io_nasti_r_bits_data, %refill_buf_0 {sv.namehint = "_wdata_T_3"} : i64, i64
    %80 = comb.mux bin %36, %79, %78 {sv.namehint = "wdata"} : i128
    %81 = comb.shl bin %c1_i256, %53 {sv.namehint = "_d_T_1"} : i256
    %82 = comb.replicate %40 : (i1) -> i256
    %83 = comb.and %82, %81 : i256
    %84 = comb.or %83, %v : i256
    %85 = comb.or bin %d, %81 {sv.namehint = "_d_T_2"} : i256
    %86 = comb.xor bin %d, %c-1_i256 {sv.namehint = "_d_T_3"} : i256
    %87 = comb.or bin %86, %81 {sv.namehint = "_d_T_4"} : i256
    %88 = comb.xor bin %87, %c-1_i256 {sv.namehint = "_d_T_5"} : i256
    %89 = comb.mux bin %36, %88, %85 {sv.namehint = "_d_T_6"} : i256
    %90 = comb.mux bin %40, %89, %d : i256
    %91 = comb.and %40, %36 {sv.namehint = "writeEnable"} : i1
    %92 = comb.extract %addr_reg from 4 {sv.namehint = "_io_nasti_ar_bits_T"} : (i32) -> i28
    %93 = comb.concat %92, %c0_i4 {sv.namehint = "io_nasti_ar_bits_aw_addr"} : i28, i4
    %94 = comb.xor bin %wrap_wrap, %true : i1
    %95 = comb.and bin %23, %94 : i1
    %96 = comb.mux bin %95, %io_nasti_r_bits_data, %refill_buf_0 : i64
    %97 = comb.and bin %23, %wrap_wrap : i1
    %98 = comb.mux bin %97, %io_nasti_r_bits_data, %refill_buf_1 : i64
    %99 = comb.concat %0, %46, %c0_i4 {sv.namehint = "io_nasti_aw_bits_aw_addr"} : i20, i8, i4
    %100 = comb.extract %52 from 0 {sv.namehint = "_io_nasti_w_bits_T"} : (i128) -> i64
    %101 = comb.extract %52 from 64 {sv.namehint = "_io_nasti_w_bits_T_1"} : (i128) -> i64
    %102 = comb.mux bin %write_count, %101, %100 {sv.namehint = "io_nasti_w_bits_w_data"} : i64
    %103 = comb.extract %54 from 0 {sv.namehint = "_is_dirty_T_1"} : (i256) -> i1
    %104 = comb.shru bin %d, %53 {sv.namehint = "_is_dirty_T_2"} : i256
    %105 = comb.extract %104 from 0 {sv.namehint = "_is_dirty_T_3"} : (i256) -> i1
    %106 = comb.and bin %103, %105 {sv.namehint = "is_dirty"} : i1
    %107 = comb.xor bin %31, %true : i1
    %108 = comb.icmp bin ne %io_cpu_req_bits_mask, %c0_i4 {sv.namehint = "_state_T"} : i4
    %109 = comb.mux bin %108, %c-2_i2, %c1_i2 {sv.namehint = "_state_T_1"} : i2
    %110 = comb.concat %false, %109 : i1, i2
    %111 = comb.mux bin %io_cpu_req_valid, %110, %state : i3
    %112 = comb.icmp bin eq %state, %c1_i3 : i3
    %113 = comb.icmp bin ne %io_cpu_req_bits_mask, %c0_i4 {sv.namehint = "_state_T_2"} : i4
    %114 = comb.mux bin %113, %c-2_i2, %c1_i2 {sv.namehint = "_state_T_3"} : i2
    %115 = comb.concat %false, %114 : i1, i2
    %116 = comb.mux bin %io_cpu_req_valid, %115, %c0_i3 : i3
    %117 = comb.xor %57, %true : i1
    %118 = comb.and %117, %106 : i1
    %119 = comb.xor bin %106, %true {sv.namehint = "_io_nasti_ar_valid_T"} : i1
    %120 = comb.xor %57, %true : i1
    %121 = comb.and %120, %119 : i1
    %122 = comb.and bin %io_nasti_aw_ready, %132 : i1
    %123 = comb.and bin %io_nasti_ar_ready, %154 : i1
    %124 = comb.mux bin %123, %c-2_i3, %state : i3
    %125 = comb.mux bin %122, %c3_i3, %124 : i3
    %126 = comb.mux bin %57, %116, %125 : i3
    %127 = comb.icmp bin eq %state, %c2_i3 : i3
    %128 = comb.or bin %37, %io_cpu_abort : i1
    %129 = comb.xor %128, %true : i1
    %130 = comb.and %127, %129, %106 : i1
    %131 = comb.mux bin %112, %118, %130 : i1
    %132 = comb.and %31, %131 {sv.namehint = "io_nasti_aw_valid"} : i1
    %133 = comb.xor bin %106, %true {sv.namehint = "_io_nasti_ar_valid_T_1"} : i1
    %134 = comb.xor %128, %true : i1
    %135 = comb.and %134, %133 : i1
    %136 = comb.mux bin %128, %c0_i3, %125 : i3
    %137 = comb.icmp bin eq %state, %c3_i3 : i3
    %138 = comb.or bin %107, %112, %127 : i1
    %139 = comb.xor %138, %true : i1
    %140 = comb.and %139, %137 {sv.namehint = "io_nasti_w_valid"} : i1
    %141 = comb.mux bin %30, %c-4_i3, %state : i3
    %142 = comb.icmp bin eq %state, %c-4_i3 : i3
    %143 = comb.or bin %138, %137 : i1
    %144 = comb.xor %143, %true : i1
    %145 = comb.and %144, %142 {sv.namehint = "io_nasti_b_ready"} : i1
    %146 = comb.and bin %145, %io_nasti_b_valid : i1
    %147 = comb.mux bin %146, %c-3_i3, %state : i3
    %148 = comb.icmp bin eq %state, %c-3_i3 : i3
    %149 = comb.or bin %137, %142 : i1
    %150 = comb.xor %149, %true : i1
    %151 = comb.and %150, %148 : i1
    %152 = comb.mux bin %127, %135, %151 : i1
    %153 = comb.mux bin %112, %121, %152 : i1
    %154 = comb.and %31, %153 {sv.namehint = "io_nasti_ar_valid"} : i1
    %155 = comb.icmp bin eq %state, %c-2_i3 : i3
    %156 = comb.concat %false, %65, %false : i1, i1, i1
    %157 = comb.and bin %155, %26 : i1
    %158 = comb.mux bin %157, %156, %state : i3
    %159 = hw.array_create %158, %158, %124, %147, %141, %136, %126, %158 : i3
    %160 = hw.array_get %159[%state] : !hw.array<8xi3>, i3
    %161 = comb.mux bin %31, %160, %111 : i3
    hw.output %68, %63, %132, %99, %140, %102, %30, %145, %154, %93, %35 : i1, i32, i1, i32, i1, i64, i1, i1, i1, i32, i1
  }
  hw.module private @MemArbiter(in %clock : !seq.clock, in %reset : i1, out io_icache_ar_ready : i1, in %io_icache_ar_valid : i1, in %io_icache_ar_bits_addr : i32, in %io_icache_r_ready : i1, out io_icache_r_valid : i1, out io_icache_r_bits_data : i64, out io_dcache_aw_ready : i1, in %io_dcache_aw_valid : i1, in %io_dcache_aw_bits_addr : i32, out io_dcache_w_ready : i1, in %io_dcache_w_valid : i1, in %io_dcache_w_bits_data : i64, in %io_dcache_w_bits_last : i1, in %io_dcache_b_ready : i1, out io_dcache_b_valid : i1, out io_dcache_ar_ready : i1, in %io_dcache_ar_valid : i1, in %io_dcache_ar_bits_addr : i32, in %io_dcache_r_ready : i1, out io_dcache_r_valid : i1, out io_dcache_r_bits_data : i64, in %io_nasti_aw_ready : i1, out io_nasti_aw_valid : i1, out io_nasti_aw_bits_addr : i32, in %io_nasti_w_ready : i1, out io_nasti_w_valid : i1, out io_nasti_w_bits_data : i64, out io_nasti_w_bits_last : i1, out io_nasti_b_ready : i1, in %io_nasti_b_valid : i1, in %io_nasti_ar_ready : i1, out io_nasti_ar_valid : i1, out io_nasti_ar_bits_addr : i32, out io_nasti_r_ready : i1, in %io_nasti_r_valid : i1, in %io_nasti_r_bits_data : i64, in %io_nasti_r_bits_last : i1) {
    %c0_i3 = hw.constant 0 : i3
    %c3_i3 = hw.constant 3 : i3
    %c2_i3 = hw.constant 2 : i3
    %c1_i3 = hw.constant 1 : i3
    %true = hw.constant true
    %c-4_i3 = hw.constant -4 : i3
    %state = seq.firreg %43 clock %clock reset sync %reset, %c0_i3 {firrtl.random_init_start = 0 : ui64} : i3
    %0 = comb.icmp bin ne %state, %c0_i3 : i3
    %1 = comb.xor bin %0, %true {sv.namehint = "_io_dcache_ar_ready_T_2"} : i1
    %2 = comb.and bin %io_dcache_aw_valid, %1 {sv.namehint = "io_nasti_aw_valid"} : i1
    %3 = comb.and bin %io_nasti_aw_ready, %1 {sv.namehint = "io_dcache_aw_ready"} : i1
    %4 = comb.icmp bin eq %state, %c3_i3 {sv.namehint = "_io_dcache_w_ready_T"} : i3
    %5 = comb.and bin %io_dcache_w_valid, %4 {sv.namehint = "io_nasti_w_valid"} : i1
    %6 = comb.and bin %io_nasti_w_ready, %4 {sv.namehint = "io_dcache_w_ready"} : i1
    %7 = comb.icmp bin eq %state, %c-4_i3 {sv.namehint = "_io_nasti_b_ready_T"} : i3
    %8 = comb.and bin %io_nasti_b_valid, %7 {sv.namehint = "io_dcache_b_valid"} : i1
    %9 = comb.and bin %io_dcache_b_ready, %7 {sv.namehint = "io_nasti_b_ready"} : i1
    %10 = comb.mux bin %io_dcache_ar_valid, %io_dcache_ar_bits_addr, %io_icache_ar_bits_addr {sv.namehint = "io_nasti_ar_bits_aw_addr"} : i32
    %11 = comb.or bin %io_icache_ar_valid, %io_dcache_ar_valid {sv.namehint = "_io_nasti_ar_valid_T"} : i1
    %12 = comb.xor bin %2, %true {sv.namehint = "_io_dcache_ar_ready_T"} : i1
    %13 = comb.and bin %11, %12, %1 {sv.namehint = "io_nasti_ar_valid"} : i1
    %14 = comb.and bin %io_nasti_ar_ready, %12, %1 {sv.namehint = "io_dcache_ar_ready"} : i1
    %15 = comb.xor bin %io_dcache_ar_valid, %true {sv.namehint = "_io_icache_ar_ready_T"} : i1
    %16 = comb.and bin %14, %15 {sv.namehint = "io_icache_ar_ready"} : i1
    %17 = comb.icmp bin eq %state, %c1_i3 {sv.namehint = "_io_nasti_r_ready_T"} : i3
    %18 = comb.and bin %io_nasti_r_valid, %17 {sv.namehint = "io_icache_r_valid"} : i1
    %19 = comb.icmp bin eq %state, %c2_i3 {sv.namehint = "_io_nasti_r_ready_T_2"} : i3
    %20 = comb.and bin %io_nasti_r_valid, %19 {sv.namehint = "io_dcache_r_valid"} : i1
    %21 = comb.and bin %io_icache_r_ready, %17 {sv.namehint = "_io_nasti_r_ready_T_1"} : i1
    %22 = comb.and bin %io_dcache_r_ready, %19 {sv.namehint = "_io_nasti_r_ready_T_3"} : i1
    %23 = comb.or bin %21, %22 {sv.namehint = "io_nasti_r_ready"} : i1
    %24 = comb.and bin %3, %io_dcache_aw_valid : i1
    %25 = comb.and bin %14, %io_dcache_ar_valid : i1
    %26 = comb.and bin %16, %io_icache_ar_valid : i1
    %27 = comb.mux bin %26, %c1_i3, %state : i3
    %28 = comb.mux bin %25, %c2_i3, %27 : i3
    %29 = comb.mux bin %24, %c3_i3, %28 : i3
    %30 = comb.icmp bin eq %state, %c1_i3 : i3
    %31 = comb.and bin %23, %io_nasti_r_valid, %io_nasti_r_bits_last : i1
    %32 = comb.mux bin %31, %c0_i3, %state : i3
    %33 = comb.icmp bin eq %state, %c2_i3 : i3
    %34 = comb.icmp bin eq %state, %c3_i3 : i3
    %35 = comb.and bin %6, %io_dcache_w_valid, %io_dcache_w_bits_last : i1
    %36 = comb.mux bin %35, %c-4_i3, %state : i3
    %37 = comb.icmp bin eq %state, %c-4_i3 : i3
    %38 = comb.and bin %37, %9, %io_nasti_b_valid : i1
    %39 = comb.mux bin %38, %c0_i3, %state : i3
    %40 = comb.mux bin %34, %36, %39 : i3
    %41 = comb.or %30, %33 : i1
    %42 = comb.mux bin %41, %32, %40 : i3
    %43 = comb.mux bin %0, %42, %29 : i3
    hw.output %16, %18, %io_nasti_r_bits_data, %3, %6, %8, %14, %20, %io_nasti_r_bits_data, %2, %io_dcache_aw_bits_addr, %5, %io_dcache_w_bits_data, %io_dcache_w_bits_last, %9, %13, %10, %23 : i1, i1, i64, i1, i1, i1, i1, i1, i64, i1, i32, i1, i64, i1, i1, i1, i32, i1
  }
  hw.module @Tile(in %clock : !seq.clock, in %reset : i1, in %io_host_fromhost_valid : i1, in %io_host_fromhost_bits : i32, out io_host_tohost : i32, in %io_nasti_aw_ready : i1, out io_nasti_aw_valid : i1, out io_nasti_aw_bits_id : i5, out io_nasti_aw_bits_addr : i32, out io_nasti_aw_bits_len : i8, out io_nasti_aw_bits_size : i3, out io_nasti_aw_bits_burst : i2, out io_nasti_aw_bits_lock : i1, out io_nasti_aw_bits_cache : i4, out io_nasti_aw_bits_prot : i3, out io_nasti_aw_bits_qos : i4, in %io_nasti_w_ready : i1, out io_nasti_w_valid : i1, out io_nasti_w_bits_data : i64, out io_nasti_w_bits_strb : i8, out io_nasti_w_bits_last : i1, out io_nasti_b_ready : i1, in %io_nasti_b_valid : i1, in %io_nasti_b_bits_id : i5, in %io_nasti_b_bits_resp : i2, in %io_nasti_ar_ready : i1, out io_nasti_ar_valid : i1, out io_nasti_ar_bits_id : i5, out io_nasti_ar_bits_addr : i32, out io_nasti_ar_bits_len : i8, out io_nasti_ar_bits_size : i3, out io_nasti_ar_bits_burst : i2, out io_nasti_ar_bits_lock : i1, out io_nasti_ar_bits_cache : i4, out io_nasti_ar_bits_prot : i3, out io_nasti_ar_bits_qos : i4, out io_nasti_r_ready : i1, in %io_nasti_r_valid : i1, in %io_nasti_r_bits_id : i5, in %io_nasti_r_bits_data : i64, in %io_nasti_r_bits_resp : i2, in %io_nasti_r_bits_last : i1) {
    %c0_i32 = hw.constant 0 : i32
    %c-1_i8 = hw.constant -1 : i8
    %c0_i5 = hw.constant 0 : i5
    %c1_i8 = hw.constant 1 : i8
    %c3_i3 = hw.constant 3 : i3
    %c1_i2 = hw.constant 1 : i2
    %false = hw.constant false
    %c0_i3 = hw.constant 0 : i3
    %c0_i4 = hw.constant 0 : i4
    %core.io_host_tohost, %core.io_icache_req_valid, %core.io_icache_req_bits_addr, %core.io_dcache_abort, %core.io_dcache_req_valid, %core.io_dcache_req_bits_addr, %core.io_dcache_req_bits_data, %core.io_dcache_req_bits_mask = hw.instance "core" @Core(clock: %clock: !seq.clock, reset: %reset: i1, io_host_fromhost_valid: %io_host_fromhost_valid: i1, io_host_fromhost_bits: %io_host_fromhost_bits: i32, io_icache_resp_valid: %icache.io_cpu_resp_valid: i1, io_icache_resp_bits_data: %icache.io_cpu_resp_bits_data: i32, io_dcache_resp_valid: %dcache.io_cpu_resp_valid: i1, io_dcache_resp_bits_data: %dcache.io_cpu_resp_bits_data: i32) -> (io_host_tohost: i32, io_icache_req_valid: i1, io_icache_req_bits_addr: i32, io_dcache_abort: i1, io_dcache_req_valid: i1, io_dcache_req_bits_addr: i32, io_dcache_req_bits_data: i32, io_dcache_req_bits_mask: i4) {sv.namehint = "io_host_tohost"}
    %icache.io_cpu_resp_valid, %icache.io_cpu_resp_bits_data, %icache.io_nasti_aw_valid, %icache.io_nasti_aw_bits_addr, %icache.io_nasti_w_valid, %icache.io_nasti_w_bits_data, %icache.io_nasti_w_bits_last, %icache.io_nasti_b_ready, %icache.io_nasti_ar_valid, %icache.io_nasti_ar_bits_addr, %icache.io_nasti_r_ready = hw.instance "icache" @Cache(clock: %clock: !seq.clock, reset: %reset: i1, io_cpu_abort: %false: i1, io_cpu_req_valid: %core.io_icache_req_valid: i1, io_cpu_req_bits_addr: %core.io_icache_req_bits_addr: i32, io_cpu_req_bits_data: %c0_i32: i32, io_cpu_req_bits_mask: %c0_i4: i4, io_nasti_aw_ready: %false: i1, io_nasti_w_ready: %false: i1, io_nasti_b_valid: %false: i1, io_nasti_ar_ready: %arb.io_icache_ar_ready: i1, io_nasti_r_valid: %arb.io_icache_r_valid: i1, io_nasti_r_bits_data: %arb.io_icache_r_bits_data: i64) -> (io_cpu_resp_valid: i1, io_cpu_resp_bits_data: i32, io_nasti_aw_valid: i1, io_nasti_aw_bits_addr: i32, io_nasti_w_valid: i1, io_nasti_w_bits_data: i64, io_nasti_w_bits_last: i1, io_nasti_b_ready: i1, io_nasti_ar_valid: i1, io_nasti_ar_bits_addr: i32, io_nasti_r_ready: i1)
    %dcache.io_cpu_resp_valid, %dcache.io_cpu_resp_bits_data, %dcache.io_nasti_aw_valid, %dcache.io_nasti_aw_bits_addr, %dcache.io_nasti_w_valid, %dcache.io_nasti_w_bits_data, %dcache.io_nasti_w_bits_last, %dcache.io_nasti_b_ready, %dcache.io_nasti_ar_valid, %dcache.io_nasti_ar_bits_addr, %dcache.io_nasti_r_ready = hw.instance "dcache" @Cache(clock: %clock: !seq.clock, reset: %reset: i1, io_cpu_abort: %core.io_dcache_abort: i1, io_cpu_req_valid: %core.io_dcache_req_valid: i1, io_cpu_req_bits_addr: %core.io_dcache_req_bits_addr: i32, io_cpu_req_bits_data: %core.io_dcache_req_bits_data: i32, io_cpu_req_bits_mask: %core.io_dcache_req_bits_mask: i4, io_nasti_aw_ready: %arb.io_dcache_aw_ready: i1, io_nasti_w_ready: %arb.io_dcache_w_ready: i1, io_nasti_b_valid: %arb.io_dcache_b_valid: i1, io_nasti_ar_ready: %arb.io_dcache_ar_ready: i1, io_nasti_r_valid: %arb.io_dcache_r_valid: i1, io_nasti_r_bits_data: %arb.io_dcache_r_bits_data: i64) -> (io_cpu_resp_valid: i1, io_cpu_resp_bits_data: i32, io_nasti_aw_valid: i1, io_nasti_aw_bits_addr: i32, io_nasti_w_valid: i1, io_nasti_w_bits_data: i64, io_nasti_w_bits_last: i1, io_nasti_b_ready: i1, io_nasti_ar_valid: i1, io_nasti_ar_bits_addr: i32, io_nasti_r_ready: i1)
    %arb.io_icache_ar_ready, %arb.io_icache_r_valid, %arb.io_icache_r_bits_data, %arb.io_dcache_aw_ready, %arb.io_dcache_w_ready, %arb.io_dcache_b_valid, %arb.io_dcache_ar_ready, %arb.io_dcache_r_valid, %arb.io_dcache_r_bits_data, %arb.io_nasti_aw_valid, %arb.io_nasti_aw_bits_addr, %arb.io_nasti_w_valid, %arb.io_nasti_w_bits_data, %arb.io_nasti_w_bits_last, %arb.io_nasti_b_ready, %arb.io_nasti_ar_valid, %arb.io_nasti_ar_bits_addr, %arb.io_nasti_r_ready = hw.instance "arb" @MemArbiter(clock: %clock: !seq.clock, reset: %reset: i1, io_icache_ar_valid: %icache.io_nasti_ar_valid: i1, io_icache_ar_bits_addr: %icache.io_nasti_ar_bits_addr: i32, io_icache_r_ready: %icache.io_nasti_r_ready: i1, io_dcache_aw_valid: %dcache.io_nasti_aw_valid: i1, io_dcache_aw_bits_addr: %dcache.io_nasti_aw_bits_addr: i32, io_dcache_w_valid: %dcache.io_nasti_w_valid: i1, io_dcache_w_bits_data: %dcache.io_nasti_w_bits_data: i64, io_dcache_w_bits_last: %dcache.io_nasti_w_bits_last: i1, io_dcache_b_ready: %dcache.io_nasti_b_ready: i1, io_dcache_ar_valid: %dcache.io_nasti_ar_valid: i1, io_dcache_ar_bits_addr: %dcache.io_nasti_ar_bits_addr: i32, io_dcache_r_ready: %dcache.io_nasti_r_ready: i1, io_nasti_aw_ready: %io_nasti_aw_ready: i1, io_nasti_w_ready: %io_nasti_w_ready: i1, io_nasti_b_valid: %io_nasti_b_valid: i1, io_nasti_ar_ready: %io_nasti_ar_ready: i1, io_nasti_r_valid: %io_nasti_r_valid: i1, io_nasti_r_bits_data: %io_nasti_r_bits_data: i64, io_nasti_r_bits_last: %io_nasti_r_bits_last: i1) -> (io_icache_ar_ready: i1, io_icache_r_valid: i1, io_icache_r_bits_data: i64, io_dcache_aw_ready: i1, io_dcache_w_ready: i1, io_dcache_b_valid: i1, io_dcache_ar_ready: i1, io_dcache_r_valid: i1, io_dcache_r_bits_data: i64, io_nasti_aw_valid: i1, io_nasti_aw_bits_addr: i32, io_nasti_w_valid: i1, io_nasti_w_bits_data: i64, io_nasti_w_bits_last: i1, io_nasti_b_ready: i1, io_nasti_ar_valid: i1, io_nasti_ar_bits_addr: i32, io_nasti_r_ready: i1) {sv.namehint = "io_nasti_r_ready"}
    hw.output %core.io_host_tohost, %arb.io_nasti_aw_valid, %c0_i5, %arb.io_nasti_aw_bits_addr, %c1_i8, %c3_i3, %c1_i2, %false, %c0_i4, %c0_i3, %c0_i4, %arb.io_nasti_w_valid, %arb.io_nasti_w_bits_data, %c-1_i8, %arb.io_nasti_w_bits_last, %arb.io_nasti_b_ready, %arb.io_nasti_ar_valid, %c0_i5, %arb.io_nasti_ar_bits_addr, %c1_i8, %c3_i3, %c1_i2, %false, %c0_i4, %c0_i3, %c0_i4, %arb.io_nasti_r_ready : i32, i1, i5, i32, i8, i3, i2, i1, i4, i3, i4, i1, i64, i8, i1, i1, i1, i5, i32, i8, i3, i2, i1, i4, i3, i4, i1
  }
}