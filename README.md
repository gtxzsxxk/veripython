# Veripython: another implementation for Verilator with Python target

VeriPython 是一个以 `Python` 为目标语言的 `Verilator` 实现。

Verilator 是一个开源的高性能 Verilog 和 SystemVerilog 模拟器，主要用于将硬件描述语言（HDL）编写的设计转换为可在软件中运行的代码，
从而对硬件设计进行仿真和验证。与传统的硬件仿真工具（比如 ModelSim、VCS 等）不同，Verilator 并不直接进行时序仿真，而是将 Verilog 
或 SystemVerilog 代码转换为 C++ 或 SystemC 代码，然后通过编译生成可执行文件来模拟硬件行为。这样做的好处是，仿真速度非常快，可以在
标准的计算机上运行，而不需要专用的硬件仿真设备。

与一般的EDA波形工具相比，Verilator 侧重于高效的功能仿真而非传统的波形生成。传统的波形工具通常用于查看电路的时序波形，以帮助调试设计中
的信号变化，而 Verilator 的目标更多是将硬件设计转化为软件可运行的模型，从而快速验证设计的功能是否正确。在许多场景下，Verilator 可以
与其他工具结合使用，进行更详细的时序分析和波形查看，但它本身不生成复杂的波形图，而是侧重于提供高效的仿真性能，适用于大规模的硬件设计验证
和功能验证。

作为编译原理课程的大作业，VeriPython 将实现一个 Verilog Parser（进行词法分析、语法分析）、电路前向传播计算图生成器（语义检查与电路生成）、
MLIR (LLVM) 生成器与 Arcilator (CIRCT (LLVM)) 中的 Python 后端。

小组分工如下：

- 词法分析、测试与电路可视化：杨憬晗
- 语法分析与 AST：徐浩然
- 语义检查与组合逻辑电路生成：杨之凡
- 语法分析、电路生成、时序逻辑与 LLVM：赵涵远

## 运行说明

## 实现

