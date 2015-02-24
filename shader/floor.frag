#version 410
uniform sampler2D env;
void main()
{
gl_FragColor = texture2D(env, gl_TexCoord[0].xz);"
}