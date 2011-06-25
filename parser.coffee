/* Parses JS code into bytecode. */

opcodes = require("./opcodes")
util = require("./util")

exports.parse = (str) ->

  lines = str.split("\n")
  bytes = for _,l of lines
    continue if l is ""
    [name, args...] = l.split " "
    fetch_opcode(name, args)


upperFirst = (str) -> str.charAt(0).toUpperCase() + str.substr(1)

fetch_opcode = (name, args) ->
  # Call constructor via apply
  opcode = opcodes[upperFirst(name)]
  obj = new opcode
  opcodes[name].apply(obj, args)
  obj

