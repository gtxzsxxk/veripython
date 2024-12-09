#!/usr/bin/env python3
import argparse
from arcilator_pylib import load_models

if __name__ == "__main__":
  parser = argparse.ArgumentParser(
    description="Generate a Python Module for an Arc model")
  parser.add_argument("state_json",
                      metavar="STATE_JSON",
                      help="state description file to process")
  args = parser.parse_args()
  models_data = load_models(args.state_json)