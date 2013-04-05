package net.dykim1983.bulletphysicsndktest;

import android.app.Activity;
import android.os.Bundle;

public class BulletPhysicsActivity extends Activity {

	BulletPhysicsView mView;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new BulletPhysicsView(getApplication());
	setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }

}
