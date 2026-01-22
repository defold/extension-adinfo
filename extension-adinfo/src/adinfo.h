#pragma once
#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID)

#include <dmsdk/sdk.h>

void ADInfo_QueueAdId(const char* adId);

void ADInfo_PlatformInitAdInfoExt(dmExtension::Params* params);
void ADInfo_PlatformGetAdId();
bool ADInfo_PlatformIsAdvertisingTrackingEnabled();
void AdInfo_PlatformUpdateConversionValue(int value);

struct ADInfoData
{
	ADInfoData()
	{
		memset(this, 0, sizeof(*this));
	};

	dmScript::LuaCallbackInfo* m_callback;

	char* m_AdId;
};


#endif
