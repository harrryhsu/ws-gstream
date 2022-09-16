
#include <stdlib.h>
#include <unistd.h>
#include "Error.h"
#include <string>
#include <queue>

// Edi
/*************************************************************************************************************/
#include "i2c.c"

// Motor Control I2C data parameters
#define SB_I2CAddr 0x15
#define SB_GetINFO 0x00
#define SB_SetPWM1 0x10
#define SB_SetPWM2 0x11
#define SB_ICRIO 0x20
#define SB_SenRst 0x30
#define SB_SenVsCnt 0x31
#define SB_SetMT1 0x40
#define SB_SetMT1Stb 0x41
#define SB_SetMT2 0x44
#define SB_SetMT2Stb 0x45
#define SB_SetMT3 0x48
#define SB_SetMT3Stb 0x49
#define SB_MTtoHome 0x50
#define SB_LENSSTB 0x60
#define SB_LENSDATA 0x61
#define SB_LENSIRIS 0x62

#define SB_GetINFObytes 4
#define SB_SetPWM1bytes 4
#define SB_SetPWM2bytes 4
#define SB_ICRIObytes 4
#define SB_SenRstbytes 1
#define SB_SenVsCntbytes 1
#define SB_SetMT1bytes 3
#define SB_SetMT1Stbbytes 2
#define SB_SetMT2bytes 3
#define SB_SetMT2Stbbytes 2
#define SB_SetMT3bytes 3
#define SB_SetMT3Stbbytes 2
#define SB_MTtoHomebytes 4
#define SB_LENSSTBbytes 156
#define SB_LENSDATAbytes 60
#define SB_LENSIRISbytes 40

// Lens control table define data
#define ZMIDX 0
#define FXIDX 1
#define AFSchRanIDX 2
#define AFSchStbIDX 3

#define PIENIDX 0
#define PILVIDX 1
#define HOMEIDX 2
#define DLYIDX 3
#define BASHIDX 4
#define ZMSEGCNT 5
#define ZMSEGIDX 6
#define FOCUSCALFLAG 7

#define PIRISMAXSTB 2
#define PIRISMINSTB 4
#define PIRISENABLE 5
#define PIRISRSTLV 6
#define PIRISRSTEN 7
#define LENSID 8

// Lens ID 0 Back focus  ID 1 ACM117VP104013CR1  ID 2 ACMZ118BP1240ICRCSR1  ID 2 D1150IR

int16_t MoveDly_TBL[13] = {
		20480, // 0  PPS   48.8
		10240, // 1  PPS   97.66
		5120,	 // 2  PPS  195.3
		2560,	 // 3  PPS  390.6
		1280,	 // 4  PPS  781.3
		640,	 // 5  PPS 1562.5
		320,	 // 6  PPS 3125
		160,
		80,
		40,
		20,
		10,
		5,
};

typedef struct lens_info
{
	int16_t AFFlag;
	int16_t AFdef_stp;
	int16_t AFschstb;
	int16_t AFstprange;
	int16_t focussteps;
	int16_t ZMdef_stp;
	int16_t zoomsteps;
	int16_t PirisFlag;
	int16_t PirisMaxStb;
	int16_t PirisMinStb;
	int16_t pirissteps;
	int16_t pirisstepssave;
	float Sharpnessaver;
	float Sharpnessmax;
	float FVvalue[5][80];
	float FVArray[8][8];
} LENSInfo;

// Lens -> Zoom & Focus model ACM117VP104013CR1
int16_t ZMotorData_TBL[15][2] = {
		// Zoom, Focus
		0, 0,				 // PI Detect enable
		0, 0,				 // PI Level
		6300, -4200, // Home
		4, 4,				 // MoveDly motor move step interval delay 0-20.48ms 1-10.24ms 2-5.12ms 3-2.56ms 4-1.28ms 5-0.64ms up to 12
		-40, 40,		 // Motor back lash
		17, 17,			 // Zoom segments 0 mean no need to initial
		0, 0,				 // Zoom index
		0, 0,				 // Focus calibrate flag
};

int16_t FocusDirBK, Zoomstepseg;

