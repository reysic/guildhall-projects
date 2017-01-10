#version 410 core

uniform vec4 gColor;
uniform sampler2D gDiffuseTex;
uniform sampler2D gNormalTex;
uniform sampler2D gSpecularTex;

uniform vec4 gAmbientLight;
uniform vec4 gLightColor;

uniform vec3 gLightPosition;
uniform float gLightIntensity;
uniform float gSpecularPower;

uniform vec3 g_cameraPosition;


in vec4 passColor;
in vec2 passUV0;
in vec3 passPosition;

out vec4 outColor;

vec3 CalculateSpecularFactor( vec3 normal)
{
	float specular_intensity =  texture(gSpecularTex, passUV0).r;

	vec3 vec_to_light = gLightPosition - passPosition;
	vec3 vec_to_eye = g_cameraPosition - passPosition;
	vec3 half_vector = vec_to_light + vec_to_eye;
	half_vector = normalize(half_vector);

	float half_dot_normal = dot(half_vector, normal);
	float intensity = pow(half_dot_normal, gSpecularPower) * specular_intensity;

	float power = dot(vec_to_light, vec_to_light);
	power = gLightIntensity / power;

	vec4 color = intensity * gLightColor * power;
	return color.rgb;
}

void debugmain (void)
{
	vec3 diff = passPosition - g_cameraPosition;
	float l = abs(diff.y);
	l /= 5.f;
	outColor = vec4(l,l,l,1.f);
}

void main ( void )
{
	//vec2 rb = gl_FragCoord.xy;

	//float len = length( vec2( 800, 600 ) - rb );

	//outColor = vec4 (sin(len / 20.0f), 0.0f, 0.0f, 1.0f);

	vec4 color = texture(gDiffuseTex, passUV0);
	vec3 normal= texture(gNormalTex, passUV0).rgb;
/*
	if (passUV0.x > 0.5f)
	{
		outColor = color;
	}
	else
	{
		outColor = normal;
	}
*/
	vec3 light_intensity = gAmbientLight.rgb * gAmbientLight.a;

	//vec3 normal = vec3(0.f,-1.f,0.f);//negative your forward
	normal = normal * vec3(2.f,2.0f,1.f) - vec3(1.f,1.f,0.f);

	vec3 final_normal = vec3(normal.x, -normal.z, normal.y);
	final_normal = normalize(final_normal);


	vec3 vec_to_light = gLightPosition - passPosition;
	vec_to_light = normalize(vec_to_light);

	float inv_len = dot(vec_to_light, vec_to_light);
	inv_len = inversesqrt(inv_len);
	vec_to_light = vec_to_light * inv_len;
	float power = gLightIntensity * (inv_len * inv_len);

	float n_dot_l = dot(final_normal, vec_to_light);
	light_intensity += vec3(n_dot_l) * gLightColor.rgb * power;
	


	light_intensity = clamp(light_intensity, vec3(0.0f), vec3(1.f));
	vec3 spec_factor = CalculateSpecularFactor(final_normal);
	outColor =  vec4(light_intensity, 1.0f) * color + vec4(spec_factor, 1.f);
	outColor = clamp(outColor, vec4(0.f), vec4(1.f));
	
	//outColor = vec4(passUV0, 0.f, 1.f);
	//outColor = gColor;
}

