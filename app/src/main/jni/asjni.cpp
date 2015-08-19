#include <jni.h>
#include <android/log.h>
#include <cstdint>
#include <cinttypes>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <thread>
#include <unistd.h>


typedef struct {
    jclass classRef;
    jmethodID constructor;
    // you can cache more thing
} cache_UserData;

cache_UserData g_userData;

extern "C" {

JavaVM *g_pJVM;

/*
 * JNI_OnLoad 是唯一可以取得 JavaVM 的地方
 * JavaVM 可以安全的在 Thread 間共享
 */
jint JNI_OnLoad(JavaVM *pJVM, void *reserved)
{
    g_pJVM = pJVM;

    JNIEnv *env;
    if (pJVM->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jclass classUserData = env->FindClass("cc/aznc/asjni/UserData");
    g_userData.classRef = (jclass)env->NewGlobalRef(classUserData);

    // Method ID is not an reference
    g_userData.constructor = env->GetMethodID(g_userData.classRef, "<init>", "()V");

    return JNI_VERSION_1_6;
}

/*
 * ====== Class PassValue ======
 */

JNIEXPORT jdouble JNICALL
Java_cc_aznc_asjni_PassValue_passDouble(JNIEnv *env, jobject instance, jdouble d) {
    double jdoubleVal = d;
    double retVal = jdoubleVal - 1;
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passDouble got %f, return %f", jdoubleVal, retVal);
    return retVal;
}

JNIEXPORT jfloat JNICALL
Java_cc_aznc_asjni_PassValue_passFloat(JNIEnv *env, jobject instance, jfloat f) {
    float jfloatVal = f;
    float retVal = jfloatVal - 1;
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passFloat got %f, return %f", jfloatVal, retVal);
    return retVal;
}

JNIEXPORT jlong JNICALL
Java_cc_aznc_asjni_PassValue_passLong(JNIEnv *env, jobject instance, jlong l) {
    int64_t jlongVal = l;
    int64_t retVal = jlongVal - 1;
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passLong got %" PRIi64 ", return %" PRIi64, jlongVal, retVal);
    return retVal;
}

JNIEXPORT jint JNICALL
Java_cc_aznc_asjni_PassValue_passInt(JNIEnv *env, jobject instance, jint i) {
    int32_t jintVal = i;
    int32_t retVal = jintVal - 1;
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passInt got %" PRIi32 ", return %" PRIi32, jintVal, retVal);
    return retVal;
}

JNIEXPORT jshort JNICALL
Java_cc_aznc_asjni_PassValue_passShort(JNIEnv *env, jobject instance, jshort s) {
    int16_t jshortVal = s;
    int16_t retVal = jshortVal - 1;
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passShort got %" PRIi16 ", return %" PRIi16, jshortVal, retVal);
    return retVal;
}

JNIEXPORT jchar JNICALL
Java_cc_aznc_asjni_PassValue_passChar(JNIEnv *env, jobject instance, jchar c) {
    uint16_t jcharVal = c;
    uint16_t retVal = jcharVal - 1;
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passChar got %c(%" PRIu16 "), return %c(%" PRIu16 ")", jcharVal, jcharVal, retVal, retVal);
    return retVal;
}

JNIEXPORT jbyte JNICALL
Java_cc_aznc_asjni_PassValue_passByte(JNIEnv *env, jobject instance, jbyte b) {
    int8_t jbyteVal = b;
    int8_t retVal = jbyteVal - 1;
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passByte got %" PRIi8 ", return %" PRIi8, jbyteVal, retVal);
    return retVal;
}

JNIEXPORT jboolean JNICALL
Java_cc_aznc_asjni_PassValue_passBoolean(JNIEnv *env, jobject instance, jboolean b) {
    bool jboolVal = b;
    if (jboolVal) {
        __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passBoolean got true, return false");
        return false;
    }
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passBoolean got false, return true");
    return true;
}

/*
 * 使用 GetStringUTFChars() 取得 UTF-8 格式的 const char*
 * 使用 std::string 複製一份，並增加內容傳回
 *
 * 順便示範 GetStringLength 與 GetStringUTFLength 的差異
 */
JNIEXPORT jstring JNICALL
Java_cc_aznc_asjni_PassValue_passStringUTF(JNIEnv *env, jobject instance, jstring s_) {
    const char *s = env->GetStringUTFChars(s_, NULL);
    if (NULL == s) {
        // TODO: move this check to last chapter
        // GetStringUTFChars can not allocate memory, it's good practice to check against NULL
    }
    std::string jstringVal(s);
    // when you no longer need the char* return by GetStringUTFChars, remember to invoke ReleaseStringUTFChars
    env->ReleaseStringUTFChars(s_, s);

    // input = "你好", GetStringLength return 2, and GetStringUTFLength return 6
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passStringUTF input %d Unicode Char, it's %d Bytes UTF-8 string",
                        env->GetStringLength(s_), env->GetStringUTFLength(s_));

    std::string retVal = jstringVal + " ヽ(✿ﾟ▽ﾟ)ノ嗨";
    __android_log_print(ANDROID_LOG_INFO, "Native PassValue", "passStringUTF got \"%s\", return \"%s\"", jstringVal.c_str(), retVal.c_str());
    return env->NewStringUTF(retVal.c_str());
}

/*
 * 使用 GetStringRegion() 將字串複製到預先準備好的空間內
 * 使用 std::u16string 或是 std::vector<uint16_t> 儲存 Java 內部的 UTF-16 編碼字串
 *
 * ** 注意，UTF-16 長度並不固定，有可能由兩個單位(4 Bytes)組成
 *    因此這範例使用 rbegin + rend 來做順序反轉，是可能出錯的，僅供參考
 */
JNIEXPORT jstring JNICALL
Java_cc_aznc_asjni_PassValue_revertString(JNIEnv *env, jobject instance, jstring s_) {
    jsize length = env->GetStringLength(s_);
    std::vector<uint16_t> jstringVal(length);
    env->GetStringRegion(s_, 0, length, &jstringVal[0]);

    std::vector<uint16_t> retVal(jstringVal.rbegin(), jstringVal.rend());
    return env->NewString(retVal.data(), length);
}

/*
 * 使用 GetBooleanArrayElements 取得陣列資料
 * 並使用 ReleaseBooleanArrayElements 將改變後的資料寫回到 Java 變數
 */
JNIEXPORT void JNICALL
Java_cc_aznc_asjni_PassValue_changeBooleanArray(JNIEnv *env, jobject instance, jbooleanArray boolArray_) {
    jsize length = env->GetArrayLength(boolArray_);
    jboolean *pBoolArray = env->GetBooleanArrayElements(boolArray_, NULL);
    for (auto i = 0; i < length; ++i) {
        pBoolArray[i] = !pBoolArray[i];
    }

    // 第三個參數很重要
    // 0 = copy back the content and free the pBoolArray buffer
    // JNI_COMMIT = copy back the content but do not free the pBoolArray buffer
    // JNI_ABORT = free the buffer without copying back the possible changes
    env->ReleaseBooleanArrayElements(boolArray_, pBoolArray, 0);
}

/*
 * 使用 GetIntArrayRegion 取得資料
 * 並使用 SetIntArrayRegion 將資料寫回 Java 變數
 */
JNIEXPORT void JNICALL
Java_cc_aznc_asjni_PassValue_changeIntArray(JNIEnv *env, jobject instance, jintArray intArray_) {
    jsize length = env->GetArrayLength(intArray_);
    std::vector<int32_t> jintArrayVal(length);
    env->GetIntArrayRegion(intArray_, 0, length, &jintArrayVal[0]);

    // modify
    std::vector<int32_t> retVal(jintArrayVal.size());
    std::transform(jintArrayVal.begin(), jintArrayVal.end(), retVal.begin(), [](int i){return i * 2;});

    // write back
    env->SetIntArrayRegion(intArray_, 0, retVal.size(), &retVal[0]);
}

/*
 * 示範基本型別以外的操作方法
 */
JNIEXPORT void JNICALL
Java_cc_aznc_asjni_PassValue_changeStringArray(JNIEnv *env, jobject instance, jobjectArray strArray) {
    jsize length = env->GetArrayLength(strArray);
    for (auto i = 0; i < length; ++i) {
        // Object 陣列只能一個個取出
        jstring javaStr = (jstring)env->GetObjectArrayElement(strArray, i);

        // 以下跟 passStringUTF 相同
        const char *s = env->GetStringUTFChars(javaStr, NULL);
        std::string jstringVal(s);
        env->ReleaseStringUTFChars(javaStr, s);
        jstringVal += " ヽ(✿ﾟ▽ﾟ)ノ嗨";

        // 用 *新的* Java String 物件替換掉
        env->SetObjectArrayElement(strArray, i, env->NewStringUTF(jstringVal.c_str()));
    }
}

/*
 * 建立新的 Java 陣列並回傳
 */
JNIEXPORT jcharArray JNICALL
Java_cc_aznc_asjni_PassValue_stringToCharArray(JNIEnv *env, jobject instance, jstring str_) {
    jsize length = env->GetStringLength(str_);
    const jchar *str = env->GetStringChars(str_, 0);
    // break string into jchars
    std::vector<jchar> retVal(str, str + length);
    env->ReleaseStringChars(str_, str);

    // create new Java array
    jcharArray retArray = env->NewCharArray(retVal.size());
    // it's good practice to check against NULL
    // TODO: move this code to last chapter
    if (NULL != retArray) {
        // write value
        env->SetCharArrayRegion(retArray, 0, retVal.size(), &retVal[0]);
    }

    return retArray;
}


/*
 * ====== Class InvokeByNative ======
 */
#include <stdexcept>
JNIEXPORT void JNICALL
Java_cc_aznc_asjni_InvokeByNative_updateTime(JNIEnv *env, jobject instance) {
    jclass classChangeFromNative = env->FindClass("cc/aznc/asjni/InvokeByNative");
    jfieldID fieldIDTimeStamp = env->GetFieldID(classChangeFromNative, "timeStamp", "J");
    jmethodID methodIDAddLog = env->GetMethodID(classChangeFromNative, "addLog", "(Ljava/lang/String;)V");

    // set time
    std::time_t timestamp = std::time(nullptr);
    env->SetLongField(instance, fieldIDTimeStamp, timestamp);

    // also add a log
    std::ostringstream msgStream;
    msgStream << "Message from C: Update timestamp to " << timestamp;
    env->CallVoidMethod(instance, methodIDAddLog, env->NewStringUTF(msgStream.str().c_str()));
}

/*
 * 如何檢查例外
 * 並將例外捕捉清除或丟回給 Java
 */
JNIEXPORT void JNICALL
Java_cc_aznc_asjni_InvokeByNative_handleException(JNIEnv *env, jobject instance) {
    jclass classChangeFromNative = env->FindClass("cc/aznc/asjni/InvokeByNative");
    jmethodID methodIDGiveMeException = env->GetMethodID(classChangeFromNative, "giveMeException", "()V");

    env->CallVoidMethod(instance, methodIDGiveMeException);
    jthrowable lexp = env->ExceptionOccurred();
    if (lexp) {
        // return 丟回去給 Java
        //return;

        // 或是捕捉並清除
        // 在 logcat 記下例外事件
        env->ExceptionDescribe();
        // 清除例外
        env->ExceptionClear();
        // 清理變數
        env->DeleteLocalRef(lexp);
    }
}

JNIEXPORT void JNICALL
Java_cc_aznc_asjni_InvokeByNative_nativeGiveMeException(JNIEnv *env, jobject instance) {
    jclass classException = env->FindClass("java/lang/Exception");
    if (classException) {
        env->ThrowNew(classException, "Exception from native!");
    }
}

static bool catchAIgnoreException(JNIEnv *pEnv)
{
    jthrowable jexception = pEnv->ExceptionOccurred();
    if (jexception) {
        //pEnv->ExceptionDescribe();
        pEnv->ExceptionClear();
        pEnv->DeleteLocalRef(jexception);
        return true;
    }
    return false;
}

void runFindClass(JNIEnv *env)
{
    // test system class
    jclass classString = env->FindClass("java/lang/String");
    if (catchAIgnoreException(env)) {
        __android_log_print(ANDROID_LOG_INFO, "Native FindClass", "failed to find class String");
    } else {
        // will run into here
        __android_log_print(ANDROID_LOG_INFO, "Native FindClass", "success to find class String");
    }

    // test application class
    jclass classUserData = env->FindClass("cc/aznc/asjni/UserData");
    if (catchAIgnoreException(env)) {
        // will run into here
        __android_log_print(ANDROID_LOG_INFO, "Native FindClass", "failed to find class UserData");
    } else {
        __android_log_print(ANDROID_LOG_INFO, "Native FindClass", "success to find class UserData");
    }

    // use cached class data
    jobject objectUserData = env->NewObject(g_userData.classRef, g_userData.constructor);
    if (catchAIgnoreException(env)) {
        __android_log_print(ANDROID_LOG_INFO, "Native FindClass", "failed to new object from cached UserData");
    } else {
        // will run into here
        __android_log_print(ANDROID_LOG_INFO, "Native FindClass", "success to create new object from cached UserData");
    }
}

void test_with_thread()
{
    JavaVMAttachArgs jvmArgs;
    jvmArgs.version = JNI_VERSION_1_6;
    jvmArgs.name = "CPP_11_Thread";
    jvmArgs.group = NULL;

    JNIEnv *env;
    if (g_pJVM->AttachCurrentThread(&env, &jvmArgs) != JNI_OK) {
        __android_log_print(ANDROID_LOG_INFO, "Native test_with_thread", "failed to attach current thread");
        return;
    }

    __android_log_print(ANDROID_LOG_INFO, "Native test_with_thread", "------ attached JNIEnv");
    runFindClass(env);

    g_pJVM->DetachCurrentThread();
}

JNIEXPORT void JNICALL
Java_cc_aznc_asjni_InvokeByNative_testClassLoader(JNIEnv *env, jobject instance) {
    // java thread
    runFindClass(env);

    // native thread
    std::thread mThread( test_with_thread );
    mThread.join();
}

/*
 * 錯誤示範, stopThread 成為 true 後就永遠維持這個狀態
 * 直到 app process 死掉為止
 */
static bool stopThread = false;
JNIEXPORT void JNICALL
Java_cc_aznc_asjni_InvokeByNative_nativeStartTick(JNIEnv *env, jobject instance, jobject dataForLock)
{
    int tickNum = 0;
    while (++tickNum <= 5) {
        if (stopThread) {
            __android_log_print(ANDROID_LOG_INFO, "Native startNativeTick", "stopThread is true!");
            break;
        }
        env->MonitorEnter(dataForLock);
        __android_log_print(ANDROID_LOG_INFO, "Native startNativeTick", "Tick %d", tickNum);
        env->MonitorExit(dataForLock);
        sleep(1);
    }
}

JNIEXPORT void JNICALL
Java_cc_aznc_asjni_InvokeByNative_nativeStopTick(JNIEnv *env, jobject instance)
{
    stopThread = true;
}

}