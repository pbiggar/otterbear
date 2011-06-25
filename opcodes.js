
exports.Add = function (lval, left, right) {
  this.lval = lval;
  this.left = left;
  this.right = right;
  return this;
}

exports.Assign = function (lval, rval) {
  this.lval = lval;
  this.rval = rval;
  return this;
}

exports.Call = function (lval, name /*, ... */) {
  this.lval = lval;
  this.name = name;
  return this;
}

