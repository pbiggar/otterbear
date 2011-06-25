exports.Add = class Add
  constructor: (@lval, @left, @right) ->

exports.Assign = class Assign
  constructor: (@lval, @rval) ->

exports.Call = class Call
  constructor: (@lval, @name, @params...) ->
