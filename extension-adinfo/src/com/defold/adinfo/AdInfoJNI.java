package com.defold.adinfo;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import android.app.Activity;

public class AdInfoJNI {
    private Activity activity;

    public AdInfoJNI(final Activity activity) {
        this.activity = activity;
        
        new Thread(new Runnable() {
            public void run() {
                try {
                    AdvertisingIdClient.Info info = AdvertisingIdClient.getAdvertisingIdInfo(activity);
                    setAdInfo(info.getId(), info.isLimitAdTrackingEnabled());
                } catch (Exception e) {
                    setAdInfo("", false);
                    System.out.println("Unable to get AdvertisingIdClient.Info, exception was thrown: "+ e.toString());
                }
            }
        }).start();
    }

    private Object m_AdGuard = new Object();
    private String m_AdId;
    private boolean m_LimitAdTracking;


    public void setAdInfo(String adId, boolean limitAdTracking)
    {
        synchronized(m_AdGuard)
        {
            m_AdId = adId;
            m_LimitAdTracking = limitAdTracking;
        }
    }

    /**
     * Return null for does not exist yet, blank string if completed but unable to get id.
     */
    public String getAdId()
    {
        synchronized(m_AdGuard)
        {
            return m_AdId;
        }
    }

    public boolean getLimitAdTracking()
    {
        synchronized(m_AdGuard)
        {
            return !m_LimitAdTracking;
        }
    }

}
