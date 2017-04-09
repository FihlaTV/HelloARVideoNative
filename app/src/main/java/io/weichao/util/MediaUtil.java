package io.weichao.util;

import android.media.MediaPlayer;
import android.os.Environment;

import java.io.File;
import java.io.IOException;

/**
 * Created by WEI CHAO on 2017/4/9.
 */

public class MediaUtil  {
    private static MediaPlayer mMediaPlayer;

    public static void playMp3_1() {
        if (mMediaPlayer != null) {
            return;
        }

        mMediaPlayer = getMp3WithDataSource();
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
            // 在播放音频资源之前，必须调用Prepare方法完成些准备工作
            mMediaPlayer.prepare();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalStateException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

//    private static MediaPlayer getMp3WithRaw(Context context) {
//        MediaPlayer mediaPlayer = MediaPlayer.create(context, R.raw.the_mass);
//        return mediaPlayer;
//    }

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
