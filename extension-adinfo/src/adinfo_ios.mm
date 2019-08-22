#if defined(DM_PLATFORM_IOS)
#include "private_adinfo.h"
#import <AdSupport/AdSupport.h>

void ADInfo_InitAdInfoExt()
{
}

char const* ADInfo_GetAdId()
{
    ASIdentifierManager* asim = [ASIdentifierManager sharedManager];
    NSString *uuid = [asim.advertisingIdentifier UUIDString];
    const char *uuid_lua = [uuid UTF8String];
    return uuid_lua;
}

bool ADInfo_IsAdvertisingTrackingEnabled()
{
    ASIdentifierManager* asim = [ASIdentifierManager sharedManager];
    return (bool)asim.advertisingTrackingEnabled;
}

#endif
