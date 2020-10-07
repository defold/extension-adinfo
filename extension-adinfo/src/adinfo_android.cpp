#if defined(DM_PLATFORM_ANDROID)

#include <jni.h>
#include <dmsdk/sdk.h>
#include "adinfo.h"

struct ThreadAttacher {
    JNIEnv *env;
    bool has_attached;
    ThreadAttacher() : env(NULL), has_attached(false) {
        if (dmGraphics::GetNativeAndroidJavaVM()->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
            dmGraphics::GetNativeAndroidJavaVM()->AttachCurrentThread(&env, NULL);
            has_attached = true;
        }
    }
    ~ThreadAttacher() {
        if (has_attached) {
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
            }
            env->ExceptionClear();
            dmGraphics::GetNativeAndroidJavaVM()->DetachCurrentThread();
        }
    }
};

struct ClassLoader {
    private:
    JNIEnv *env;
    jobject class_loader_object;
    jmethodID find_class;
    public:
    ClassLoader(JNIEnv *env) : env(env) {
        jclass activity_class = env->FindClass("android/app/NativeActivity");
        jmethodID get_class_loader = env->GetMethodID(activity_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
        class_loader_object = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
        jclass class_loader = env->FindClass("java/lang/ClassLoader");
        find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        env->DeleteLocalRef(activity_class);
        env->DeleteLocalRef(class_loader);
    }
    ~ClassLoader() {
        env->DeleteLocalRef(class_loader_object);
    }
    jclass load(const char *class_name) {
        jstring str_class_name = env->NewStringUTF(class_name);
        jclass loaded_class = (jclass)env->CallObjectMethod(class_loader_object, find_class, str_class_name);
        env->DeleteLocalRef(str_class_name);
        return loaded_class;
    }
};

struct AdInfo
{
    jobject                 m_AdInfo;

    jmethodID               m_getAdId;
    jmethodID               m_getLimitAdTracking;
};

static AdInfo         g_adinfo;

void ADInfo_InitAdInfoExt(dmExtension::Params* params)
{
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;
    ClassLoader class_loader = ClassLoader(env);
    jclass cls = class_loader.load("com.defold.adinfo.AdInfoJNI");

    g_adinfo.m_getAdId = env->GetMethodID(cls, "getAdId", "()Ljava/lang/String;");
    g_adinfo.m_getLimitAdTracking = env->GetMethodID(cls, "getLimitAdTracking", "()Z");

    jmethodID jni_constructor = env->GetMethodID(cls, "<init>", "(Landroid/app/Activity;)V");
    g_adinfo.m_AdInfo = env->NewGlobalRef(env->NewObject(cls, jni_constructor, dmGraphics::GetNativeAndroidActivity()));
}

void ADInfo_GetAdId()
{
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;

    jstring return_value = (jstring)env->CallObjectMethod(g_adinfo.m_AdInfo, g_adinfo.m_getAdId);
    if (return_value)
    {
        const char* new_char = env->GetStringUTFChars(return_value, 0);
        env->DeleteLocalRef(return_value);

        ADInfo_QueueAdId(new_char);
    }
}

bool ADInfo_IsAdvertisingTrackingEnabled()
{
    ThreadAttacher attacher;
    JNIEnv *env = attacher.env;

    jboolean return_value = (jboolean)env->CallBooleanMethod(g_adinfo.m_AdInfo, g_adinfo.m_getLimitAdTracking);
    return (return_value == JNI_TRUE);
}

#endif
