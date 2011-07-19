runtime = require("./runtime")
var_dump = require('./util').var_dump

exports.Interpreter = class
  constructor: () ->

  execute: (rt, funktion) ->
    for bc in funktion.bytes
      bc.execute (rt)
