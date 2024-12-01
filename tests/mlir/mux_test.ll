; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"

define void @mux_2to1_eval(ptr %0) {
  %2 = getelementptr i8, ptr %0, i32 2
  %3 = load i2, ptr %2, align 1
  %4 = getelementptr i8, ptr %0, i32 1
  %5 = load i4, ptr %4, align 1
  %6 = load i3, ptr %0, align 1
  %7 = trunc i2 %3 to i1
  %8 = lshr i4 %5, 1
  %9 = trunc i4 %8 to i3
  %10 = lshr i2 %3, 1
  %11 = trunc i2 %10 to i1
  %12 = select i1 %11, i3 %6, i3 2
  %13 = select i1 %7, i3 %9, i3 %12
  %14 = getelementptr i8, ptr %0, i32 3
  store i3 %13, ptr %14, align 1
  ret void
}

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}