int16_t ZMotorSTB_TBL[20][4] = {
		-40, 1382 + 60, 36, 4,	 // Zoom  0  Zoomstep,  AFSchRange, AFSchStb
		-378, 1508 + 60, 36, 4,	 // Zoom  1  Zoomstep,  AFSchRange, AFSchStb
		-755, 1652 + 60, 36, 4,	 // Zoom  2  Zoomstep,  AFSchRange, AFSchStb
		-1133, 1798 + 60, 36, 4, // Zoom  3  Zoomstep,  AFSchRange, AFSchStb
		-1510, 1946 + 60, 36, 5, // Zoom  4  Zoomstep,  AFSchRange, AFSchStb
		-1888, 2094 + 60, 36, 5, // Zoom  5  Zoomstep,  AFSchRange, AFSchStb
		-2266, 2242 + 60, 36, 5, // Zoom  6  Zoomstep,  AFSchRange, AFSchStb
		-2643, 2384 + 60, 36, 6, // Zoom  7  Zoomstep,  AFSchRange, AFSchStb
		-3021, 2520 + 60, 36, 6, // Zoom  8  Zoomstep,  AFSchRange, AFSchStb
		-3398, 2644 + 60, 36, 6, // Zoom  9  Zoomstep,  AFSchRange, AFSchStb
		-3776, 2750 + 60, 36, 7, // Zoom 10  Zoomstep,  AFSchRange, AFSchStb
		-4154, 2834 + 60, 36, 7, // Zoom 11  Zoomstep,  AFSchRange, AFSchStb
		-4531, 2884 + 60, 36, 7, // Zoom 12  Zoomstep,  AFSchRange, AFSchStb
		-4909, 2890 + 60, 36, 8, // Zoom 13  Zoomstep,  AFSchRange, AFSchStb
		-5287, 2834 + 60, 36, 8, // Zoom 14  Zoomstep,  AFSchRange, AFSchStb
		-5664, 2706 + 60, 36, 8, // Zoom 15  Zoomstep,  AFSchRange, AFSchStb
		-5944, 2536 + 60, 36, 8, // Zoom 16  Zoomstep,  AFSchRange, AFSchStb
};

int16_t PIRIS_TBL[19] = {
		0,	 // PI Detect enable
		0,	 // PI Level
		-64, // FullOPEN
		1,	 // MoveDly motor move step interval delay 0-20.48ms 1-10.24ms 2-5.12ms 3-2.56ms 4-1.28ms 5-0.64ms up to 12
		-8,	 // Close or limit step
		1,	 // PIRIS Enable
		-32, // PIRIS Reset Level
		0,	 // PIRIS Reset Flag
		1,	 // Lens ID
};

bool Lens_FindMCU();
bool Lens_ICRMode(bool icemode);
bool Lens_ReadLensData(void);
bool Lens_WriteLensData(void);
int16_t Lens_GetFocusStep(void);
int16_t Lens_Focusmove(int16_t focusstb);
int16_t Lens_GetZoommoveStep(void);
int16_t Lens_Zoommove(int16_t zoomseg);
bool Lens_Initial(void);
int16_t Lens_GetPIrisStep(void);
bool Piris_move(int16_t movesteps);
bool Piris_Initial(void);

int i2cfd;
I2CDevice device;
LENSInfo Lenspara;
unsigned char i2cbuff[520];
ssize_t i2cret;

bool Lens_FindMCU()
{
	uint8_t temp1;
	/* Fill i2c device struct */
	device.addr = 0x15;
	device.tenbit = 0;
	device.delay = 2;
	device.flags = 0;
	device.page_bytes = 256;
	device.iaddr_bytes = 1; /* Set this to zero, and using i2c_ioctl_xxxx API will ignore chip internal address */

	/* find MCU */
	/* First open i2c bus */
	char I2CBus[] = "/dev/i2c-0";
	for (temp1 = 0; temp1 < 8; temp1++)
	{

		if ((i2cfd = i2c_open(I2CBus)) != -1)
		{
			device.bus = i2cfd;
			i2cret = i2c_ioctl_read(&device, 0x0, i2cbuff, 4);
			if (i2cret == -1 || (size_t)i2cret != 4)
			{
				i2c_close(i2cfd);
			}
			else
			{
				printf("Open %s MCU 0x15 ID=%d %d success\n", I2CBus, i2cbuff[0] * 256 + i2cbuff[1], i2cbuff[2] * 256 + i2cbuff[3]);
				break;
			}
		}
		I2CBus[9]++;
	}

	if (I2CBus[9] == '8')
	{
		printf("Sensor Board MCU not found\n");
		return false;
	}
	return true;
}

bool Lens_ICRMode(bool icemode)
{
	if (icemode)
	{
		i2cbuff[0] = 0x1;
		i2cbuff[1] = 0x0;
		printf("ICR to Day mode ");
	}
	else
	{
		i2cbuff[0] = 0x0;
		i2cbuff[1] = 0x1;
		printf("ICR to Night mode");
	}
	i2cbuff[2] = 0x0;
	i2cbuff[3] = 0xF0;
	i2cret = i2c_ioctl_write(&device, 0x20, i2cbuff, 4);
	if (i2cret == -1 || (size_t)i2cret != 4)
	{
		printf(" ICR error\n");
		// i2c_close(i2cfd);
		return false;
	}
	printf(" Done!\n");
	usleep(250000);
	return true;
}

