opcodes = [];

function Add(lval, left, right) {
  this.lval = lval;
  this.left = left;
  this.right = right;
  return this;
}

function Assign(lval, rval) {
  this.lval = lval;
  this.rval = rval;
  return this;
}

function Call(lval, name /*, ... */) {
  this.lval = lval;
  this.name = name;
  return this;
}



opcodes["add"] = Add;
opcodes["assign"] = Assign;
opcodes["call"] = Call;
