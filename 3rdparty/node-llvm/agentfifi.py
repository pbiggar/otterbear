#!/usr/bin/env python

import sys
import yaml

class Enum:
  def __init__(self, name):
    self.name = name

class Basic:
  def __init__(self, name):
    self.name = name

class Advanced:
  def __init__(self, name, specs):
    self.name = name
    self.specs = specs


class AgentFiFI:

  def __init__(self, filename):
    self.filename = filename
    self.parse()

  def parse(self):
    struct = yaml.safe_load(file(self.filename))
    for type in struct:
      getattr(self, type.lower())(struct[type])

  def basic(self, funcs):
    self.basics = [Basic(name) for name in funcs]

  def advanced(self, funcs):
    for func in funcs:
      self.advanced = [Advanced(name, specs) for name,specs in func.items()]

  def prefix(self, prefix):
    self._prefix = prefix

  def enum(self, names):
    self.enums = [Enum(name) for name in names]



class NodeJS(object):
  def __init__(self, ffi):
    self.ffi = ffi

  def generate(self):
    output = ""

    for self.ffi.enums:

    pass




ffi = AgentFiFI(sys.argv[1])
print NodeJS(ffi).generate()