bool Lens_ReadLensData(void)
{
	uint8_t *tempindexu8;
	int16_t tempdata;
	uint16_t tempchecksum;

	i2cret = i2c_ioctl_read(&device, SB_LENSSTB, i2cbuff, SB_LENSSTBbytes);
	if (i2cret == -1 || (size_t)i2cret != SB_LENSSTBbytes)
	{
		printf("SB_LENSSTB Read error i2cret=%d", (int)i2cret);
		// i2c_close(i2cfd);
	}

	memcpy(&ZMotorSTB_TBL, &i2cbuff[4], sizeof(ZMotorSTB_TBL));

	i2cret = i2c_ioctl_read(&device, SB_LENSDATA, i2cbuff, SB_LENSDATAbytes);
	if (i2cret == -1 || (size_t)i2cret != SB_LENSDATAbytes)
	{
		printf("SB_LENSDATA Read error i2cret=%d", (int)i2cret);
		// i2c_close(i2cfd);
	}

	memcpy(&ZMotorData_TBL, &i2cbuff[2], sizeof(ZMotorData_TBL));

	i2cret = i2c_ioctl_read(&device, SB_LENSIRIS, i2cbuff, SB_LENSIRISbytes);
	if (i2cret == -1 || (size_t)i2cret != SB_LENSIRISbytes)
	{
		printf("SB_LENSIRIS Read error i2cret=%d", (int)i2cret);
		// i2c_close(i2cfd);
	}

	memcpy(&PIRIS_TBL, &i2cbuff[2], sizeof(PIRIS_TBL));

	printf("ZMotorData_TBL read\n");
	for (tempdata = 0; tempdata < 15;)
	{
		printf("%d %d \n", ZMotorData_TBL[tempdata][0], ZMotorData_TBL[tempdata][1]);
		tempdata++;
	}

	printf("ZMotorSTB_TBL read\n");
	for (tempdata = 0; tempdata < 20;)
	{
		printf("%d %d %d %d\n", ZMotorSTB_TBL[tempdata][0], ZMotorSTB_TBL[tempdata][1], ZMotorSTB_TBL[tempdata][2], ZMotorSTB_TBL[tempdata][3]);
		tempdata++;
	}

	printf("PIRIS_TBL read\n");
	for (tempdata = 0; tempdata < 19;)
	{
		printf("%d \n", PIRIS_TBL[tempdata]);
		tempdata++;
	}

	return true;
}

bool Lens_WriteLensData(void)
{
	/*
	#define SB_LENSSTB    0x60
	#define SB_LENSDATA   0x61
	#define SB_LENSIRIS   0x62

	#define SB_LENSSTBbytes  156
	#define SB_LENSDATAbytes  60
	#define SB_LENSIRISbytes  40
	int16_t ZMotorData_TBL[15][2]= 60
	int16_t ZMotorSTB_TBL[20][4]= 156
	int16_t PIRIS_TBL[19]=40
	*/
	uint8_t *tempindexu8;
	int16_t tempdata;
	uint16_t tempchecksum;

	tempindexu8 = (uint8_t *)ZMotorSTB_TBL;
	for (tempdata = 4, tempchecksum = 0; tempdata < SB_LENSSTBbytes; tempdata++)
	{
		tempchecksum += i2cbuff[tempdata] = *tempindexu8++;
	}

	i2cbuff[0] = 0;
	i2cbuff[1] = PIRIS_TBL[LENSID];
	i2cbuff[2] = tempchecksum >> 8;
	i2cbuff[3] = tempchecksum & 0xff;
	i2cret = i2c_ioctl_write(&device, SB_LENSSTB, i2cbuff, SB_LENSSTBbytes);
	if (i2cret == -1 || (size_t)i2cret != SB_LENSSTBbytes)
	{
		printf("MCU Flash write error i2cret=%d", (int)i2cret);
		// i2c_close(i2cfd);
	}

	printf("SB_LENSSTB Set Lens ID %d CKSum %4x\n", i2cbuff[0] * 256 + i2cbuff[1], i2cbuff[2] * 256 + i2cbuff[3]);
	for (tempdata = 0; tempdata < SB_LENSSTBbytes;)
	{
		printf("%x %x %x %x %x %x %x %x\n", i2cbuff[tempdata], i2cbuff[tempdata + 1], i2cbuff[tempdata + 2], i2cbuff[tempdata + 3], i2cbuff[tempdata + 4], i2cbuff[tempdata + 5], i2cbuff[tempdata + 6], i2cbuff[tempdata + 7]);
		tempdata += 8;
	}
	usleep(100000);

	tempindexu8 = (uint8_t *)ZMotorData_TBL;
	for (tempdata = 2, tempchecksum = 0; tempdata < SB_LENSSTBbytes; tempdata++)
	{
		tempchecksum += i2cbuff[tempdata] = *tempindexu8++;
	}

	i2cbuff[0] = tempchecksum >> 8;
	i2cbuff[1] = tempchecksum & 0xff;
	i2cret = i2c_ioctl_write(&device, SB_LENSDATA, i2cbuff, SB_LENSDATAbytes);
	if (i2cret == -1 || (size_t)i2cret != SB_LENSDATAbytes)
	{
		printf("MCU Flash write error i2cret=%d", (int)i2cret);
		// i2c_close(i2cfd);
	}

	printf("SB_LENSDATA Set CKSum %4x\n", i2cbuff[0] * 256 + i2cbuff[1]);
	for (tempdata = 0; tempdata < SB_LENSDATAbytes;)
	{
		printf("%x %x %x %x %x %x %x %x\n", i2cbuff[tempdata], i2cbuff[tempdata + 1], i2cbuff[tempdata + 2], i2cbuff[tempdata + 3], i2cbuff[tempdata + 4], i2cbuff[tempdata + 5], i2cbuff[tempdata + 6], i2cbuff[tempdata + 7]);
		tempdata += 8;
	}
	usleep(100000);

	tempindexu8 = (uint8_t *)PIRIS_TBL;
	for (tempdata = 2, tempchecksum = 0; tempdata < SB_LENSIRISbytes; tempdata++)
	{
		tempchecksum += i2cbuff[tempdata] = *tempindexu8++;
	}

	i2cbuff[0] = tempchecksum >> 8;
	i2cbuff[1] = tempchecksum & 0xff;
	i2cret = i2c_ioctl_write(&device, SB_LENSIRIS, i2cbuff, SB_LENSIRISbytes);
	if (i2cret == -1 || (size_t)i2cret != SB_LENSIRISbytes)
	{
		printf("MCU Flash write error i2cret=%d", (int)i2cret);
		// i2c_close(i2cfd);
	}

	printf("PIRIS_TBL Set CKSum %4x\n", i2cbuff[0] * 256 + i2cbuff[1]);
	for (tempdata = 0; tempdata < SB_LENSDATAbytes;)
	{
		printf("%x %x %x %x %x %x %x %x\n", i2cbuff[tempdata], i2cbuff[tempdata + 1], i2cbuff[tempdata + 2], i2cbuff[tempdata + 3], i2cbuff[tempdata + 4], i2cbuff[tempdata + 5], i2cbuff[tempdata + 6], i2cbuff[tempdata + 7]);
		tempdata += 8;
	}
	usleep(100000);
	return true;
}

