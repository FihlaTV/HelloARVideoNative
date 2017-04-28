package io.weichao.util;

import android.content.Context;
import android.media.MediaPlayer;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.IOException;

import cn.easyar.samples.helloarvideo.R;

/**
 * Created by WEI CHAO on 2017/4/9.
 */

public class MediaUtil {
    private static final String TAG = "MediaUtil";

    private static MediaPlayer mMediaPlayer;

    public static void playMp3_1(Context context) {
        Log.i(TAG, "playMp3_1");
        if (mMediaPlayer != null) {
            return;
        }

        if (context != null) {
            mMediaPlayer = getMp3WithRaw(context);
        } else {
            mMediaPlayer = getMp3WithDataSource();
        }
        // 当播放完音频资源时，会触发onCompletion事件，可以在该事件中释放音频资源，以便其他应用程序可以使用该资源:
        mMediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                mp.release();
            }
        });
        mMediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
            @Override
            public void onPrepared(MediaPlayer mp) {
                mp.start();
            }
        });
        try {
            // 在播放之前先判断 mMediaPlayer 是否被占用，否则可能报错：prepareAsync called in state 8
            if (mMediaPlayer != null) {
                mMediaPlayer.stop();
            }
            // 在播放音频资源之前，必须调用 prepare 方法完成些准备工作
            mMediaPlayer.prepare();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalStateException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void stopMp3_1(Context context) {
        Log.i(TAG, "stopMp3_1");
        if (mMediaPlayer != null) {
            try {
                mMediaPlayer.stop();
                mMediaPlayer.release();
                mMediaPlayer = null;
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
            }
        }
    }

    private static MediaPlayer getMp3WithRaw(Context context) {
        return MediaPlayer.create(context, R.raw.the_mass);
    }

    private static MediaPlayer getMp3WithDataSource() {
        MediaPlayer mediaPlayer = new MediaPlayer();
        try {
            mediaPlayer.setDataSource(Environment.getExternalStorageDirectory().getPath() + File.separator + "the_mass.mp3");
        } catch (IOException e) {
            e.printStackTrace();
        }
        return mediaPlayer;
    }
}
