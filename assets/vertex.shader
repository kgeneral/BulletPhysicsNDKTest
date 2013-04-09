//http://www.raywenderlich.com/10862/how-to-create-cool-effects-with-custom-shaders-in-opengl-es-2-0-and-cocos2d-2-x

//galaxy note 2 screen test
// y_ortho = x / y (720/1280=0.5625)
// bottom row -> x, y, z translate
mat4 projection = mat4(0.1, 0.0, 0.0, 0.0,
                       0.0, 0.05625, 0.0, 0.0,
                       0.0, 0.0, 0.1, 0.0,
                       0.0, -0.1, 0.0, 1.0);
mat4 modelview = mat4(1.0);


attribute vec4 vPosition;     // Per-vertex position information we will pass in.
//attribute vec4 vColor;        // Per-vertex color information we will pass in.
attribute vec3 vNormal;       // Per-vertex normal information we will pass in.
 
varying vec3 varyPosition;       // This will be passed into the fragment shader.
varying vec4 varyColor;          // This will be passed into the fragment shader.
varying vec3 varyNormal;         // This will be passed into the fragment shader.


attribute vec2 a_TexCoordinate; // Per-vertex texture coordinate information we will pass in.
varying vec2 v_TexCoordinate;   // This will be passed into the fragment shader.

void main() {

	vec4 vColor = vec4(0.5,0.5,0.1,0.2);

	// Transform the vertex into eye space.
    varyPosition = vec3(modelview * vPosition);
    
    // Pass through the color.
    varyColor = vColor;
 
    // Transform the normal's orientation into eye space.
    varyNormal = vec3(modelview * vec4(vNormal, 0.0));
 
    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = projection * (modelview * vPosition);
    
    // Pass through the texture coordinate.
	v_TexCoordinate = a_TexCoordinate;
}