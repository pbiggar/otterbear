
util = require("./util")
var_dump = util.var_dump

exports.Compiler = class
  constructor: () ->

  compile: (funktions) ->
    for f of funktions
      for bc in f.bytes
        var_dump (bc)
        code = bc.compile()
