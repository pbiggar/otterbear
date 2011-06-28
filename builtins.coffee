var_dump = require('./util').var_dump

exports.builtins =
  print:
    execute: (rt, args) ->
      for arg in args
        console.log(arg)
      '0'
