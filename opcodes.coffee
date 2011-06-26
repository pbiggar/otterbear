exports.Add = class Add
  constructor: (@lval, @left, @right) ->

  execute: (rt) ->
    left = rt.read_local @left
    right = rt.read_local @right
    rt.write_local @lval, left + right

exports.Assign = class Assign
  constructor: (@lval, @rval) ->

  execute: (rt) ->
    rval = rt.read_local @rval
    rt.write_local @lval, rval


exports.Call = class Call
  constructor: (@lval, @name, @params...) ->

  execute: (rt) ->
    func = rt.read_local(@name)

