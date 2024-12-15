from enum import Enum, auto
import ctypes
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
  io = [Signal("a", 0, 3, SignalType.INPUT),
        Signal("b", 1, 4, SignalType.INPUT),
        Signal("sel", 2, 2, SignalType.INPUT),
        Signal("out", 3, 3, SignalType.OUTPUT)]


class mux_2to1View:
  class __io_in_op_uint8:
    def __init__(self, ptr):
      self.__ptr = ptr

    def poke(self, data: int):
      self.__ptr.contents = ctypes.c_uint8(data)

    def peek(self):
      return self.__ptr.contents.value

  class __io_out_op_uint8:
    def __init__(self, ptr):
      self.__ptr = ptr

    def poke(self, data: int):
      raise TypeError("This port is read-only")

    def peek(self):
      return self.__ptr.contents.value

  def __init__(self):
    self.storage = ctypes.create_string_buffer(4)
    self.a = self.__io_in_op_uint8(ctypes.cast(ctypes.byref(self.storage, 0),
                                               ctypes.POINTER(ctypes.c_uint8)))
    self.b = self.__io_in_op_uint8(ctypes.cast(ctypes.byref(self.storage, 1),
                                               ctypes.POINTER(ctypes.c_uint8)))
    self.sel = self.__io_in_op_uint8(ctypes.cast(ctypes.byref(self.storage, 2),
                                                 ctypes.POINTER(ctypes.c_uint8)))
    self.out = self.__io_out_op_uint8(ctypes.cast(ctypes.byref(self.storage, 3),
                                                  ctypes.POINTER(ctypes.c_uint8)))


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
    self.view = mux_2to1View()

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

  def eval(self):
    self.__eval_func(self.eval_param)


if __name__ == "__main__":
  dut = mux_2to1()
  dut.view.a.poke(3)
  dut.view.b.poke(5)
  dut.view.sel.poke(2)
  dut.eval()
  print(dut.view.a.peek())
  print(dut.view.b.peek())
  print(dut.view.sel.peek())
  print(dut.view.out.peek())
