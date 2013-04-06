package net.dykim1983.bulletphysicsndktest;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class BulletPhysicsActivity extends Activity {
	private static String TAG = "BulletPhysicsActivity";

	BulletPhysicsView mView;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        Log.i(TAG, " onCreate");
        BulletPhysicsLib.initassets(getAssets());
        mView = new BulletPhysicsView(getApplication());
        setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
        
        BulletPhysicsLib.initassets(getAssets());
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
        
        BulletPhysicsLib.initassets(getAssets());
    }
    
    @Override protected void onStop() {
    	Log.i(TAG, "GL2JNIActivity onStop");
    	super.onStop();
    	// clean box2d
    	//BulletPhysicsLib.clean();
    	
    }
    
    @Override
    protected void onStart() {
    	super.onStart();
    	
    	//BulletPhysicsLib.initassets(getAssets());
    }

}