int16_t Lens_GetFocusStep(void)
{
	return Lenspara.focussteps;
}

int16_t Lens_Focusmove(int16_t focusstb)
{
	Lenspara.focussteps += focusstb;
	i2cbuff[0] = Lenspara.focussteps >> 8;
	i2cbuff[1] = Lenspara.focussteps & 0xff;
	i2cbuff[2] = ZMotorData_TBL[DLYIDX][FXIDX];
	i2cret = i2c_ioctl_write(&device, SB_SetMT1, i2cbuff, SB_SetMT1bytes);
	if (i2cret == -1 || (size_t)i2cret != SB_SetMT1bytes)
	{
		printf("Focus to Step error");
		i2c_close(i2cfd);
	}
	printf("Focus to Step %d", Lenspara.focussteps);
	return Lenspara.focussteps;
}

int16_t Lens_GetZoommoveStep(void)
{
	return Zoomstepseg;
}

int16_t Lens_Zoommove(int16_t zoomseg)
{
	int32_t delayus;
	int16_t tempa;

	if (zoomseg > ZMotorData_TBL[ZMSEGCNT][ZMIDX] - 1)
		zoomseg = ZMotorData_TBL[ZMSEGCNT][ZMIDX] - 1;
	else if (zoomseg < 0)
		zoomseg = 0;
	Zoomstepseg = zoomseg;
	if (ZMotorData_TBL[ZMSEGIDX][ZMIDX] == zoomseg)
	{
		printf("Input Zoom Seg same as current!");
		return zoomseg;
	}

	ZMotorData_TBL[ZMSEGIDX][ZMIDX] = zoomseg;
	// Move Focus to new position
	Lenspara.AFdef_stp = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][FXIDX];
	Lenspara.AFschstb = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][AFSchStbIDX];
	if (ZMotorData_TBL[FOCUSCALFLAG][ZMIDX] != 0) // Focus Calibrating
		Lenspara.AFstprange = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][AFSchRanIDX] * 2;
	else
		Lenspara.AFstprange = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][AFSchRanIDX];
	if (Lenspara.AFstprange > 60)
		Lenspara.AFstprange = 60;
	tempa = 1;
	if (ZMotorData_TBL[BASHIDX][FXIDX] > 0)
	{ // >0 mean back lash must add if new step < current step
		if (Lenspara.AFdef_stp < Lenspara.focussteps)
		{
			tempa = Lenspara.AFdef_stp - ZMotorData_TBL[BASHIDX][FXIDX];
			tempa = Lenspara.focussteps - tempa;
			Lenspara.focussteps = Lenspara.AFdef_stp - ZMotorData_TBL[BASHIDX][FXIDX];
			i2cbuff[0] = Lenspara.focussteps >> 8;
			i2cbuff[1] = Lenspara.focussteps & 0xff;
			i2cbuff[2] = ZMotorData_TBL[DLYIDX][FXIDX];
			i2cret = i2c_ioctl_write(&device, SB_SetMT1, i2cbuff, SB_SetMT1bytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT1bytes)
			{
				printf("Focus to Step error");
				// i2c_close(i2cfd);
				return -1;
			}
		}
	}
	else if (ZMotorData_TBL[BASHIDX][FXIDX] < 0)
	{ // <0 mean back lash must add if new step > current step
		if (Lenspara.AFdef_stp > Lenspara.focussteps)
		{
			tempa = Lenspara.AFdef_stp - ZMotorData_TBL[BASHIDX][FXIDX];
			tempa = Lenspara.focussteps - tempa;
			Lenspara.focussteps = Lenspara.AFdef_stp - ZMotorData_TBL[BASHIDX][FXIDX];
			i2cbuff[0] = Lenspara.focussteps >> 8;
			i2cbuff[1] = Lenspara.focussteps & 0xff;
			i2cbuff[2] = ZMotorData_TBL[DLYIDX][FXIDX];
			i2cret = i2c_ioctl_write(&device, SB_SetMT1, i2cbuff, SB_SetMT1bytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT1bytes)
			{
				printf("Focus to Step error");
				// i2c_close(i2cfd);
				return -1;
			}
		}
	}
	delayus = abs((int32_t)MoveDly_TBL[ZMotorData_TBL[DLYIDX][FXIDX]] * tempa) + 100000;
	printf("Sleep Focus to next position %dus BL Steps %d\n", delayus, tempa);
	usleep(delayus); // Sleep wait for motor move done!

	tempa = Lenspara.focussteps - Lenspara.AFdef_stp;
	Lenspara.focussteps = Lenspara.AFdef_stp;
	i2cbuff[0] = Lenspara.focussteps >> 8;
	i2cbuff[1] = Lenspara.focussteps & 0xff;
	i2cbuff[2] = ZMotorData_TBL[DLYIDX][FXIDX];
	i2cret = i2c_ioctl_write(&device, SB_SetMT1, i2cbuff, SB_SetMT1bytes);
	if (i2cret == -1 || (size_t)i2cret != SB_SetMT1bytes)
	{
		printf("Focus to Step error");
		// i2c_close(i2cfd);
		return -1;
	}
	delayus = abs((int32_t)MoveDly_TBL[ZMotorData_TBL[DLYIDX][FXIDX]] * tempa) + 100000;
	printf("Sleep Focus to next position %dus step %d\n", delayus, Lenspara.focussteps);
	usleep(delayus); // Sleep wait for motor move done!

	// Move Zoom to new position
	Lenspara.ZMdef_stp = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][ZMIDX];
	tempa = 1;
	if (ZMotorData_TBL[BASHIDX][ZMIDX] > 0)
	{ // >0 mean back lash must add if new step < current step
		if (Lenspara.ZMdef_stp < Lenspara.zoomsteps)
		{
			tempa = Lenspara.ZMdef_stp - ZMotorData_TBL[BASHIDX][ZMIDX];
			tempa = Lenspara.zoomsteps - tempa;
			Lenspara.zoomsteps = Lenspara.ZMdef_stp - ZMotorData_TBL[BASHIDX][ZMIDX];
			i2cbuff[0] = Lenspara.zoomsteps >> 8;
			i2cbuff[1] = Lenspara.zoomsteps & 0xff;
			i2cbuff[2] = ZMotorData_TBL[DLYIDX][ZMIDX];
			i2cret = i2c_ioctl_write(&device, SB_SetMT2, i2cbuff, SB_SetMT2bytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT2bytes)
			{
				printf("Zoom to Step error");
				// i2c_close(i2cfd);
				return -1;
			}
		}
	}
	else if (ZMotorData_TBL[BASHIDX][ZMIDX] < 0)
	{ // <0 mean back lash must add if new step > current step
		if (Lenspara.ZMdef_stp > Lenspara.zoomsteps)
		{
			tempa = Lenspara.ZMdef_stp - ZMotorData_TBL[BASHIDX][ZMIDX];
			tempa = Lenspara.zoomsteps - tempa;
			Lenspara.zoomsteps = Lenspara.ZMdef_stp - ZMotorData_TBL[BASHIDX][ZMIDX];
			i2cbuff[0] = Lenspara.zoomsteps >> 8;
			i2cbuff[1] = Lenspara.zoomsteps & 0xff;
			i2cbuff[2] = ZMotorData_TBL[DLYIDX][ZMIDX];
			i2cret = i2c_ioctl_write(&device, SB_SetMT2, i2cbuff, SB_SetMT2bytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT2bytes)
			{
				printf("Zoom to Step error");
				// i2c_close(i2cfd);
				return -1;
			}
		}
	}
	delayus = abs((int32_t)MoveDly_TBL[ZMotorData_TBL[DLYIDX][ZMIDX]] * tempa) + 100000;
	printf("Sleep Zoom to next position BL %dus BL Steps %d\n", delayus, tempa);
	usleep(delayus); // Sleep wait for motor move done!

	tempa = Lenspara.zoomsteps - Lenspara.ZMdef_stp;
	Lenspara.zoomsteps = Lenspara.ZMdef_stp;
	i2cbuff[0] = Lenspara.zoomsteps >> 8;
	i2cbuff[1] = Lenspara.zoomsteps & 0xff;
	i2cbuff[2] = ZMotorData_TBL[DLYIDX][ZMIDX];
	i2cret = i2c_ioctl_write(&device, SB_SetMT2, i2cbuff, SB_SetMT2bytes);
	if (i2cret == -1 || (size_t)i2cret != SB_SetMT2bytes)
	{
		printf("Zoom to Step error");
		i2c_close(i2cfd);
		return -1;
	}
	delayus = abs((int32_t)MoveDly_TBL[ZMotorData_TBL[DLYIDX][ZMIDX]] * tempa) + 100000;
	printf("Sleep Zoom to next position %dus step %d\n", delayus, Lenspara.zoomsteps);
	usleep(delayus); // Sleep wait for motor move done!
	return zoomseg;
	;
}

