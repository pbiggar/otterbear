
util = require("./util")
var_dump = util.var_dump

exports.Compiler = class
  constructor: () ->

  compile: (funktions) ->
    for name,f of funktions
      @.compile_funktion(f)

  compile_funktion: (f) ->
    return
    # Parse each bytecode's implementation into bitcode
    bitcodes =
      for bc in f.bytes
        ast = Parser.parse(bc.impl)
        ast.to_bitcode()

    # Stitch them all together
    bitcode = bitcodes.join()
    exe = llvm.compile(bitcode)
    exe.save('a.out')

