package io.weichao.activity;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;

import cn.easyar.samples.helloarvideo.MainActivity;
import cn.easyar.samples.helloarvideo.R;
import io.weichao.util.PermissionUtil;

/**
 * Created by WEI CHAO on 2017/3/27.
 */

public class SplashActivity extends Activity {
    /**
     * 请求码
     */
    private static final int REQUEST_CODE = 0;
    /**
     * 所需的全部权限
     */
    private static final String[] PERMISSIONS = new String[]{Manifest.permission.READ_EXTERNAL_STORAGE,Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.CAMERA, Manifest.permission.INTERNET};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (Build.VERSION.SDK_INT >= 23 && PermissionUtil.isLackPermissions(this, PERMISSIONS)) {
            //　Android 版本为6.0+，并且缺少权限, 进入权限配置页面
            PermissionActivity.startActivityForResult(this, REQUEST_CODE, PERMISSIONS);
        } else {
            onResumeContinue();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        // 拒绝时, 关闭页面, 缺少主要权限, 无法运行
        if (requestCode == REQUEST_CODE && resultCode == PermissionActivity.PERMISSION_DENIED) {
            finish();
        } else {
            onResumeContinue();
        }
    }

    public void onResumeContinue() {
        startActivity(new Intent(this, MainActivity.class));
        finish();
    }
}
