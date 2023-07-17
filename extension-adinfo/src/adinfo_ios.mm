#if defined(DM_PLATFORM_IOS)
#include "adinfo.h"
#import <AdSupport/AdSupport.h>

#if __has_include(<AppTrackingTransparency/ATTrackingManager.h>)
#import <AppTrackingTransparency/ATTrackingManager.h>
#endif

#import <StoreKit/SKAdNetwork.h>

void ADInfo_PlatformInitAdInfoExt(dmExtension::Params* params)
{
    if (dmConfigFile::GetInt(params->m_ConfigFile, "adinfo.register_for_attribution", 0))
    {
        if (@available(iOS 11.3, *))
        {
            [SKAdNetwork registerAppForAdNetworkAttribution];
        }
        else
        {
            dmLogInfo("registerAppForAdNetworkAttribution() not available on iOS < 11.3");
        }
    }
}

void ADInfo_PlatformGetAdId()
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

bool ADInfo_PlatformIsAdvertisingTrackingEnabled()
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


void AdInfo_PlatformUpdateConversionValue(int value) {
    if (@available(iOS 14, *))
    {
        [SKAdNetwork updateConversionValue:value];
    }
    else
    {
        dmLogInfo("updateConversionValue() not available on iOS < 14");
    }
};

#endif
