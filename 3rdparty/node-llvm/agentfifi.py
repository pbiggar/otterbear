#!/usr/bin/env python

import sys
import yaml
import clang
from clang.cindex import Index

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

  class func(object):
    def __init__(self, name):
      self.name = name
      self.body = []

    def add_to_body(self, str):
      self.body += [str]

    # TODO: ninja or another template framework will simplify this
    def __str__(self):
      return """static v8::Handle<v8::Value>
""" + self.name + """(const v8::Arguments& args)
{
  v8::HandleScope scope;
  """ + "\n  ".join(self.body) + """
}
"""

 

  def generate(self):
    output = ""

    for enum in self.ffi.enums:
      func = self.func(enum.name)
      func.add_to_body("ASSIGN((int)(" + enum.name + "));")
      func.add_to_body("RETURN0();")
      output += str(func)

    for func in self.ffi.basics:
      func = self.func(func.name)
      sig = llvm.read_signature(func.name)
      for i,arg in enumerate(sig.args):
        func.add_to_body("ARG(" + i + ', ' + arg.type + ');')
      output += str(func)

    return output




ffi = AgentFiFI(sys.argv[1])
print NodeJS(ffi).generate()
