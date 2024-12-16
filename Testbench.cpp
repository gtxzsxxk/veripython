//
// Created by hanyuan on 2024/12/16.
//

#include "Testbench.h"
#include "Exceptions.h"
#include <sstream>

static const char *pytb_basic = "from enum import Enum, auto\n"
                                "from vcd import VCDWriter\n"
                                "import ctypes\n"
                                "import llvmlite\n"
                                "\n"
                                "llvmlite.opaque_pointers_enabled = True\n"
                                "import llvmlite.binding as llvm\n"
                                "\n"
                                "\n"
                                "class SignalType(Enum):\n"
                                "  INPUT = auto()\n"
                                "  OUTPUT = auto()\n"
                                "  REGISTER = auto()\n"
                                "  MEMORY = auto()\n"
                                "  WIRE = auto()\n"
                                "\n"
                                "\n"
                                "class Signal:\n"
                                "  def __init__(self, name: str, offset: int, num_bits: int, signal_type: SignalType):\n"
                                "    self.name = name\n"
                                "    self.offset = offset\n"
                                "    self.num_bits = num_bits\n"
                                "    self.type = signal_type\n";

/* Copied from lib/Dialect/Arc/ModelInfo.cpp */
Testbench::Testbench(mlir::ModuleOp &moduleOp) {
    llvm::SmallVector<circt::arc::ModelInfo> models;
    if (mlir::failed(circt::arc::collectModels(moduleOp, models))) {
        throw CircuitException("Cannot collect memory layout from Arc model");
    }
    for (const circt::arc::ModelInfo &model: models) {
        modelInfos.push_back(model);
    }
}

