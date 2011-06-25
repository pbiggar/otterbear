/* Parses JS code into bytecode. */

opcodes = require("./opcodes")

exports.parse = (str) ->

  lines = str.split("\n")
  bytes = for l of lines
    continue if l is ""
    [name, args...] = l.split " "
    fetch_opcode(name, args)


fetch_opcode = (name, args) ->
  # Call constructor via apply
   obj = new opcodes[name]
  opcodes[name].apply(obj, args)
  obj

