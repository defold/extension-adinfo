#if defined(DM_PLATFORM_IOS)
#include "adinfo.h"
#import <AdSupport/AdSupport.h>

#if __has_include(<AppTrackingTransparency/ATTrackingManager.h>)
#import <AppTrackingTransparency/ATTrackingManager.h>
#endif

#import <StoreKit/SKAdNetwork.h>

void ADInfo_InitAdInfoExt(dmExtension::Params* params)
{
    dmLogInfo("ADInfo_InitAdInfoExt");
    if (dmConfigFile::GetInt(params->m_ConfigFile, "adinfo.register_for_attribution", 0))
    {
        if (@available(iOS 11.3, *))
        {
            dmLogInfo("ADInfo_InitAdInfoExt registerAppForAdNetworkAttribution");
            [SKAdNetwork registerAppForAdNetworkAttribution];
        }
        else
        {
            dmLogInfo("ADInfo_InitAdInfoExt registerAppForAdNetworkAttribution not available on iOS < 11.3");
        }
    }
}

void ADInfo_GetAdId()
{
    if (@available(iOS 14, *))
    {
        [ATTrackingManager requestTrackingAuthorizationWithCompletionHandler:^(ATTrackingManagerAuthorizationStatus status) {
            dmLogInfo("ADInfo_GetAdId callback");
            if (status == ATTrackingManagerAuthorizationStatusAuthorized)
            {
                dmLogInfo("ADInfo_GetAdId ATTrackingManagerAuthorizationStatusAuthorized");
                ASIdentifierManager* asim = [ASIdentifierManager sharedManager];
                NSString *uuid = [asim.advertisingIdentifier UUIDString];
                const char *uuid_lua = [uuid UTF8String];
                ADInfo_QueueAdId(uuid_lua);
            }
            else
            {
                dmLogInfo("ADInfo_GetAdId not authorized");
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
