exports.builtins =
  print:
    execute: (rt, arg) ->
      for arg in rt.frame().registers
        console.log(arg)
