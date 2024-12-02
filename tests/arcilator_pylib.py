#!/usr/bin/env python3
import json
import re
from dataclasses import dataclass
from enum import Enum
from typing import *


# needed to support python3 older than 3.9
def removeprefix(text, prefix):
  if text.startswith(prefix):
    return text[len(prefix):]
  return text


# Read the JSON descriptor.
class StateType(Enum):
  INPUT = "input"
  OUTPUT = "output"
  REGISTER = "register"
  WIRE = "wire"
  MEMORY = "memory"


@dataclass
class StateInfo:
  name: Optional[str]
  offset: int
  numBits: int
  typ: StateType
  stride: Optional[int]
  depth: Optional[int]

  def decode(d: dict) -> "StateInfo":
    return StateInfo(d["name"], d["offset"], d["numBits"], StateType(d["type"]),
                     d.get("stride"), d.get("depth"))


@dataclass
class StateHierarchy:
  name: str
  states: List[StateInfo]
  children: List["StateHierarchy"]


@dataclass
class ModelInfo:
  name: str
  numStateBytes: int
  initialFnSym: str
  states: List[StateInfo]
  io: List[StateInfo]
  hierarchy: List[StateHierarchy]

  def decode(d: dict) -> "ModelInfo":
    return ModelInfo(d["name"], d["numStateBytes"], d.get("initialFnSym", ""),
                     [StateInfo.decode(d) for d in d["states"]], list(), list())


# Organize the state by hierarchy.
def group_state_by_hierarchy(
      states: List[StateInfo]) -> Tuple[List[StateInfo], List[StateHierarchy]]:
  local_state = list()
  hierarchies = list()
  remainder = list()

  used_names = set()

  def uniquify(name: str) -> str:
    if name in used_names:
      i = 0
      while f"{name}_{i}" in used_names:
        i += 1
      name = f"{name}_{i}"
    used_names.add(name)
    return name

  for state in states:
    if not state.name or "/" not in state.name:
      state.name = uniquify(state.name)
      local_state.append(state)
    else:
      remainder.append(state)
  while len(remainder) > 0:
    left = list()
    prefix = remainder[0].name.split("/")[0]
    substates = list()
    for state in remainder:
      if not state.name.startswith(prefix + "/"):
        left.append(state)
        continue
      state.name = removeprefix(state.name, prefix + "/")
      substates.append(state)
    remainder = left
    hierarchy_states, hierarchy_children = group_state_by_hierarchy(substates)
    prefix = uniquify(prefix)
    hierarchies.append(
      StateHierarchy(prefix, hierarchy_states, hierarchy_children))
  return local_state, hierarchies


# Process each model separately.
def format_signal(state: StateInfo) -> str:
  fields = [
    f"\"{state.name}\"", state.offset, state.numBits,
    f"Signal::{state.typ.value.capitalize()}"
  ]
  if state.typ == StateType.MEMORY:
    fields += [state.stride, state.depth]
  fields = ", ".join((str(f) for f in fields))
  return f"Signal{{{fields}}}"


def format_hierarchy(hierarchy: StateHierarchy) -> str:
  states = ",\n  ".join((format_signal(s) for s in hierarchy.states))
  if states:
    states = "\n  " + states + "\n"
  states = "{" + states + "}"
  children = ",\n  ".join(
    (format_hierarchy(c).replace("\n", "\n  ") for c in hierarchy.children))
  if children:
    children = "\n  " + children + "\n"
  children = "{" + children + "}"
  return f"Hierarchy{{\"{hierarchy.name}\", {len(hierarchy.states)}, {len(hierarchy.children)}, (Signal[]){states}, (Hierarchy[]){children}}}"


def state_cpp_type_nonmemory(state: StateInfo) -> str:
  for bits, ty in [(8, "uint8_t"), (16, "uint16_t"), (32, "uint32_t"),
                   (64, "uint64_t")]:
    if state.numBits <= bits:
      return ty
  return f"Bytes<{(state.numBits + 7) // 8}>"


def state_cpp_type(state: StateInfo) -> str:
  if state.typ == StateType.MEMORY:
    return f"Memory<{state_cpp_type_nonmemory(state)}, {state.stride}, {state.depth}>"
  return state_cpp_type_nonmemory(state)


C_KEYWORDS = """
alignas alignof and and_eq asm auto bitand bitor bool break case catch char
char16_t char32_t class compl const const_cast constexpr continue decltype
default delete do double dynamic_cast else enum explicit export extern false
float for friend goto if inline int long mutable namespace new noexcept not
not_eq nullptr operator or or_eq private protected public register
reinterpret_cast return short signed sizeof static static_assert static_cast
struct switch template this thread_local throw true try typedef typeid typename
union unsigned using virtual void volatile wchar_t while xor xor_eq
""".split()


def clean_name(name: str) -> str:
  name = re.sub(r'[^a-zA-Z_0-9]', "_", name)
  if not re.match(r'^[a-zA-Z_]', name):
    name = "_" + name
  if name in C_KEYWORDS:
    name = "_" + name
  return name


def format_view_hierarchy(hierarchy: StateHierarchy, depth: int) -> str:
  lines = []
  for state in hierarchy.states:
    lines.append(f"{state_cpp_type(state)} &{clean_name(state.name)};")
  if depth != 0:
    for child in hierarchy.children:
      lines.append(
        f"{indent(format_view_hierarchy(child, depth - 1))} {clean_name(child.name)};"
      )
  lines = "\n  ".join(lines)
  if lines:
    lines = "\n  " + lines + "\n"
  return f"struct {{{lines}}}"


def state_cpp_ref(state: StateInfo) -> str:
  return f"*({state_cpp_type(state)}*)(state+{state.offset})"


def format_view_constructor(hierarchy: StateHierarchy, depth: int) -> str:
  lines = []
  for state in hierarchy.states:
    lines.append(f".{clean_name(state.name)} = {state_cpp_ref(state)}")
  if depth != 0:
    for child in hierarchy.children:
      lines.append(
        f".{clean_name(child.name)} = {indent(format_view_constructor(child, depth - 1))}"
      )
  lines = ",\n  ".join(lines)
  if lines:
    lines = "\n  " + lines + "\n"
  return f"{{{lines}}}"


def indent(s: str, amount: int = 1):
  return s.replace("\n", "\n" + "  " * amount)


def load_models(state_json):
  with open(state_json, "r") as f:
    ret = [ModelInfo.decode(d) for d in json.load(f)]

  for mdl in ret:
    internal = list()
    for state in mdl.states:
      if state.typ != StateType.INPUT and state.typ != StateType.OUTPUT:
        internal.append(state)
      else:
        mdl.io.append(state)
    mdl.hierarchy = [
      StateHierarchy("internal", *group_state_by_hierarchy(internal))
    ]

  return ret
