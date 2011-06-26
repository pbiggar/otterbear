class Runtime

  constructor: () ->
    @stack = new Stack()
    @stack.frames.push (new StackFrame())

  push_frame: () ->
    @stack.frames.push (new StackFrame())

  pop_frame: () ->
    @stack.frames.pop()

  write_local: (name, value) ->
    @stack.frame().locals[name] = value

  read_local: (name) ->
    @stack.frame().locals[name]


class Stack
  constructor: () ->
    @frames = []

  frame: () ->
    @frames[@frames.length - 1]

class StackFrame
  constructor: () ->
    @locals = {}


exports.Runtime = Runtime
