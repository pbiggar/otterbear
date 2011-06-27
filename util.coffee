util = require('util') # The CommonJS one

exports.var_dump = (obj) ->
  x = obj.constructor.toString().split("\n")[0]
  x = x.match(/function ([^(]+)\(/)[1]

  console.log(x + ' ' + util.inspect(obj))
  obj


