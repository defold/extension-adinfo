#if defined(DM_PLATFORM_IOS)
#include "adinfo.h"
#import <AdSupport/AdSupport.h>

#import <AppTrackingTransparency/ATTrackingManager.h>

void ADInfo_InitAdInfoExt()
{
}

void ADInfo_GetAdId()
{
    if (@available(iOS 14, *))
    {
        [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
            if (status == ATTrackingManagerAuthorizationStatusAuthorized)
            {
                ASIdentifierManager* asim = [ASIdentifierManager sharedManager];
                NSString *uuid = [asim.advertisingIdentifier UUIDString];
                const char *uuid_lua = [uuid UTF8String];
                ADInfo_QueueAdId(uuid_lua);
            }
            else
            {
                ADInfo_QueueAdId("");
            }
        }];
    }
    else
    {
        ASIdentifierManager* asim = [ASIdentifierManager sharedManager];
        NSString *uuid = [asim.advertisingIdentifier UUIDString];
        const char *uuid_lua = [uuid UTF8String];
        ADInfo_QueueAdId(uuid_lua);
    }
}

bool ADInfo_IsAdvertisingTrackingEnabled()
{
    if (@available(iOS 14, *))
    {
        return ([ATTrackingManager trackingAuthorizationStatus] == ATTrackingManagerAuthorizationStatusAuthorized);
    }
    else
    {
        ASIdentifierManager* asim = [ASIdentifierManager sharedManager];
        return (bool)asim.advertisingTrackingEnabled;
    }
}

#endif
