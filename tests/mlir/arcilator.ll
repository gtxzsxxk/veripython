; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"

define void @Top_eval(ptr %0) {
  %2 = load i1, ptr %0, align 1
  %3 = getelementptr i8, ptr %0, i32 3
  %4 = load i1, ptr %3, align 1
  store i1 %2, ptr %3, align 1
  %5 = xor i1 %4, %2
  %6 = and i1 %5, %2
  br i1 %6, label %7, label %18

7:                                                ; preds = %1
  %8 = getelementptr i8, ptr %0, i32 1
  %9 = load i4, ptr %8, align 1
  %10 = getelementptr i8, ptr %0, i32 2
  %11 = load i4, ptr %10, align 1
  %12 = add i4 %9, %11
  %13 = xor i4 %12, %9
  %14 = getelementptr i8, ptr %0, i32 4
  store i4 %13, ptr %14, align 1
  %15 = load i4, ptr %10, align 1
  %16 = xor i4 %12, %15
  %17 = getelementptr i8, ptr %0, i32 5
  store i4 %16, ptr %17, align 1
  br label %18

18:                                               ; preds = %7, %1
  %19 = getelementptr i8, ptr %0, i32 4
  %20 = load i4, ptr %19, align 1
  %21 = getelementptr i8, ptr %0, i32 5
  %22 = load i4, ptr %21, align 1
  %23 = mul i4 %20, %22
  %24 = getelementptr i8, ptr %0, i32 6
  store i4 %23, ptr %24, align 1
  ret void
}

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}