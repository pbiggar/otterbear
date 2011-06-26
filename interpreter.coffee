runtime = require("./runtime")

class Interpreter
  constructor: () ->

  execute: (rt, funktion) ->
    for _,bc of funktion.bytecodes
      bc.execute (rt)

exports.Interpreter = Interpreter
