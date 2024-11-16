# Veripython: another implementation for Verilator with Python target

源语言：Verilog，目标语言：LLVM 与 Python。

Verilog 是一种硬件描述语言（HDL），与传统的通用编程语言（如 C、Python）有本质上的区别：

- 硬件描述：Verilog 用于描述电路而非软件逻辑，它表达的是硬件结构和行为。代码往往包含并行语义，而通用编程语言通常是顺序执行的。
- 时间和时序逻辑：Verilog 使用时钟和时序语句（如 always 块和 @(posedge clk) 等）来描述时序电路的行为。
通用编程语言没有时钟概念，也不会有类似的时序控制语句。
- 并行执行：Verilog 的描述是并行执行的，尤其是多个 always 块和模块实例之间是并发执行的，而传统编程语言往往依赖于函数调用和循环来进行顺序计算。
- 模块化设计：Verilog 有模块（module）这一关键语法结构，用于描述硬件模块，并可以嵌套和实例化模块，这是其主要的结构化特征。而通用编程语言通常使用函数和类来进行模块化。

VeriPython 是一个以 `Python` 为目标语言的 `Verilator` 实现。

Verilator 是一个开源的高性能 Verilog 和 SystemVerilog 模拟器，主要用于将硬件描述语言（HDL）编写的设计转换为可在软件中运行的代码，
从而对硬件设计进行仿真和验证。与传统的硬件仿真工具（比如 ModelSim、VCS 等）不同，Verilator 并不直接进行时序仿真，而是将 Verilog 
或 SystemVerilog 代码转换为 C++ 或 SystemC 代码，然后通过编译生成可执行文件来模拟硬件行为。这样做的好处是，仿真速度非常快，可以在
标准的计算机上运行，而不需要专用的硬件仿真设备。传统的波形工具通常用于查看电路的时序波形，以帮助调试设计中 的信号变化，
而 Verilator 的目标更多是将硬件设计转化为软件可运行的模型，从而快速验证设计的功能是否正确。在许多场景下，Verilator 可以
与其他工具结合使用，进行更详细的时序分析和波形查看，但它本身不生成复杂的波形图，而是侧重于提供高效的仿真性能，适用于大规模的硬件设计验证
和功能验证。

作为编译原理课程的大作业，VeriPython 将实现一个 Verilog Parser（进行词法分析、语法分析）、电路前向传播计算图生成器（语义检查与电路生成）、
MLIR (LLVM) 生成器与 Arcilator (CIRCT (LLVM)) 中的 Python 后端。
本项目不仅符合编译原理课程的要求（包括词法分析、语法分析、IR 生成和目标代码生成），还会深入探索硬件描述语言的编译过程，
这对编译器设计的理解具有很好的拓展性和研究价值。它能够结合我们对硬件与编译器的兴趣，展示出我们对这两个领域的交叉应用能力。

小组分工如下：

- 词法分析、测试与电路可视化：杨憬晗
- 语法分析与 AST：徐浩然
- 语义检查与组合逻辑电路生成：杨之凡
- 语法分析、电路生成、时序逻辑与 LLVM：赵涵远

## 编译与运行说明

目前提交的代码仅包含前端部分。支持的参数有：
- `-o`：指定输出文件名
- `-ast`：将语法分析树以`json`格式进行输出
- `-vis`：使用`graphviz`生成 HDL 的 RTL 视图到 `rtl_view.png`中

Example:
```bash
$ mkdir build && cd build
$ cmake ..
$ make -j
$ veripython ../tests/verilog_srcs/full_adder.v -o full_adder.json -ast -vis
```

## 实现

### 词法分析

为了解析 Verilog 的语法，我们使用了 `flex` （fast lexical analyzer generator，快速词法分析产生器）进行词法分析。
`flex` 应用广泛，从 `linux` 内核、`bash shell` 到 `MySQL` 的 `SQL` 解析器，都有 `flex` 的身影。
`flex` 自带最长匹配与最早匹配原则，非常便于进行一门语言的词法分析。
我们在 `verilog.l` 中定义了词法规则，使用具体的字符串与正则表达式（pcre2 规范）进行 token 的匹配，
一旦匹配到 `token`，就将对应的 `token id` 作为 `yylex` 函数的返回值。这些 `token id` 定义于 `Lexer.h` 中。

