#pragma once
#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID)

void ADInfo_InitAdInfoExt();
char const* ADInfo_GetAdId();
bool ADInfo_IsAdvertisingTrackingEnabled();

#endif
