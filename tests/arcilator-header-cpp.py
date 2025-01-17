#!/usr/bin/env python3
import argparse
from arcilator_pylib import indent, format_hierarchy, format_view_hierarchy, state_cpp_type, state_cpp_ref, \
  format_view_constructor, format_signal, load_models
from jinja2 import Environment, DictLoader, select_autoescape

header_cpp_template = """#include "arcilator-runtime.h"
{% for model in models %}
extern "C" {
{% if model.initialFnSym %}
void {{ model.name }}_initial(void* state);
{% endif %}
void {{ model.name }}_eval(void* state);
}

class {{ model.name }}Layout {
public:
  static const char *name;
  static const unsigned numStates;
  static const unsigned numStateBytes;
  static const std::array<Signal, {{ model.io|length }}> io;
  static const Hierarchy hierarchy;
};

const char *{{ model.name }}Layout::name = "{{ model.name }}";
const unsigned {{ model.name }}Layout::numStates = {{ model.states|length }};
const unsigned {{ model.name }}Layout::numStateBytes = {{ model.numStateBytes }};
const std::array<Signal, {{ model.io|length }}> {{ model.name }}Layout::io = {
{% for io in model.io %}
  {{ format_signal(io) }},
{% endfor %}
};

const Hierarchy {{ model.name }}Layout::hierarchy = {{ indent(format_hierarchy(model.hierarchy[0])) }};

class {{ model.name }}View {
public:
{% for io in model.io %}
  {{ state_cpp_type(io) }} &{{ io.name }};
{% endfor %}
  {{ indent(format_view_hierarchy(model.hierarchy[0], view_depth)) }} {{ model.hierarchy[0].name }};
  uint8_t *state;

  {{ model.name }}View(uint8_t *state) :
{% for io in model.io %}
    {{ io.name }}({{ state_cpp_ref(io) }}),
{% endfor %}
    {{ model.hierarchy[0].name }}({{ indent(format_view_constructor(model.hierarchy[0], view_depth), 2) }}),
    state(state) {}
};

class {{ model.name }} {
public:
  std::vector<uint8_t> storage;
  {{ model.name }}View view;

  {{ model.name }}() : storage({{ model.name }}Layout::numStateBytes, 0), view(&storage[0]) {
{% if model.initialFnSym %}
    {{ model.initialFnSym }}(&storage[0]);
{% endif %}
  }
  void eval() { {{ model.name }}_eval(&storage[0]); }
  ValueChangeDump<{{ model.name }}Layout> vcd(std::basic_ostream<char> &os) {
    ValueChangeDump<{{ model.name }}Layout> vcd(os, &storage[0]);
    vcd.writeHeader();
    vcd.writeDumpvars();
    return vcd;
  }
};

#define {{ model.name.upper() }}_PORTS \\
{% for io in model.io %}
{% if loop.last %}
  PORT({{io.name}})
{%- else %}
  PORT({{io.name}}) \\
{% endif %}
{% endfor %}
{% endfor %}
"""


def render_header_cpp(models, view_depth):
  for model in models:
    reserved = {"state"}
    for io in model.io:
      if io.name in reserved:
        io.name = io.name + "_"

  env = Environment(
    loader=DictLoader({"header-cpp.template": header_cpp_template}),
    autoescape=select_autoescape(),
    trim_blocks=True
  )

  template = env.get_template("header-cpp.template")

  return template.render(
    models=models,
    indent=indent,
    format_hierarchy=format_hierarchy,
    format_view_hierarchy=format_view_hierarchy,
    state_cpp_type=state_cpp_type,
    state_cpp_ref=state_cpp_ref,
    format_view_constructor=format_view_constructor,
    format_signal=format_signal,
    view_depth=view_depth,
  )


if __name__ == "__main__":
  # Parse command line arguments.
  parser = argparse.ArgumentParser(
    description="Generate a C++ header for an Arc model")
  parser.add_argument("state_json",
                      metavar="STATE_JSON",
                      help="state description file to process")
  parser.add_argument("--view-depth",
                      metavar="DEPTH",
                      type=int,
                      default=-1,
                      help="hierarchy levels to expose as C++ structs")
  args = parser.parse_args()

  models_data = load_models(args.state_json)
  print(render_header_cpp(models_data, args.view_depth))
