/* Parses JS code into bytecode. */

/* For now, simply read test.js. */

(function() {

function parse(str)
{
  lines = str.split("\n");
  bytes = [];
  for each (var l in lines) {
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

function fetch_opcode (name, args) {
  // Call constructor via apply
  var obj = new opcodes[name];
  opcodes[name].apply(obj, args);
  return obj;
}

var bc = parse(read("test.bc"));

})();
