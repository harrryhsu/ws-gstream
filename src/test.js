const ci2c = require("bindings")("ci2c");

const a = ci2c.read(2, new Buffer(4), 4);
console.log(a);
