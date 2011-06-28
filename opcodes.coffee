exports.Add = class Add
  constructor: (@lval, @left, @right) ->

  execute: (rt) ->
    rt.registers[@lval] = rt.registers[@left] + rt.registers[@right]


exports.Assign = class Assign
  constructor: (@lval, @rval) ->

  execute: (rt) ->
    rt.registers[@lval] = @rval


exports.Copy = class Copy
  constructor: (@lval, @rval) ->

  execute: (rt) ->
    rt.registers[@lval] = rt.registers[@rval]


exports.Read = class Read
  constructor: (@lval, @rval) ->

  execute: (rt) ->
    rt.registers[@lval] = rt.locals[@rval]


exports.Write = class Write
  constructor: (@lval, @rval) ->

  execute: (rt) ->
    rt.locals[@lval] = rt.registers[@rval]


exports.Call = class Call
  constructor: (@lval, @name, @params...) ->

  execute: (rt) ->
    args = (rt.registers[reg] for reg in @params)
    rt.registers[@lval] = rt.call(@name, args)


exports.Funktion = class Funktion
  constructor: (@name, @params, @bytes) ->

  execute: (rt) ->
    rt.engine.execute(rt, @)
    rt.registers[0]


exports.Return = class Return
  constructor: (@rval) ->

  execute: (rt) ->
    rt.registers[0] = rt.registers[@rval]
