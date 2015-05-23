package com.fotomaterjal.timelapseexpress;

/**
 * Created by Oll on 8.03.2015.
 */
public class keyFrame{
    int mDistance;
    int mHorDeg;
    int mVerDeg;
    int mTime;

    public keyFrame() {
        mDistance = 0;
        mHorDeg = 0;
        mVerDeg = 0;
        mTime = 0;
    }

    public keyFrame(int mDistance, int mHorDeg, int mVerDeg, int mTime) {
        this.mDistance = mDistance;
        this.mHorDeg = mHorDeg;
        this.mVerDeg = mVerDeg;
        this.mTime = mTime;
    }
}
