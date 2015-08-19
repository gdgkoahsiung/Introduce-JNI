package cc.aznc.asjni;

import android.util.Log;

public class InvokeByNative {
    static {
        System.loadLibrary("asjni");
    }

    protected long timeStamp = 0;

    public long getTime()
    {
        updateTime();
        return timeStamp;
    }

    public native void handleException();
    public native void nativeGiveMeException() throws Exception;
    public native void testClassLoader();
    public native void nativeStartTick(UserData dataForLock);
    public native void nativeStopTick();
    protected native void updateTime();

    protected void addLog(String msg)
    {
        Log.i("Java InvokeByNative", msg);
    }
    protected void giveMeException() throws Exception {
        throw new Exception("surprise exception !");
    }
}
