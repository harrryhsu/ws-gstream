const ci2c = require("./ci2c");

ci2c.open("/dev/i2c-2");
const a = ci2c.read(0, 4);
console.log(
  `Open /dev/i2c-2 MCU 0x15 ID=${a[0] * 256 + a[1]} ${
    a[2] * 256 + a[3]
  } success`
);

console.log(a);
