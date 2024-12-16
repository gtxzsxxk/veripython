运行说明
======

本目录下的 veripython 二进制文件为 Linux 平台的可执行文件，
运行平台为 Ubuntu 22.04 amd64。

详细说明请参见作业 PDF 文档的编译与运行说明。

相比于前端部分的提交，由于侧重点不同，我们做了些许改动。
我们提供了三个测试文件，分别是 mux_test.v （复用器模块）
，reg_simple_test1.v （简单的计数器模块）与
birthday.v （生日密码锁模块）。

同时我们也附上 birthday.v 生成的 Python Testbench 模块。
该模块为 veripython 自动生成的产物，依赖于 llvmlite 与 pyvcd。
安装好依赖后，该 py 文件可以直接执行。

直接执行后，会输出一个 vcd 文件。该文件可以用 gtkwave 打开并
查看波形。

程序运行结果在 PDF 文档中也有完整呈现。
