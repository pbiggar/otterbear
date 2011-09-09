# Parses JS code into bytecode.

fs = require('fs')
cs = require('coffee-script')
narcissus = require('narcissus')

exports.Parser = class
  constructor: (@filename) ->

  parse: () ->
    input = fs.readFileSync(@filename, 'utf-8')

    if (/\.coffee$/.test(@filename))
      input = cs.compile(input)

    ast = narcissus.parser.parse(input)
    new AST(ast)


class AST
  constructor: (@n_ast) ->

  to_bytecode: () ->
    # TODO


bc = (new exports.Parser('test.js')).parse().to_bytecode()

# The aim is to ahead-of-time compile the compiler, and use it to interpret
# (and later JIT) the bytecodes (naturally, the compiled compiler will be able
# to interpret itself).
#
# To compile the compiler, we translate it into bytecodes, then translate the
# bytecodes into bitcode, then pass that off to LLVM.
#
# That leaves the standard library, the GC, and implementations of values,
# strings, arrays, etc. They should be also implemented in coffeescript, but a
# lower-level version which corresponds directly to bitcode. We can also write
# the bytecode definitions in this RCS (restricted coffeescript, based on the
# RPython), and preprocess them depending on whether we're translating or
# interpreting or jitting them.


opcodes = require("./opcodes")
util = require("util")
assert = require("assert")

remove_empty = (strings) ->
  (str for str in strings when str isnt "")

# Returns a list of functions, each with a list of bytecodes
exports.parse = (str) ->

  # Parse each function
  funktion_strings = remove_empty (str.split /^function /m)

  funks = {}
  for fs in funktion_strings

    # Strip comments and blank lines
    lines = remove_empty (fs.split /\n/)
    lines = (l for l in lines when l[0] != '#')

    # Parse this function's bytecodes
    bytes = for l in lines.splice(1)
      [name, args...] = l.split " "
      fetch_opcode(name, args)

    # Add the function
    [name, params...] = remove_empty (lines[0].split " ")
    funks[name] = new opcodes.Funktion(name, params, bytes)

  assert.ok 'main' of funks
  funks



upperFirst = (str) -> str.charAt(0).toUpperCase() + str.substr(1)

# Vars of the form _N are register numbers
parseRegNums = (var_str) ->
  if var_str[0] is '_' then parseInt(var_str.slice(1)) else var_str

fetch_opcode = (name, args) ->
  args = (parseRegNums(arg) for arg in args)

  # Call constructor via apply
  opcode = opcodes[upperFirst(name)]
  obj = new opcode
  opcode.apply(obj, args)
  obj

