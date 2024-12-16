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

class birthday_lockerLayout:
  name = "birthday_locker"
  num_states = "5"
  num_state_bytes = "16"
  io = [
    Signal("clk", 0, 1, SignalType.INPUT),
    Signal("rst_n", 1, 1, SignalType.INPUT),
    Signal("key", 2, 10, SignalType.INPUT),
    Signal("unlocked", 6, 1, SignalType.OUTPUT),
    Signal("status", 8, 64, SignalType.OUTPUT),
  ]

class birthday_lockerView:
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
    self.storage = ctypes.create_string_buffer(16)
    self.clk = self.__io_in_(ctypes.cast(ctypes.pointer(self.storage),
                                       ctypes.POINTER(ctypes.c_uint8)), 0, 1)
    self.rst_n = self.__io_in_(ctypes.cast(ctypes.pointer(self.storage),
                                       ctypes.POINTER(ctypes.c_uint8)), 1, 1)
    self.key = self.__io_in_(ctypes.cast(ctypes.pointer(self.storage),
                                       ctypes.POINTER(ctypes.c_uint8)), 2, 10)
    self.unlocked = self.__io_out_(ctypes.cast(ctypes.pointer(self.storage),
                                       ctypes.POINTER(ctypes.c_uint8)), 6, 1)
    self.status = self.__io_out_(ctypes.cast(ctypes.pointer(self.storage),
                                       ctypes.POINTER(ctypes.c_uint8)), 8, 64)


class birthday_locker:
  llvm_ir = """
  ; ModuleID = 'LLVMDialectModule'
  source_filename = "LLVMDialectModule"

  define void @birthday_locker_eval(ptr %0) {
    %2 = load i1, ptr %0, align 1
    %3 = getelementptr i8, ptr %0, i32 5
    %4 = load i1, ptr %3, align 1
    store i1 %2, ptr %3, align 1
    %5 = xor i1 %4, %2
    %6 = and i1 %5, %2
    br i1 %6, label %7, label %32

  7:                                                ; preds = %1
    %8 = getelementptr i8, ptr %0, i32 4
    %9 = load i4, ptr %8, align 1
    %10 = icmp eq i4 %9, 3
    %11 = getelementptr i8, ptr %0, i32 2
    %12 = load i10, ptr %11, align 2
    %13 = getelementptr i8, ptr %0, i32 1
    %14 = load i1, ptr %13, align 1
    %15 = icmp ne i10 %12, 0
    %16 = icmp eq i4 %9, 0
    %17 = lshr i10 %12, 9
    %18 = trunc i10 %17 to i1
    %19 = zext i1 %18 to i4
    %20 = icmp eq i4 %9, 1
    %21 = lshr i10 %12, 1
    %22 = trunc i10 %21 to i1
    %23 = select i1 %22, i4 2, i4 %19
    %24 = icmp eq i4 %9, 2
    %25 = select i1 %18, i4 3, i4 0
    %26 = select i1 %10, i4 %19, i4 %9
    %27 = select i1 %24, i4 %25, i4 %26
    %28 = select i1 %20, i4 %23, i4 %27
    %29 = select i1 %16, i4 %19, i4 %28
    %30 = select i1 %15, i4 %29, i4 %9
    %31 = select i1 %14, i4 %30, i4 0
    store i4 %31, ptr %8, align 1
    br label %32

  32:                                               ; preds = %7, %1
    %33 = getelementptr i8, ptr %0, i32 4
    %34 = load i4, ptr %33, align 1
    %35 = icmp eq i4 %34, 3
    %36 = getelementptr i8, ptr %0, i32 6
    store i1 %35, ptr %36, align 1
    %37 = select i1 %35, i64 6146934444722439492, i64 5498687660606357504
    %38 = getelementptr i8, ptr %0, i32 8
    store i64 %37, ptr %38, align 4
    ret void
  }

      """

  def __init__(self, vcd_enabled=False):
    if int(llvmlite.__version__.split('.')[1]) < 44:
      raise ImportError("The version of llvmlite must greater than or equal to 0.44")
    self.view = birthday_lockerView()
    self.layout = birthday_lockerLayout()
    self.vcd_enabled = vcd_enabled

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
    self.func_ptr = self.engine.get_function_address("birthday_locker_eval")
    # Run the function via ctypes
    self.__eval_func = ctypes.CFUNCTYPE(None, ctypes.POINTER(ctypes.c_uint8))(self.func_ptr)
    self.eval_param = ctypes.cast(ctypes.byref(self.view.storage, 0),
                                  ctypes.POINTER(ctypes.c_uint8))

    fp = open(self.layout.name + "_dump.vcd", "w")
    self.vcd_writer = VCDWriter(fp, timescale='1 ns', date='today')
    for sig in self.layout.io:
      sig.vcd_var = self.vcd_writer.register_var(self.layout.name, sig.name, "wire", size=sig.num_bits)

    self.timestamp = 0

  def eval(self):
    self.__eval_func(self.eval_param)
    if self.vcd_enabled:
      self.vcd_change()
    self.timestamp += 1

  def vcd_change(self):
    for sig in self.layout.io:
      self.vcd_writer.change(sig.vcd_var, self.timestamp, getattr(self.view, sig.name).peek())

  def __del__(self):
    self.vcd_writer.close()


if __name__ == "__main__":
  dut = birthday_locker(True)

  def digitToOneHot(digit):
    assert (0 <= digit <= 9)
    return 2 ** digit


  def doInputPerClock(digit):
    dut.view.clk.poke(0)
    dut.view.key.poke(digitToOneHot(digit))
    dut.eval()

    dut.view.clk.poke(1)
    dut.eval()


  # 复位
  dut.view.key.poke(0)
  dut.view.rst_n.poke(0)
  dut.view.clk.poke(0)
  dut.eval()
  dut.view.clk.poke(1)
  dut.eval()

  # 复位结束，电路开始工作
  dut.view.rst_n.poke(1)

  # 输入一串数字，观察什么时候开锁
  doInputPerClock(8)
  doInputPerClock(9)
  doInputPerClock(1)
  doInputPerClock(9)
  doInputPerClock(9)
  doInputPerClock(1)
  doInputPerClock(9)

  dut.view.key.poke(0)

  dut.view.clk.poke(0)
  dut.eval()
  dut.view.clk.poke(1)
  dut.eval()
  dut.view.clk.poke(0)
  dut.eval()
  dut.view.clk.poke(1)
  dut.eval()
  dut.view.clk.poke(0)
  dut.eval()
  dut.view.clk.poke(1)
  dut.eval()
