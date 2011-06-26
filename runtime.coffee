class Runtime

  constructor: () ->
    @stack = new Stack()
    @stack.frames.push (new StackFrame([]))

  push_frame: (params) ->
    @stack.frames.push (new StackFrame(params))

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
  constructor: (@params) ->
    @locals = {}


exports.Runtime = Runtime
