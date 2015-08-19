package cc.aznc.asjni;

public class PassValue {
    static {
        System.loadLibrary("asjni");
    }

    // 基本型別
    public native boolean passBoolean(boolean b);
    public native byte passByte(byte b);
    public native char passChar(char c);
    public native short passShort(short s);
    public native int passInt(int i);
    public native long passLong(long l);
    public native float passFloat(float f);
    public native double passDouble(double d);

    // 字串
    public native String passStringUTF(String s);
    public native String revertString(String s);

    // 陣列
    public native void changeBooleanArray(boolean[] boolArray);
    public native void changeIntArray(int[] intArray);
    public native void changeStringArray(String[] strArray);
    public native char[] stringToCharArray(String str);
}
