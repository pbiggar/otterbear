runtime = require("./runtime")

class Interpreter
  constructor: () ->

  execute: (rt, funktion) ->
    for bc in funktion.bytes
      bc.execute (rt)

exports.Interpreter = Interpreter
