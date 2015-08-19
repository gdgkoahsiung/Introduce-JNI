package cc.aznc.asjni;

import android.util.Log;

import java.util.Arrays;

public class Sample {

    final UserData dataForLock = new UserData();

    public void run()
    {
        runPassValue();
        runChangeFromNative();
    }

    private void runPassValue() {
        // Primitive type
        PassValue passValue = new PassValue();
        Log.i("Java PassValue", "return val = " + passValue.passBoolean(true));
        Log.i("Java PassValue", "return val = " + passValue.passBoolean(false));
        Log.i("Java PassValue", "return val = " + passValue.passByte(Byte.parseByte("123")));
        Log.i("Java PassValue", "return val = " + passValue.passChar('a'));
        Log.i("Java PassValue", "return val = " + passValue.passShort(Short.MAX_VALUE));
        Log.i("Java PassValue", "return val = " + passValue.passInt(Integer.MAX_VALUE));
        Log.i("Java PassValue", "return val = " + passValue.passLong(Long.MAX_VALUE));
        Log.i("Java PassValue", "return val = " + passValue.passFloat(Float.parseFloat("123.456")));
        Log.i("Java PassValue", "return val = " + passValue.passDouble(Double.MAX_VALUE));

        // String
        Log.i("Java PassValue", "return val = " + passValue.passStringUTF("你好"));
        // ** the implement of revertString may have issue with some string
        Log.i("Java PassValue", "return val = " + passValue.revertString("abc123你好嗎"));

        // change array
        boolean[] boolArrayForChange = new boolean[]{true, false, true};
        Log.i("Java PassValue", "boolArray before change = " + Arrays.toString(boolArrayForChange));
        passValue.changeBooleanArray(boolArrayForChange);
        Log.i("Java PassValue", "boolArray after change = " + Arrays.toString(boolArrayForChange));

        int[] intArrayForChange = new int[]{1, 2, 3};
        Log.i("Java PassValue", "intArray before change = " + Arrays.toString(intArrayForChange));
        passValue.changeIntArray(intArrayForChange);
        Log.i("Java PassValue", "intArray after change = " + Arrays.toString(intArrayForChange));

        String[] stringArray = new String[]{"A.", "B.", "C."};
        Log.i("Java PassValue", "stringArray before change = " + Arrays.toString(stringArray));
        passValue.changeStringArray(stringArray);
        Log.i("Java PassValue", "stringArray after change = " + Arrays.toString(stringArray));

        // create array
        Log.i("Java PassValue", "stringArray after change = " + Arrays.toString(passValue.stringToCharArray("GDG高雄")));
    }

    private void runChangeFromNative()
    {
        final InvokeByNative invokeByNative = new InvokeByNative();
        Log.i("Java InvokeByNative", "getTime() = " + invokeByNative.getTime());

        invokeByNative.handleException();

        try {
            invokeByNative.nativeGiveMeException();
        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            Thread t = new Thread(new Runnable() {
                @Override
                public void run() {
                    invokeByNative.testClassLoader();
                }
            });
            t.start();
            t.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void startTick() {
        Thread t1 = new Thread(new Runnable() {
            @Override
            public void run() {
                InvokeByNative invokeByNative = new InvokeByNative();
                invokeByNative.nativeStartTick(dataForLock);
            }
        });
        t1.start();

        Thread t2 = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    int run = 0;
                    while (++run <= 5) {
                        synchronized (dataForLock) {
                            Thread.sleep(2000);
                        }
                        Thread.sleep(100);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
        t2.start();
    }

    public void stopTick() {
        InvokeByNative invokeByNative = new InvokeByNative();
        invokeByNative.nativeStopTick();
    }
}
