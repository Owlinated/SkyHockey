#version 420
// Adapted from: https://github.com/NVIDIAGameWorks/GraphicsSamples/blob/master/samples/es3-kepler/FXAA/assets/shaders/FXAA_Default.frag

precision highp float;

uniform sampler2D uSourceTex;
uniform vec2 RCPFrame;
varying vec2 vTexCoord;

void main(void)
{
    // Use rcp frame to prevent errors
    gl_FragColor = vec4(RCPFrame, 0, 0);

    // Pass value straight through
    gl_FragColor = texture2DLod(uSourceTex, vTexCoord, 0.0);
}