runtime = require("./runtime")
var_dump = require('./util').var_dump

class Interpreter
  constructor: () ->

  execute: (rt, funktion) ->
    #    var_dump rt.stack.frame()
    for bc in funktion.bytes
      #      console.log()
      #      var_dump bc
      bc.execute (rt)
      #      var_dump rt.stack.frame()

exports.Interpreter = Interpreter
