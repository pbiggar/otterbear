util = require('util') # The CommonJS one

exports.var_dump = (obj) ->
  if 'constructor' in obj
    x = obj.constructor.toString().split("\n")[0]
    x = x.match(/function ([^(]+)\(/)[1]
    console.log(x + ' ' + util.inspect(obj))
  else
    console.log(util.inspect(obj))
  obj