bool Lens_Initial(void)
{
	int32_t delayus;

	ZMotorData_TBL[ZMSEGIDX][ZMIDX] = 0; // Reset Zoom index step
	// Focus back to home
	if (ZMotorData_TBL[ZMSEGCNT][FXIDX] != 0)
	{
		if (ZMotorData_TBL[PIENIDX][FXIDX] != 0)
		{																					 // Focus to home with PI
			if (ZMotorData_TBL[PILVIDX][FXIDX] == 0) // Focus Channel 0
				i2cbuff[0] = 0;
			else
				i2cbuff[0] = 0x80;
			i2cbuff[1] = ZMotorData_TBL[DLYIDX][FXIDX];
			i2cbuff[2] = ZMotorData_TBL[HOMEIDX][FXIDX] >> 8;
			i2cbuff[3] = ZMotorData_TBL[HOMEIDX][FXIDX] & 0xff;
			i2cret = i2c_ioctl_write(&device, SB_MTtoHome, i2cbuff, SB_MTtoHomebytes);
			if (i2cret == -1 || (size_t)i2cret != SB_MTtoHomebytes)
			{
				perror("Focus to PI error");
				i2c_close(i2cfd);
				return false;
			}
			printf("Focus to PI");
		}
		else
		{ // Focus to home w/o PI
			// Set Focus current step to 0
			Lenspara.focussteps = 0;
			i2cbuff[0] = Lenspara.focussteps >> 8;
			i2cbuff[1] = Lenspara.focussteps & 0xff;
			i2cret = i2c_ioctl_write(&device, SB_SetMT1Stb, i2cbuff, SB_SetMT1Stbbytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT1Stbbytes)
			{
				printf("Focus set Step error");
				i2c_close(i2cfd);
				return false;
			}
			else
				printf("Focus initial set step  %d\n", Lenspara.focussteps);
			usleep(50000);
			i2cbuff[0] = ZMotorData_TBL[HOMEIDX][FXIDX] >> 8;
			i2cbuff[1] = ZMotorData_TBL[HOMEIDX][FXIDX] & 0xff;
			i2cbuff[2] = ZMotorData_TBL[DLYIDX][FXIDX];
			i2cret = i2c_ioctl_write(&device, SB_SetMT1, i2cbuff, SB_SetMT1bytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT1bytes)
			{
				perror("Focus to Home error");
				i2c_close(i2cfd);
				return false;
			}
			printf("Focus to move back %d steps\n", ZMotorData_TBL[HOMEIDX][FXIDX]);
		}
		delayus = abs((int32_t)MoveDly_TBL[ZMotorData_TBL[DLYIDX][FXIDX]] * ZMotorData_TBL[HOMEIDX][FXIDX]) + 200000;
		printf("Sleep for Focus to Home %dus step %d\n", delayus, ZMotorData_TBL[HOMEIDX][FXIDX]);
		usleep(delayus); // Sleep wait for motor move done!

		// Set Focus current step to 0
		Lenspara.focussteps = 0;
		i2cbuff[0] = Lenspara.focussteps >> 8;
		i2cbuff[1] = Lenspara.focussteps & 0xff;
		i2cret = i2c_ioctl_write(&device, SB_SetMT1Stb, i2cbuff, SB_SetMT1Stbbytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT1Stbbytes)
		{
			printf("Focus set Step error");
			i2c_close(i2cfd);
			return false;
		}
		usleep(1000);
		// Move Focus to Initial position
		Lenspara.AFFlag = 0; // 0 stop, 1 - 10 back to start search point, 20 - 60 searching
		Lenspara.AFdef_stp = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][FXIDX];
		Lenspara.AFschstb = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][AFSchStbIDX];	 // AF each seaching moving steps
		Lenspara.AFstprange = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][AFSchRanIDX]; // AF search steps range + / 1   example 20  ->  AFdef_stp - 20  to AFdef_stp + 20

		Lenspara.focussteps = Lenspara.AFdef_stp;
		i2cbuff[0] = Lenspara.focussteps >> 8;
		i2cbuff[1] = Lenspara.focussteps & 0xff;
		i2cbuff[2] = ZMotorData_TBL[DLYIDX][FXIDX];
		i2cret = i2c_ioctl_write(&device, SB_SetMT1, i2cbuff, SB_SetMT1bytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT1bytes)
		{
			printf("Focus to Step error");
			i2c_close(i2cfd);
			return false;
		}
		delayus = abs((int32_t)MoveDly_TBL[ZMotorData_TBL[DLYIDX][FXIDX]] * Lenspara.focussteps) + 200000;
		printf("Sleep Focus to initial position %dus step %d\n", delayus, Lenspara.focussteps);
		usleep(delayus); // Sleep wait for motor move done!
	}
	else
		printf("Focus no need to initial!\n");

	// Zoom back to home
	if (ZMotorData_TBL[ZMSEGCNT][ZMIDX] != 0)
	{
		if (ZMotorData_TBL[PIENIDX][ZMIDX] != 0)
		{																					 // Zoom to home with PI
			if (ZMotorData_TBL[PILVIDX][ZMIDX] == 0) // Zoom Channel 1
				i2cbuff[0] = 1;
			else
				i2cbuff[0] = 0x81;
			i2cbuff[1] = ZMotorData_TBL[DLYIDX][ZMIDX];
			i2cbuff[2] = ZMotorData_TBL[HOMEIDX][ZMIDX] >> 8;
			i2cbuff[3] = ZMotorData_TBL[HOMEIDX][ZMIDX] & 0xff;
			i2cret = i2c_ioctl_write(&device, SB_MTtoHome, i2cbuff, SB_MTtoHomebytes);
			if (i2cret == -1 || (size_t)i2cret != SB_MTtoHomebytes)
			{
				perror("Zoom to PI error");
				i2c_close(i2cfd);
				return false;
			}
			printf("Zoom to PI");
		}
		else
		{ // Zoom to home w/o PI
			// Set Zoom current step to 0
			Lenspara.zoomsteps = 0;
			i2cbuff[0] = Lenspara.zoomsteps >> 8;
			i2cbuff[1] = Lenspara.zoomsteps & 0xff;
			i2cret = i2c_ioctl_write(&device, SB_SetMT2Stb, i2cbuff, SB_SetMT2Stbbytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT2Stbbytes)
			{
				printf("Zoom to Step error");
				i2c_close(i2cfd);
				return false;
			}
			usleep(50000);
			i2cbuff[0] = ZMotorData_TBL[HOMEIDX][ZMIDX] >> 8;
			i2cbuff[1] = ZMotorData_TBL[HOMEIDX][ZMIDX] & 0xff;
			i2cbuff[2] = ZMotorData_TBL[DLYIDX][ZMIDX];
			i2cret = i2c_ioctl_write(&device, SB_SetMT2, i2cbuff, SB_SetMT2bytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT2bytes)
			{
				perror("Zoom to Home error");
				i2c_close(i2cfd);
				return false;
			}
			printf("Zoom to move back %d steps", ZMotorData_TBL[HOMEIDX][ZMIDX]);
		}
		delayus = abs((int32_t)MoveDly_TBL[ZMotorData_TBL[DLYIDX][ZMIDX]] * ZMotorData_TBL[HOMEIDX][ZMIDX]) + 200000;
		printf("Sleep Zoom to Home %dus step %dus\n", delayus, ZMotorData_TBL[HOMEIDX][ZMIDX]);
		usleep(delayus); // Sleep wait for motor move done!

		// Set Zoom current step to 0
		Lenspara.zoomsteps = 0;
		i2cbuff[0] = Lenspara.zoomsteps >> 8;
		i2cbuff[1] = Lenspara.zoomsteps & 0xff;
		i2cret = i2c_ioctl_write(&device, SB_SetMT2Stb, i2cbuff, SB_SetMT2Stbbytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT2Stbbytes)
		{
			printf("Zoom to Step error");
			i2c_close(i2cfd);
			return false;
		}
		usleep(1000);
		Lenspara.zoomsteps = ZMotorSTB_TBL[ZMotorData_TBL[ZMSEGIDX][ZMIDX]][ZMIDX];
		i2cbuff[0] = Lenspara.zoomsteps >> 8;
		i2cbuff[1] = Lenspara.zoomsteps & 0xff;
		i2cbuff[2] = ZMotorData_TBL[DLYIDX][ZMIDX];
		i2cret = i2c_ioctl_write(&device, SB_SetMT2, i2cbuff, SB_SetMT2bytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT2bytes)
		{
			printf("Zoom to Step error");
			i2c_close(i2cfd);
			return false;
		}
		usleep(400000);
	}
	else
		printf("Zoom no need to initial!\n");
}

