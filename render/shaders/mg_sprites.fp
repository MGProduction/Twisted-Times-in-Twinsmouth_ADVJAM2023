varying mediump vec4 position;
varying mediump vec2 var_texcoord0;

uniform lowp sampler2D DIFFUSE_TEXTURE;
uniform lowp vec4 resolution;
uniform vec4 u_tone;
uniform lowp vec4 tint;
uniform lowp vec4 efx;

// https://www.shadertoy.com/view/XdXXD4

vec3 Sepia(vec3 inputColor)
{
	const mat3 Mat = mat3(
		0.393, 0.769, 0.189,
		0.349, 0.686, 0.168,
		0.272, 0.534, 0.131);
		return inputColor * Mat;
	}
	
void main()
{
	vec4 tint_pm = vec4(tint.xyz * tint.w, tint.w);
	vec4 col = texture2D(DIFFUSE_TEXTURE, var_texcoord0.xy) * tint_pm;
	if(efx.a==1)
		gl_FragColor = vec4(col.rgb,1.0);
	else
		gl_FragColor = vec4(Sepia(col.rgb),col.a);

	/*
	//create our sepia tone from some constant value
	vec3 sepiaColor = vec3(gray) * SEPIA;

	//again we'll use mix so that the sepia effect is at 75%
	texColor.rgb = mix(texColor.rgb, sepiaColor, 0.75);

	//final colour, multiplied by vertex colour
	gl_FragColor = texColor * vColor;
	*/

	//float gray = dot(col, vec4(0.299, 0.587, 0.114, 0));
	//vec4 shade = vec4(gray * u_tone.rgb, col.a);
	
	//gl_FragColor = mix(shade, col, u_tone.a);	
	//float gray = 0.21 * col.r + 0.71 * col.g + 0.07 * col.b;
	//gl_FragColor = vec4(col.rgb * (1.0 - u_colorFactor) + (gray * u_colorFactor), col.a);
}
/*
void main()
{
	float height=2000;//resolution.y
	vec2 uv = var_texcoord0.xy;
	vec4 col = texture2D(DIFFUSE_TEXTURE, uv );

	// scanline
	float scanline = sin(uv.y*height)*0.04;
	col -= scanline;

	gl_FragColor = col;
}*/