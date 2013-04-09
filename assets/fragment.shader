//http://www.learnopengles.com/android-lesson-one-getting-started/
// http://pdtextures.blogspot.kr/2008/03/first-set.html

precision mediump float;       // Set the default precision to medium. We don't need as high of a
                               // precision in the fragment shader.
//uniform vec3 u_LightPos;       // The position of the light in eye space.
 
varying vec3 varyPosition;       // This will be passed into the fragment shader.
varying vec4 varyColor;          // This will be passed into the fragment shader.
varying vec3 varyNormal;         // This will be passed into the fragment shader.

uniform sampler2D u_Texture;    // The input texture.

varying vec2 v_TexCoordinate; // Interpolated texture coordinate per fragment.

void main()
{
    vec3 u_LightPos = vec3(0.0, 8.0, 0.0);
	// Will be used for attenuation.
    float distance = length(u_LightPos - varyPosition);
 
    // Get a lighting direction vector from the light to the vertex.
    vec3 lightVector = normalize(u_LightPos - varyPosition);
 
    // Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
    // pointing in the same direction then it will get max illumination.
    float diffuse = max(dot(varyNormal, lightVector), 0.1);
 
    // Add attenuation.
    diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));
 
    // Multiply the color by the diffuse illumination level to get final output color.
    gl_FragColor = varyColor * diffuse;
    
	//gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	//gl_FragColor =  texture2D(u_Texture, v_TexCoordinate);
}