int16_t Lens_GetPIrisStep(void)
{
	return Lenspara.pirissteps;
}

bool Piris_move(int16_t movesteps)
{
	if (PIRIS_TBL[PIRISENABLE])
	{
		if (Lenspara.PirisMaxStb > 0)
		{
			if (movesteps >= Lenspara.PirisMaxStb)
			{
				movesteps = Lenspara.PirisMaxStb;
				if (PIRIS_TBL[PIRISRSTEN])
				{
					movesteps += 16;
					PIRIS_TBL[PIRISRSTEN] = 2;
				}
			}
			else if (movesteps <= Lenspara.PirisMinStb)
				movesteps = Lenspara.PirisMinStb;
			if (movesteps < PIRIS_TBL[PIRISRSTLV])
				PIRIS_TBL[PIRISRSTEN] = 1;
		}
		else
		{
			if (movesteps <= Lenspara.PirisMaxStb)
			{
				movesteps = Lenspara.PirisMaxStb;
				if (PIRIS_TBL[PIRISRSTEN])
				{
					movesteps -= 16;
					PIRIS_TBL[PIRISRSTEN] = 2;
				}
			}
			else if (movesteps >= Lenspara.PirisMinStb)
				movesteps = Lenspara.PirisMinStb;
			if (movesteps > PIRIS_TBL[PIRISRSTLV])
				PIRIS_TBL[PIRISRSTEN] = 1;
		}
		Lenspara.pirissteps = movesteps;
		i2cbuff[0] = Lenspara.pirissteps >> 8;
		i2cbuff[1] = Lenspara.pirissteps & 0xff;
		i2cbuff[2] = PIRIS_TBL[DLYIDX] | 0x80; // Enable Piris hold when Zoom moving
		i2cret = i2c_ioctl_write(&device, SB_SetMT3, i2cbuff, SB_SetMT3bytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT3bytes)
		{
			printf("Piris to Step error");
			i2c_close(i2cfd);
			return false;
		}

		if (PIRIS_TBL[PIRISRSTEN] == 2)
		{
			usleep(20000);
			PIRIS_TBL[PIRISRSTEN] = 0;
			Lenspara.pirissteps = Lenspara.PirisMaxStb;
			i2cbuff[0] = Lenspara.pirissteps >> 8;
			i2cbuff[1] = Lenspara.pirissteps & 0xff;
			i2cret = i2c_ioctl_write(&device, SB_SetMT3Stb, i2cbuff, SB_SetMT3Stbbytes);
			if (i2cret == -1 || (size_t)i2cret != SB_SetMT3Stbbytes)
			{
				printf("Piris to Step error");
				i2c_close(i2cfd);
				return false;
			}
			printf("Piris set Step %d \n", Lenspara.pirissteps);
		}
		printf("Piris to Step %d %x PIRISRSTFLAG %d ", Lenspara.pirissteps, i2cbuff[2], PIRIS_TBL[PIRISRSTEN]);
		return true;
	}
	else
		return false;
}

