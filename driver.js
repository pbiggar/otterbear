parser = require("./parser");
interpreter = require("./interpreter");
fs = require("fs")

input = fs.readFileSync("test.bc", 'utf-8');
console.log(input);
bytecodes = parser.parse(input);
interpreter.interpret(bytecodes);
