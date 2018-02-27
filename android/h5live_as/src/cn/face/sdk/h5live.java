package cn.face.sdk;



public class h5live {

	static h5live camera = null;
	
	public h5live() {
		FaceCommon.loadLibrarys();
	}

	public static h5live getInstance() {

		if (null == camera) {
			camera = new h5live();
		}
		return camera;
	}

	static public native int H5liveStart(String root, int port);	
	static public native int H5livePush(String channel, byte[] data);
	static public native int H5liveStop();
	static public native int H5liveAddStream(String channel);
	static public native int H5liveRemoveStream(String channel);
	static public native int H5liveGetClientNum(String channel);

}
