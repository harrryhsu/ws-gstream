const ci2c = require("../src/ci2c");

ci2c.Lens_FindMCU();
ci2c.Lens_ICRMode(true);
ci2c.Lens_ReadLensData();
ci2c.Lens_Initial();
ci2c.Piris_Initial();