根据开发规范，`flex` 生成的 `lex.yy.c` 将在编译时链接到我们的程序中，并通过调用 `yylex` 函数获取下一个 token 的 id。
为了便于在代码中实现前瞻等特性，我还实现了一个 `token buffer`。由 `yylex` 解析出来的 `token` 都会加入 `token buffer` 中，
当 parser 进行前瞻时，直接返回 `token buffer` 的队首，当 parser 进行 eat 时，就 pop 掉队首元素。

### 语法分析

在编写语法分析有关的代码时，尽管已有 ANTLR、bison 等自动语法分析工具可供使用，我们仍然选择了与主流编译器前端相同的策略，也就是
手写递归下降器，这么做的原因如下。

对于电路描述语言，语法规则复杂多变，解析过程往往设计上下文敏感的信息，并且要想让实现具有高效性，就需要在语法分析阶段进行部分的语义检查，
例如在解析非阻塞赋值或者 `assign` 语句时，与其返回一颗语法分析树，不如直接将这一抽象的电路连接加入符号表中。手写递归下降器允许我们处理
一些复杂的语法或者特殊的语言结构，如果使用自动语法分析工具，它们通常要求给出的上下文无关文法非常清晰并且没有歧义，所以我们可能需要手动指定
拓展或者解决冲突，而手写递归下降器允许我们通过回溯或者前瞻更加灵活地处理语法规则。

在处理表达式解析时，相比于定义没有二义性的文法，更为便利的做法是根据运算符优先级进行解析。我们定义了运算符优先级表，根据优先级表，对 `primary`
进行“移入”或者“归约”。`primary` 为表达式中的单元，可以为identifier、常量，也可以为 `5'd3`这样的电路常量，也可以为括号包裹起来的表达式。
我们还修改了 HDL 的运算符优先级解析器，使其支持三元表达式。

```c++
/* 运算符优先级，数值越高越优先 */
std::unordered_map<VeriPythonTokens, int> Parser::operatorPrecedence = {
        {TOKEN_colon,          5},
        {TOKEN_question,       5},
        {TOKEN_logical_and,    20},
        {TOKEN_bitwise_or,     30},
        {TOKEN_bitwise_xor,    40},
        {TOKEN_bitwise_and,    50},
        {TOKEN_cond_eq,        60},
        {TOKEN_cond_ne,        60},
        {TOKEN_cond_lt,        70},
        {TOKEN_cond_le,        70},
        {TOKEN_cond_gt,        70},
        {TOKEN_cond_ge,        70},
        {TOKEN_arith_lshift,   80},
        {TOKEN_logical_lshift, 80},
        {TOKEN_arith_rshift,   80},
        {TOKEN_logical_rshift, 80},
        {TOKEN_op_add,         90},
        {TOKEN_op_sub,         90},
        {TOKEN_op_mod,         100},
        {TOKEN_op_mul,         100},
        {TOKEN_op_div,         100},
        {TOKEN_logical_not,    110},
        {TOKEN_bitwise_not,    110},
};
```

由于 verilog 支持对信号进行切片，例如 `a[3:2]` 就是取 identifier a 的第三根线与第二根线的信号，因此语法分析时也需要对这这一部分解析，
这也再次凸显了递归下降器的优势，只需要前瞻下一个 token 是否为 `[`，如果是，则执行 `parsePortSlicing`，并且将返回值加入电路连接。这一
过程涉及了语法分析也涉及了语义分析。

在设计 AST 时我们充分利用了 C++ 的面向对象特性，在基类中我们定义了虚函数 `toString`。这样只需要执行基类的函数，递归打印子树，
即可输出 AST。

### 语义分析与电路生成
