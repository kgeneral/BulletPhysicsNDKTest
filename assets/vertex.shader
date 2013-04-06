//http://www.raywenderlich.com/10862/how-to-create-cool-effects-with-custom-shaders-in-opengl-es-2-0-and-cocos2d-2-x

//galaxy note 2 screen test
// y_ortho = x / y (720/1280=0.5625)
// bottom row -> x, y, z translate
mat4 projection = mat4(0.1, 0.0, 0.0, 0.0,
                       0.0, 0.05625, 0.0, 0.0,
                       0.0, 0.0, 0.1, 0.0,
                       0.0, -0.5, 0.0, 1.0);
mat4 modelview = mat4(1.0);


attribute vec4 vPosition;

attribute vec2 a_TexCoordinate; // Per-vertex texture coordinate information we will pass in.
varying vec2 v_TexCoordinate;   // This will be passed into the fragment shader.

void main() {
    gl_Position = projection * (modelview * vPosition);
    // Pass through the texture coordinate.
	v_TexCoordinate = a_TexCoordinate;
}