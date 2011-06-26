/* Parses JS code into bytecode. */

opcodes = require("./opcodes")
util = require("util")
assert = require("assert")

var_dump = (obj) ->
  console.log(util.inspect(obj))
  obj

remove_empty = (strings) ->
  (str for str in strings when str isnt "")

# Returns a list of functions, each with a list of bytecodes
exports.parse = (str) ->

  # Parse each function
  funktion_strings = remove_empty (str.split /^function /m)

  funks = {}
  for fs in funktion_strings

    # Parse this function's bytecodes
    lines = remove_empty(fs.split /\n/)
    bytes = for l in lines.splice(1)
      [name, args...] = l.split " "
      fetch_opcode(name, args)

    # Add the function
    [name, params...] = remove_empty (lines[0].split " ")
    funks[name] = new opcodes.Funktion(name, params, bytes)

  assert.ok 'main' of funks
  funks



upperFirst = (str) -> str.charAt(0).toUpperCase() + str.substr(1)

fetch_opcode = (name, args) ->
  # Call constructor via apply
  opcode = opcodes[upperFirst(name)]
  obj = new opcode
  opcode.apply(obj, args)
  obj

