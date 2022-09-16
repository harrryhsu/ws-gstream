const ci2c = require("bindings")("ci2c");

console.log(ci2c);

const SB_I2CAddr = 0x15;
const SB_GetINFO = 0x00;
const SB_SetPWM1 = 0x10;
const SB_SetPWM2 = 0x11;
const SB_ICRIO = 0x20;
const SB_SenRst = 0x30;
const SB_SenVsCnt = 0x31;
const SB_SetMT1 = 0x40;
const SB_SetMT1Stb = 0x41;
const SB_SetMT2 = 0x44;
const SB_SetMT2Stb = 0x45;
const SB_SetMT3 = 0x48;
const SB_SetMT3Stb = 0x49;
const SB_MTtoHome = 0x50;
const SB_LENSSTB = 0x60;
const SB_LENSDATA = 0x61;
const SB_LENSIRIS = 0x62;

const SB_GetINFObytes = 4;
const SB_SetPWM1bytes = 4;
const SB_SetPWM2bytes = 4;
const SB_ICRIObytes = 4;
const SB_SenRstbytes = 1;
const SB_SenVsCntbytes = 1;
const SB_SetMT1bytes = 3;
const SB_SetMT1Stbbytes = 2;
const SB_SetMT2bytes = 3;
const SB_SetMT2Stbbytes = 2;
const SB_SetMT3bytes = 3;
const SB_SetMT3Stbbytes = 2;
const SB_MTtoHomebytes = 4;
const SB_LENSSTBbytes = 156;
const SB_LENSDATAbytes = 60;
const SB_LENSIRISbytes = 40;

const ZMIDX = 0;
const FXIDX = 1;
const AFSchRanIDX = 2;
const AFSchStbIDX = 3;

const PIENIDX = 0;
const PILVIDX = 1;
const HOMEIDX = 2;
const DLYIDX = 3;
const BASHIDX = 4;
const ZMSEGCNT = 5;
const ZMSEGIDX = 6;
const FOCUSCALFLAG = 7;

const PIRISMAXSTB = 2;
const PIRISMINSTB = 4;
const PIRISENABLE = 5;
const PIRISRSTLV = 6;
const PIRISRSTEN = 7;
const LENSID = 8;

const MoveDly_TBL = [
  20480, //0  PPS   48.8
  10240, //1  PPS   97.66
  5120, //2  PPS  195.3
  2560, //3  PPS  390.6
  1280, //4  PPS  781.3
  640, //5  PPS 1562.5
  320, //6  PPS 3125
  160,
  80,
  40,
  20,
  10,
  5,
];

const Lenspara = {
  AFFlag: 0,
  AFdef_stp: 0,
  AFschstb: 0,
  AFstprange: 0,
  focussteps: 0,
  ZMdef_stp: 0,
  zoomsteps: 0,
  PirisFlag: 0,
  PirisMaxStb: 0,
  PirisMinStb: 0,
  pirissteps: 0,
  pirisstepssave: 0,
  Sharpnessaver: 0,
  Sharpnessmax: 0,
};

const ZMotorData_TBL = [
  [0, 0], // PI Detect enable
  [0, 0], // PI Level
  [6300, -4200], // Home
  [4, 4], // MoveDly motor move step interval delay 0-20.48ms 1-10.24ms 2-5.12ms 3-2.56ms 4-1.28ms 5-0.64ms up to 12
  [-40, 40], // Motor back lash
  [17, 17], // Zoom segments 0 mean no need to initial
  [0, 0], // Zoom index
  [0, 0], // Focus calibrate flag
];

