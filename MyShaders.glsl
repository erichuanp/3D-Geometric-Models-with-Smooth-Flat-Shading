// ***************************
// Shader programs to be used in conjunction with the
//  Phong lighting shaders of EduPhong.glsl
// Are first used with Project #6, Math 155A, Winter 2019
//
// Revision: Feb 23, 2019
// ***************************

// #beginglsl ...  #endglsl mark begining and end of code blocks.
// Syntax for #beginglsl is:
//
//   #beginglsl <shader-type> <shader-code-name>
//
// where <shader-type> is
//      vertexshader or fragmentshader or geometryshader or codeblock,
// and <shader-code-name> is used to compile/link the shader into a 
//      shader program.
// A codeblock is meant to be used as a portion of a larger shader.

// *****************************
// applyTextureMap - code block
//    applyTextureMap() is called after the Phong lighting is calculated.
//        - It returns a vec4 color value, which is used as the final pixel color.
//    Inputs: (all global variables)
//        - nonspecColor and specularColor (global variables, vec3 objects)
//        - theTexCoords (the texture coordinates, a vec2 object)
//    Returns a vec4:
//       - Will be used as the final fragment color
// *****************************
#beginglsl codeblock MyProcTexture
// vec3 nonspecColor;		// These items already declared 
// vec3 specularColor;
// vec2 theTexCoords;

uniform sampler2D theTextureMap;	// An OpenGL texture map

bool InFshape( vec2 pos );	// Function prototype

uniform float currentTime;

vec4 applyTextureFunction() {
	vec2 wrappedTexCoords = fract(theTexCoords);	// Wrap s,t to [0,1].
	if ( InFshape(wrappedTexCoords) ) {	
		return vec4( currentTime, currentTime/2, currentTime/3, currentTime/4 );                // Black color inside the "F"
	}
	else {
		vec3 combinedPhongColor = nonspecColor+specularColor;
        return vec4(combinedPhongColor, 1.0f);   // Use the Phong light colors
	}
}

// *******************************
// Recognize the interior of an "F" shape
//   Input "pos" contains s,t  texture coordinates.
//   Returns: true if inside the "F" shape.
//            false otherwise
// ******************************
bool InFshape( vec2 pos ) {
	float margin = 0.2;
	float width = 0.1;
	if ( pos.x<margin || pos.x>1.0-margin ||
	         pos.y<margin || pos.y>1.0-margin) {
		 return false;
    }
	int count = 0;
	float d = 0.01;
	float xp = margin;
	for (float i = margin; i < 1 - margin; i=i+0.05) {
		if ( pos.x >= xp + count * d && pos.x <= xp + count * d + width && pos.y >= i && pos.y <= i+width) {
			return true;
		}
		if ( i > 0.2 && i < 0.7 && pos.x >= xp + count * d + 0.2 && pos.x <= xp + count * d  + 0.2 + width && pos.y >= i  && pos.y <= i+width) {
			return true;
		}
		if ( pos.x >= xp + count * d + 0.4 && pos.x <= xp + count * d  + 0.4 + width && pos.y >= i  && pos.y <= i+width) {
			return true;
		}
		count++;
	}


	return false;
}
#endglsl
