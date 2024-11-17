运行说明
======

本目录下的 veripython 二进制文件为 Linux 平台的可执行文件，
运行平台为 Ubuntu amd64。

详细说明请参见作业 PDF 文档的编译与运行说明。

我们提供了两个测试文件，分别是 full_adder.v （1位全加器模块）
与 reg_simple_test2.v （简单的计数器模块）。

词法分析输出的 token 流分别为 full_adder_token.json 与
reg_simple_test2_token.json

我们的前端进行解析后输出的 AST 分别为 full_adder.json 与
reg_simple_test2.json。

由于 AST 阅读上较为困难，因此我们也附上了生成的 RTL 电路图，用于
辅助。分别为 full_adder.png 与 reg_simple_test2.png。