const ZMotorSTB_TBL = [
  [-40, 1382 + 60, 36, 4], // Zoom  0  Zoomstep,  AFSchRange, AFSchStb
  [-378, 1508 + 60, 36, 4], // Zoom  1  Zoomstep,  AFSchRange, AFSchStb
  [-755, 1652 + 60, 36, 4], // Zoom  2  Zoomstep,  AFSchRange, AFSchStb
  [-1133, 1798 + 60, 36, 4], // Zoom  3  Zoomstep,  AFSchRange, AFSchStb
  [-1510, 1946 + 60, 36, 5], // Zoom  4  Zoomstep,  AFSchRange, AFSchStb
  [-1888, 2094 + 60, 36, 5], // Zoom  5  Zoomstep,  AFSchRange, AFSchStb
  [-2266, 2242 + 60, 36, 5], // Zoom  6  Zoomstep,  AFSchRange, AFSchStb
  [-2643, 2384 + 60, 36, 6], // Zoom  7  Zoomstep,  AFSchRange, AFSchStb
  [-3021, 2520 + 60, 36, 6], // Zoom  8  Zoomstep,  AFSchRange, AFSchStb
  [-3398, 2644 + 60, 36, 6], // Zoom  9  Zoomstep,  AFSchRange, AFSchStb
  [-3776, 2750 + 60, 36, 7], // Zoom 10  Zoomstep,  AFSchRange, AFSchStb
  [-4154, 2834 + 60, 36, 7], // Zoom 11  Zoomstep,  AFSchRange, AFSchStb
  [-4531, 2884 + 60, 36, 7], // Zoom 12  Zoomstep,  AFSchRange, AFSchStb
  [-4909, 2890 + 60, 36, 8], // Zoom 13  Zoomstep,  AFSchRange, AFSchStb
  [-5287, 2834 + 60, 36, 8], // Zoom 14  Zoomstep,  AFSchRange, AFSchStb
  [-5664, 2706 + 60, 36, 8], // Zoom 15  Zoomstep,  AFSchRange, AFSchStb
  [-5944, 2536 + 60, 36, 8], // Zoom 16  Zoomstep,  AFSchRange, AFSchStb
];

const PIRIS_TBL = [
  0, // PI Detect enable
  0, // PI Level
  -64, // FullOPEN
  1, // MoveDly motor move step interval delay 0-20.48ms 1-10.24ms 2-5.12ms 3-2.56ms 4-1.28ms 5-0.64ms up to 12
  -8, // Close or limit step
  1, // PIRIS Enable
  -32, // PIRIS Reset Level
  0, // PIRIS Reset Flag
  1, // Lens ID
];

var i2cbus;
const i2cbuff = Buffer.from([...Array(520)].map((i) => 0));

// const read = (cmd, length) => {
//   return new Promise((res, rej) => {
//     i2cbus.readBytes(cmd, length, (err, buffer) => {
//       if (err) rej(err);
//       else res(buffer);
//     });
//   });
// };

// const write = (cmd, length) => {
//   return new Promise((res, rej) => {
//     i2cbus.writeI2cBlock(SB_I2CAddr, cmd, length, i2cbuff, (err, rawData) => {
//       if (err) rej(err);
//       else res();
//     });
//   });
// };

// const open = async () => {
//   for (let i = 0; i < 8; i++) {
//     try {
//       i2cbus = new i2c(SB_I2CAddr, { device: "/dev/i2c-" + i });
//       if (i2cbus) {
//         const buffer = await read(0x0, 4);
//         console.log(
//           `Open bus-${i} MCU 0x15 ID=${buffer[0] * 256 + buffer[1]} ${
//             buffer[2] * 256 + buffer[3]
//           } success\n`
//         );
//         return;
//       }
//     } catch (e) {}
//   }

//   throw "I2C Bus not found";
// };

// const readLensData = () => {
//   return read(SB_LENSDATA, SB_LENSDATAbytes).then((buffer) => {
//     console.log(buffer);
//   });
// };

// const focus = (focusstb) => {
//   Lenspara.focussteps += focusstb;
//   i2cbuff[0] = Lenspara.focussteps >> 8;
//   i2cbuff[1] = Lenspara.focussteps & 0xff;
//   i2cbuff[2] = ZMotorData_TBL[DLYIDX][FXIDX];

//   return write(SB_SetMT1, SB_SetMT1bytes)
//     .then(() => {
//       printf("Focus to Step %d", Lenspara.focussteps);
//       return Lenspara.focussteps;
//     })
//     .catch(() => console.log("Focus to Step error") || close());
// };

// open().then(async () => {
//   await readLensData();
//   close();
// });

// docker run --restart always -d --network=host aler9/rtsp-simple-server
// gst-launch-1.0 nvarguscamerasrc sensor-id=0 ! 'video/x-raw(memory:NVMM),width=1920, height=1080, framerate=30/1, format=NV12' ! nvv4l2h264enc ! h264parse ! rtspclientsink location=rtsp://localhost:8554/live protocols=tcp
