//http://www.learnopengles.com/android-lesson-one-getting-started/
// http://pdtextures.blogspot.kr/2008/03/first-set.html
precision mediump float;

uniform sampler2D u_Texture;    // The input texture.

varying vec2 v_TexCoordinate; // Interpolated texture coordinate per fragment.

void main()
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	//gl_FragColor =  texture2D(u_Texture, v_TexCoordinate);
}