assert = require('assert')
var_dump = require('./util').var_dump

class Runtime

  constructor: (@engine, @funktions) ->
    @stack = new Stack()

  call: (funktion_name, args) ->
    @push_frame(args)
    @engine.execute(this, @funktions[funktion_name])
    return_val = @registers[0]
    var_dump return_val
    @pop_frame()
    return return_val


  push_frame: (args) ->
    @stack.frames.push (new StackFrame())
    @cache_frame()
    @registers.push(0)
    for arg in args
      @registers.push(arg)

  pop_frame: () ->
    @stack.frames.pop()
    @cache_frame()

  # Nicer API
  cache_frame: () ->
    @registers = @stack.frame().registers
    @locals = @stack.frame().locals


class Stack
  constructor: () ->
    @frames = []

  frame: () ->
    assert.ok (@frames.length > 0)
    @frames[@frames.length - 1]

class StackFrame
  constructor: () ->
    @locals = {}
    @registers = []


exports.Runtime = Runtime
