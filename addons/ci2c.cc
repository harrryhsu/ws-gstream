#include "napi.h"
#include "TBImage.c"

using namespace Napi;

Boolean NAN_Lens_FindMCU(const CallbackInfo &info)
{
	return Boolean::New(info.Env(), Lens_FindMCU());
}

Boolean NAN_Lens_ICRMode(const CallbackInfo &info)
{
	auto mode = info[0].ToBoolean().Value();
	return Boolean::New(info.Env(), Lens_ICRMode(mode));
}

Boolean NAN_Lens_ReadLensData(const CallbackInfo &info)
{
	return Boolean::New(info.Env(), Lens_ReadLensData());
}

Boolean NAN_Lens_WriteLensData(const CallbackInfo &info)
{
	return Boolean::New(info.Env(), Lens_WriteLensData());
}

Number NAN_Lens_GetFocusStep(const CallbackInfo &info)
{
	return Number::New(info.Env(), Lens_GetFocusStep());
}

Number NAN_Lens_Focusmove(const CallbackInfo &info)
{
	auto zoomseg = info[0].ToNumber().Uint32Value();
	return Number::New(info.Env(), Lens_Focusmove(zoomseg));
}

Number NAN_Lens_GetZoommoveStep(const CallbackInfo &info)
{
	return Number::New(info.Env(), Lens_GetZoommoveStep());
}

Number NAN_Lens_Zoommove(const CallbackInfo &info)
{
	auto focusstb = info[0].ToNumber().Uint32Value();
	return Number::New(info.Env(), Lens_Zoommove(focusstb));
}

Boolean NAN_Lens_Initial(const CallbackInfo &info)
{
	return Boolean::New(info.Env(), Lens_Initial());
}

Number NAN_Lens_GetPIrisStep(const CallbackInfo &info)
{
	return Number::New(info.Env(), Lens_GetPIrisStep());
}

Boolean NAN_Piris_move(const CallbackInfo &info)
{
	auto movesteps = info[0].ToNumber().Uint32Value();
	return Boolean::New(info.Env(), Piris_move(movesteps));
}

Boolean NAN_Piris_Initial(const CallbackInfo &info)
{
	return Boolean::New(info.Env(), Piris_Initial());
}

void NAN_i2c_close(const CallbackInfo &info)
{
	i2c_close(device.bus);
}

Object Init(Env env, Object exports)
{
	exports.Set(String::New(env, "i2c_close"), Function::New(env, NAN_i2c_close));
	exports.Set(String::New(env, "Lens_FindMCU"), Function::New(env, NAN_Lens_FindMCU));
	exports.Set(String::New(env, "Lens_ICRMode"), Function::New(env, NAN_Lens_ICRMode));
	exports.Set(String::New(env, "Lens_ReadLensData"), Function::New(env, NAN_Lens_ReadLensData));
	exports.Set(String::New(env, "Lens_WriteLensData"), Function::New(env, NAN_Lens_WriteLensData));
	exports.Set(String::New(env, "Lens_GetFocusStep"), Function::New(env, NAN_Lens_GetFocusStep));
	exports.Set(String::New(env, "Lens_Focusmove"), Function::New(env, NAN_Lens_Focusmove));
	exports.Set(String::New(env, "Lens_GetZoommoveStep"), Function::New(env, NAN_Lens_GetZoommoveStep));
	exports.Set(String::New(env, "Lens_Zoommove"), Function::New(env, NAN_Lens_Zoommove));
	exports.Set(String::New(env, "Lens_Initial"), Function::New(env, NAN_Lens_Initial));
	exports.Set(String::New(env, "Lens_GetPIrisStep"), Function::New(env, NAN_Lens_GetPIrisStep));
	exports.Set(String::New(env, "Piris_move"), Function::New(env, NAN_Piris_move));
	exports.Set(String::New(env, "Piris_Initial"), Function::New(env, NAN_Piris_Initial));
	return exports;
}

NODE_API_MODULE(ci2c, Init)