std::string Testbench::emitPythonModule(const std::string &ir) {
    std::stringstream out;
    out << pytb_basic << "\n";

    auto model = modelInfos[0];
    out << "class " << model.name << "Layout:\n";
    out << "  name = \"" << model.name << "\"\n";
    out << "  num_states = \"" << model.states.size() << "\"\n";
    out << "  num_state_bytes = \"" << model.numStateBytes << "\"\n";
    out << "  io = [\n";
    for (const auto &signal: model.states) {
        out << "    Signal(\"" << signal.name << "\", " << signal.offset << ", "
            << signal.numBits << ", SignalType." <<
            ((signal.type == circt::arc::StateInfo::Type::Input) ? "INPUT" :
             ((signal.type == circt::arc::StateInfo::Type::Output) ? "OUTPUT" :
              ((signal.type == circt::arc::StateInfo::Type::Register) ? "REGISTER" :
               ((signal.type == circt::arc::StateInfo::Type::Memory) ? "MEMORY" : "WIRE"))))
            << "),\n";
    }
    out << "  ]\n\n";

    out << "class " << model.name << "View:\n";
    out << "  class __io_in_:\n"
           "    def __init__(self, ptr, offset, bits):\n"
           "      self.__ptr = ptr\n"
           "      self.__offset = offset\n"
           "      self.__bits = bits\n"
           "\n"
           "    # Data is little endian\n"
           "    def poke(self, data: int):\n"
           "      cur_pos = 0\n"
           "      while True:\n"
           "        self.__ptr[self.__offset + cur_pos] = (data >> (cur_pos * 8)) & 0xff\n"
           "        cur_pos += 1\n"
           "        if cur_pos * 8 >= self.__bits:\n"
           "          break\n"
           "\n"
           "    def peek(self):\n"
           "      data = 0\n"
           "      cur_pos = 0\n"
           "      while True:\n"
           "        data |= (self.__ptr[self.__offset + cur_pos] << (cur_pos * 8))\n"
           "        cur_pos += 1\n"
           "        if cur_pos * 8 >= self.__bits:\n"
           "          break\n"
           "      return data\n"
           "\n"
           "  class __io_out_:\n"
           "    def __init__(self, ptr, offset, bits):\n"
           "      self.__ptr = ptr\n"
           "      self.__offset = offset\n"
           "      self.__bits = bits\n"
           "\n"
           "    def poke(self, data: int):\n"
           "      raise TypeError(\"This port is read-only\")\n"
           "\n"
           "    def peek(self):\n"
           "      data = 0\n"
           "      cur_pos = 0\n"
           "      while True:\n"
           "        data |= (self.__ptr[self.__offset + cur_pos] << (cur_pos * 8))\n"
           "        cur_pos += 1\n"
           "        if cur_pos * 8 >= self.__bits:\n"
           "          break\n"
           "      return data\n\n";

    out << "  def __init__(self):\n"
        << "    self.storage = ctypes.create_string_buffer(" << model.numStateBytes << ")\n";

    for (const auto &signal: model.states) {
        if (!((signal.type == circt::arc::StateInfo::Type::Input) ||
              (signal.type == circt::arc::StateInfo::Type::Output))) {
            continue;
        }
        out << "    self." << signal.name << " = ";
        if (signal.type == circt::arc::StateInfo::Type::Input) {
            out << "self.__io_in_";
        } else {
            out << "self.__io_out_";
        }
        out << "(ctypes.cast(ctypes.pointer(self.storage),\n"
               "                                       ctypes.POINTER(ctypes.c_uint8)), "
            << signal.offset << ", " << signal.numBits << ")\n";
    }

    out << "\n\n";

    out << "class " << model.name << ":\n"
        << "  llvm_ir = \"\"\"\n" << ir << "\n    \"\"\"\n\n";

    out << "  def __init__(self, vcd_enabled=False):\n"
           "    if int(llvmlite.__version__.split('.')[1]) < 44:\n"
           "      raise ImportError(\"The version of llvmlite must greater than or equal to 0.44\")\n";
    out << "    self.view = " << model.name << "View()\n";
    out << "    self.layout = " << model.name << "Layout()\n";
    out << "    self.vcd_enabled = vcd_enabled\n\n"
           "    llvm.initialize()\n"
           "    llvm.initialize_native_target()\n"
           "    llvm.initialize_native_asmprinter()\n"
           "\n"
           "    def create_execution_engine():\n"
           "      \"\"\"\n"
           "      Create an ExecutionEngine suitable for JIT code generation on\n"
           "      the host CPU.  The engine is reusable for an arbitrary number of\n"
           "      modules.\n"
           "      \"\"\"\n"
           "      # Create a target machine representing the host\n"
           "      target = llvm.Target.from_default_triple()\n"
           "      target_machine = target.create_target_machine()\n"
           "      # And an execution engine with an empty backing module\n"
           "      backing_mod = llvm.parse_assembly(\"\")\n"
           "      eng = llvm.create_mcjit_compiler(backing_mod, target_machine)\n"
           "      return eng\n"
           "\n"
           "    def compile_ir(engine_, llvm_ir_):\n"
           "      \"\"\"\n"
           "        Compile the LLVM IR string with the given engine.\n"
           "        The compiled module object is returned.\n"
           "        \"\"\"\n"
           "      # Create a LLVM module object from the IR\n"
           "      mod_ = llvm.parse_assembly(llvm_ir_)\n"
           "      mod_.verify()\n"
           "      # Now add the module and make sure it is ready for execution\n"
           "      engine_.add_module(mod_)\n"
           "      engine_.finalize_object()\n"
           "      engine_.run_static_constructors()\n"
           "      return mod_\n"
           "\n"
           "    self.engine = create_execution_engine()\n"
           "    self.mod = compile_ir(self.engine, self.llvm_ir)\n"
           "\n"
           "    # Look up the function pointer (a Python int)\n";
    out << "    self.func_ptr = self.engine.get_function_address(\"" << model.name << "_eval\")\n";
    out << "    # Run the function via ctypes\n"
           "    self.__eval_func = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_uint8))(self.func_ptr)\n"
           "    self.eval_param = ctypes.cast(ctypes.byref(self.view.storage, 0),\n"
           "                                  ctypes.POINTER(ctypes.c_uint8))\n"
           "\n"
           "    fp = open(self.layout.name + \"_dump.vcd\", \"w\")\n"
           "    self.vcd_writer = VCDWriter(fp, timescale='1 ns', date='today')\n"
           "    for sig in self.layout.io:\n"
           "      sig.vcd_var = self.vcd_writer.register_var(self.layout.name, sig.name, \"wire\", size=sig.num_bits)\n"
           "\n"
           "    self.timestamp = 0\n"
           "\n"
           "  def eval(self):\n"
           "    self.__eval_func(self.eval_param)\n"
           "    if self.vcd_enabled:\n"
           "      self.vcd_change()\n"
           "    self.timestamp += 1\n"
           "\n"
           "  def vcd_change(self):\n"
           "    for sig in self.layout.io:\n"
           "      self.vcd_writer.change(sig.vcd_var, self.timestamp, getattr(self.view, sig.name).peek())\n"
           "\n"
           "  def __del__(self):\n"
           "    self.vcd_writer.close()\n"
           "\n"
           "\n"
           "if __name__ == \"__main__\":\n"
           "  dut = ";

    out << model.name << "(True)\n";

    return out.str();
}
