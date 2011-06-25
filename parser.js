/* Parses JS code into bytecode. */

opcodes = require("./opcodes");

exports.parse = function parse(str)
{
  lines = str.split("\n");
  bytes = [];
  for (var i in lines) {
    var l = lines[i];
    if (l == "") {
      continue;
    }

    tokens = l.split(" ");
    name = tokens[0];

    args = tokens.splice(1);

    bytes += [fetch_opcode(name, args)];
  }
  return bytes;
}

fetch_opcode = function (name, args) {
  // Call constructor via apply
  var obj = new opcodes[name];
  opcodes[name].apply(obj, args);
  return obj;
}
