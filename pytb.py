from enum import Enum, auto
from vcd import VCDWriter
import ctypes
import llvmlite

llvmlite.opaque_pointers_enabled = True
import llvmlite.binding as llvm


class SignalType(Enum):
  INPUT = auto()
  OUTPUT = auto()
  REGISTER = auto()
  MEMORY = auto()
  WIRE = auto()


class Signal:
  def __init__(self, name: str, offset: int, num_bits: int, signal_type: SignalType):
    self.name = name
    self.offset = offset
    self.num_bits = num_bits
    self.type = signal_type


class mux_2to1Layout:
  name = "mux_2to1"
  num_states = 4
  num_state_bytes = 4
  io = [
    Signal("a", 0, 3, SignalType.INPUT),
    Signal("b", 1, 4, SignalType.INPUT),
    Signal("sel", 2, 2, SignalType.INPUT),
    Signal("out", 3, 3, SignalType.OUTPUT)
  ]


class mux_2to1View:
  class __io_in_:
    def __init__(self, ptr, offset, bits):
      self.__ptr = ptr
      self.__offset = offset
      self.__bits = bits

    # Data is little endian
    def poke(self, data: int):
      cur_pos = 0
      while True:
        self.__ptr[self.__offset + cur_pos] = (data >> (cur_pos * 8)) & 0xff
        cur_pos += 1
        if cur_pos * 8 >= self.__bits:
          break

    def peek(self):
      data = 0
      cur_pos = 0
      while True:
        data |= (self.__ptr[self.__offset + cur_pos] << (cur_pos * 8))
        cur_pos += 1
        if cur_pos * 8 >= self.__bits:
          break
      return data

  class __io_out_:
    def __init__(self, ptr, offset, bits):
      self.__ptr = ptr
      self.__offset = offset
      self.__bits = bits

    def poke(self, data: int):
      raise TypeError("This port is read-only")

    def peek(self):
      data = 0
      cur_pos = 0
      while True:
        data |= (self.__ptr[self.__offset + cur_pos] << (cur_pos * 8))
        cur_pos += 1
        if cur_pos * 8 >= self.__bits:
          break
      return data

  def __init__(self):
    self.storage = ctypes.create_string_buffer(4)
    self.a = self.__io_in_(ctypes.cast(ctypes.pointer(self.storage),
                                       ctypes.POINTER(ctypes.c_uint8)), 0, 3)
    self.b = self.__io_in_(ctypes.cast(ctypes.pointer(self.storage),
                                       ctypes.POINTER(ctypes.c_uint8)), 1, 4)
    self.sel = self.__io_in_(ctypes.cast(ctypes.pointer(self.storage),
                                         ctypes.POINTER(ctypes.c_uint8)), 2, 2)
    self.out = self.__io_out_(ctypes.cast(ctypes.pointer(self.storage),
                                          ctypes.POINTER(ctypes.c_uint8)), 3, 3)


class mux_2to1:
  llvm_ir = """
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
    """

  def __init__(self):
    if int(llvmlite.__version__.split('.')[1]) < 44:
      raise ImportError("The version of llvmlite must greater than or equal to 0.44")

    self.view = mux_2to1View()
    self.layout = mux_2to1Layout()

    llvm.initialize()
    llvm.initialize_native_target()
    llvm.initialize_native_asmprinter()

    def create_execution_engine():
      """
      Create an ExecutionEngine suitable for JIT code generation on
      the host CPU.  The engine is reusable for an arbitrary number of
      modules.
      """
      # Create a target machine representing the host
      target = llvm.Target.from_default_triple()
      target_machine = target.create_target_machine()
      # And an execution engine with an empty backing module
      backing_mod = llvm.parse_assembly("")
      eng = llvm.create_mcjit_compiler(backing_mod, target_machine)
      return eng

    def compile_ir(engine_, llvm_ir_):
      """
        Compile the LLVM IR string with the given engine.
        The compiled module object is returned.
        """
      # Create a LLVM module object from the IR
      mod_ = llvm.parse_assembly(llvm_ir_)
      mod_.verify()
      # Now add the module and make sure it is ready for execution
      engine_.add_module(mod_)
      engine_.finalize_object()
      engine_.run_static_constructors()
      return mod_

    self.engine = create_execution_engine()
    self.mod = compile_ir(self.engine, self.llvm_ir)

    # Look up the function pointer (a Python int)
    self.func_ptr = self.engine.get_function_address("mux_2to1_eval")

    # Run the function via ctypes
    self.__eval_func = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_uint8))(self.func_ptr)
    self.eval_param = ctypes.cast(ctypes.byref(self.view.storage, 0),
                                  ctypes.POINTER(ctypes.c_uint8))

    fp = open(self.layout.name + "_dump.vcd", "w")
    self.vcd_writer = VCDWriter(fp, timescale='1 ns', date='today')
    for sig in self.layout.io:
      sig.vcd_var = self.vcd_writer.register_var(self.layout.name, sig.name, "wire", size=sig.num_bits)

    self.timestamp = 0

  def eval(self, vcd_enabled=False):
    self.__eval_func(self.eval_param)
    if vcd_enabled:
      self.vcd_change()
    self.timestamp += 1

  def vcd_change(self):
    for sig in self.layout.io:
      self.vcd_writer.change(sig.vcd_var, self.timestamp, getattr(self.view, sig.name).peek())

  def __del__(self):
    self.vcd_writer.close()


if __name__ == "__main__":
  dut = mux_2to1()
  dut.view.a.poke(4)
  dut.view.b.poke(5)
  dut.view.sel.poke(2)
  dut.eval(True)
  print(dut.view.out.peek())
  assert dut.view.out.peek() == 4
  dut.view.a.poke(5)
  dut.view.b.poke(7)
  dut.view.sel.poke(1)
  dut.eval(True)
  print(dut.view.out.peek())
  assert dut.view.out.peek() == 3
  dut.view.a.poke(5)
  dut.view.b.poke(7)
  dut.view.sel.poke(0)
  dut.eval(True)
  print(dut.view.out.peek())
  assert dut.view.out.peek() == 2
