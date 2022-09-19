const ci2c = require("./ci2c");

const ret = ci2c.open("/dev/i2c-2");
console.log(ret);
const buf = new Buffer(4);
const a = ci2c.read(0, buf, 4);
console.log(a);
console.log(
  `Open /dev/i2c-2 MCU 0x15 ID=${buf[0] * 256 + buf[1]} ${
    buf[2] * 256 + buf[3]
  } success`
);
