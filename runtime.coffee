assert = require('assert')
var_dump = require('./util').var_dump

class Runtime

  constructor: (@engine, @funktions) ->
    @stack = new Stack()

  call: (funktion_name, args) ->
    @push_frame(args)
    return_val = @funktions[funktion_name].execute(@)
    @pop_frame()
    return return_val


  push_frame: (args) ->
    @stack.frames.push (new StackFrame())
    @cache_frame()
    @registers.push('0')
    for arg in args
      @registers.push(arg)

  pop_frame: () ->
    @stack.frames.pop()
    @cache_frame()

  # Nicer API
  cache_frame: () ->
    @check_invariants()
    @registers = @stack.frame().registers
    @locals = @stack.frame().locals

  check_invariants: () ->
    if @registers
      for reg in @registers
        if typeof(reg) isnt 'string'
          assert.fail(reg)
      for variable of @locals
        if typeof(variable) isnt 'string'
          assert.fail(variable)



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