bool Piris_Initial(void)
{
	if (PIRIS_TBL[PIRISENABLE])
	{
		// Set Piris current step to 0
		i2cbuff[0] = 0;
		i2cbuff[1] = 0;
		i2cret = i2c_ioctl_write(&device, SB_SetMT3Stb, i2cbuff, SB_SetMT3Stbbytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT3Stbbytes)
		{
			printf("Piris to Step error");
			i2c_close(i2cfd);
			return false;
		}
		usleep(1000);
		if (PIRIS_TBL[PIRISMAXSTB] > 0)
			Lenspara.pirissteps = PIRIS_TBL[PIRISMAXSTB] + 16;
		else
			Lenspara.pirissteps = PIRIS_TBL[PIRISMAXSTB] - 16;

		i2cbuff[0] = Lenspara.pirissteps >> 8;
		i2cbuff[1] = Lenspara.pirissteps & 0xff;
		i2cbuff[2] = PIRIS_TBL[DLYIDX] | 0x80; // Enable Piris hold when Zoom moving
		i2cret = i2c_ioctl_write(&device, SB_SetMT3, i2cbuff, SB_SetMT3bytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT3bytes)
		{
			printf("Piris to Step error");
			i2c_close(i2cfd);
			return false;
		}

		usleep(20000);

		Lenspara.pirissteps = PIRIS_TBL[PIRISMAXSTB];

		i2cbuff[0] = Lenspara.pirissteps >> 8;
		i2cbuff[1] = Lenspara.pirissteps & 0xff;
		i2cret = i2c_ioctl_write(&device, SB_SetMT3Stb, i2cbuff, SB_SetMT3Stbbytes);
		if (i2cret == -1 || (size_t)i2cret != SB_SetMT3Stbbytes)
		{
			printf("Piris to Step error");
			i2c_close(i2cfd);
			return false;
		}
	}
	//    Lenspara.PirisFlag   = 0;
	PIRIS_TBL[PIRISRSTEN] = 0;
	//    Lenspara.pirissteps  = PIRIS_TBL[PIRISMAXSTB];
	Lenspara.PirisMaxStb = PIRIS_TBL[PIRISMAXSTB];
	Lenspara.PirisMinStb = PIRIS_TBL[PIRISMINSTB];
	return true;
}
