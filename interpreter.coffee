runtime = require("./runtime")

exports.interpret = (bytecodes) ->
  rt = new runtime.Runtime()

  for _, bc of bytecodes
    bc.execute (rt)
    console.log rt
