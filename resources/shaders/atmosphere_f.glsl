#version 430

#define PI 3.141592653589

struct SunlightProperties
{
    vec3 sun_direction;
    float sun_luminance;    // luminance of the sun just before it hits the atmosphere
};


uniform sampler2D normal_depth_tx2D;
uniform vec3 camera_position;

uniform SunlightProperties suns[10];
uniform int sun_count;

uniform sampler3D rayleigh_inscatter_tx3D;
uniform sampler3D mie_inscatter_tx3D;

uniform float max_altitude[128];
uniform float min_altitude[128];
uniform vec3 atmosphere_center[128];

in vec3 position;
flat in int instanceID;
in vec4 deviceCoords;

layout (location = 0) out vec4 frag_colour;

struct Ray
{
	vec3 origin;
	vec3 direction;
};

float square(in float base)
{
	return base*base;
}

bool intersectAtmosphere(in Ray ray,in out vec2 d)
{

	/*	view ray parameters */
	vec3 pos = ray.origin - atmosphere_center[instanceID];
	vec3 dir = ray.direction;
	
	/* now solve linear equation for line circle intersection */
	float e = 0.001f; //epsilon value
	float a = dot(dir,dir);
	float b = dot(pos,dir);
	float c = dot(pos,pos) - square(max_altitude[instanceID]+e);
	
	
	float diskr = square(b) - a*c;
	
	if(diskr < 0.0) return false;
	
	d.x = (-b + sqrt(diskr))/a;
	d.y = (-b - sqrt(diskr))/a;
	
	return true;
}

/*
*	Compute Rayleigh phase function
*/
float rayleighPhaseFunction(in float scattering_angle)
{
	return (3.0 / (16.0 * PI)) * (1.0 + square(scattering_angle));
	//return 0.8*( (7.0/5.0) + 0.5*cos(scattering_angle));
}

/*
*	Compute Mie phase function
*/
float miePhaseFunction(in float scattering_angle, in float mieG)
{
	return 1.5 * 1.0 / (4.0 * PI) * (1.0 - mieG*mieG) * pow(1.0 + (mieG*mieG) - 2.0*mieG*scattering_angle, -3.0/2.0) * (1.0 + square(scattering_angle)) / (2.0 + mieG*mieG);
	//return ( (3.0*(1.0-square(g))) / (2.0*(2.0+square(g))) ) * ( (1.0 + square(cos(scattering_angle))) / pow(1.0+square(g)-2.0*g*cos(scattering_angle),1.5) );
}

/*
*	Access the precomputed inscatter table.
*	Parametrization is a mix between the one proposed by Bruneton and Neyret, by Elek and my own tweaks. 
*/
vec4 accessInscatterTexture(sampler3D inscatter_tx3D, float altitude, float viewZenith, float sunZenith, float viewSun)
{
    float H = sqrt(square(max_altitude[instanceID]) - square(min_altitude[instanceID]));
    float rho = sqrt(square(altitude) - square(min_altitude[instanceID]));
	
	float rmu = altitude * viewZenith;
    float delta = square(rmu) - square(altitude) + square(min_altitude[instanceID]);
	
	float res_mu = 128.0;
	float res_mu_s = 32.0;
	float res_r = 32.0;
	float res_nu = 8.0;
	
    vec4 cst = rmu < 0.0 && delta > 0.0 ? vec4(1.0, 0.0, 0.0, 0.5 - 0.5 / res_mu) : vec4(-1.0, H * H, H, 0.5 + 0.5 / res_mu);
	float uR = 0.5 / res_r + rho / H * (1.0 - 1.0 / res_r);
    //float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / res_mu);
	float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / res_mu);
	/*	for now, use a simpler mapping (as proposed by Elek) */
	uMu = (1.0 + viewZenith)/2.0;
	 // paper formula
    float uMuS = 0.5 / res_mu_s + max((1.0 - exp(-3.0 * sunZenith - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / res_mu_s);
    // better formula
    //float uMuS = 0.5 / res_mu_s + (atan(max(sunZenith, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / res_mu_s);
	
    //float lerp = (viewSun + 1.0) / 2.0 * (res_nu - 1.0);
    //float uNu = floor(lerp);
    //lerp = lerp - uNu;
	
	//making my own calculation for the x access coordinate...the original just won't work
	float x_coord = max((1.0 - exp(-3.0 * sunZenith - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / res_mu_s);
	float lerp = mod(x_coord,1.0 / res_mu_s);
	x_coord += viewSun * (1.0/res_mu_s);
	float x_coord_l = x_coord - lerp;
	float x_coord_u = x_coord + ((1.0 / res_mu_s)-lerp);
	lerp /= (1.0/res_mu_s);
	return texture(inscatter_tx3D, vec3(x_coord_l, uMu, uR)) * (1.0 - lerp) +
					texture(inscatter_tx3D, vec3(x_coord_u, uMu, uR)) * lerp;
	
	//return texture3D(inscatter_tx3D, vec3((uNu + uMuS) / res_nu, uMu, uR)) * (1.0 - lerp) +
    //       texture3D(inscatter_tx3D, vec3((uNu + uMuS + 1.0) / res_nu, uMu, uR)) * lerp;
}

/*
*	Compute the color of the sky using the precomputed tables and phase functions
*/
vec3 computeSkyColour(float viewSun, float altitude, float viewZenith, float sunZenith, float sun_luminance)
{
	vec3 rgb_sky = vec3(0.0);
	if(altitude > min_altitude[instanceID]+0.001)
	{
		rgb_sky = rayleighPhaseFunction(viewSun) * accessInscatterTexture(rayleigh_inscatter_tx3D,altitude,viewZenith,sunZenith,viewSun).xyz;
		rgb_sky += miePhaseFunction(viewSun,0.8) * accessInscatterTexture(mie_inscatter_tx3D,altitude,viewZenith,sunZenith,viewSun).xyz;
		rgb_sky /= 4.0*PI;
        
        rgb_sky *= sun_luminance;
        
        // There seems to be an error somewhere in the atmosphere computation as the sky is too dark
        // when using physical units and realistic values for the sun luminance.
        // Therefore an additional factor is multiplied until the error is found
        rgb_sky *= 20.0;
	}
	
	return max(rgb_sky,vec3(0.0));
}

void main()
{	
    /* get depth value from g-buffer */
	vec2 uvCoords = ((deviceCoords.xy / deviceCoords.w) + 1.0) * 0.5;
	vec4 normal_depth = texture(normal_depth_tx2D,uvCoords);
	float depth = (normal_depth.z + normal_depth.w)/1024.0;
	
	Ray view_ray;
	view_ray.origin = camera_position;
	view_ray.direction = normalize(position-camera_position);
	
	vec3 rgb_linear = vec3(0.0);
	
	vec2 intersections;
	if(!intersectAtmosphere(view_ray,intersections))
	{
		discard;
	}
	
	/*	check for two positive intersections -> camera outside atmosphere */
	if( intersections.x>0.0 && intersections.y>0.0)
		view_ray.origin = view_ray.origin + view_ray.direction * min(intersections.x,intersections.y);
		
	float altitude = length(view_ray.origin - atmosphere_center[instanceID]);
	float viewZenith = dot( normalize(view_ray.direction), normalize(view_ray.origin - atmosphere_center[instanceID]) );
    
    for(int i=0; i<sun_count; i++)
    {
	   float sunZenith = dot( normalize(suns[i].sun_direction), normalize(view_ray.origin - atmosphere_center[instanceID]) );
	   float viewSun = dot( normalize(view_ray.direction), normalize(suns[i].sun_direction) );
	   
	   rgb_linear += computeSkyColour(viewSun,altitude,viewZenith,sunZenith,suns[i].sun_luminance);
       
       /*	fake sun disc */
		if( (abs(viewSun)>0.999) && !(depth > 0.0) )
		{
			float intensity = pow((abs(viewSun)-0.999)/(1.0-0.999),3.0) * suns[i].sun_luminance;
			rgb_linear += vec3(intensity,intensity,intensity) * 0.9;
		}
    }
    
    
	if( depth > 0.0)
	{
	//	vec3 geometry_position = view_ray.origin + depth * view_ray.direction;
	//
	//	altitude = length(geometry_position - atmosphere_center[instanceID]);
	//	viewZenith = dot( normalize(view_ray.direction), normalize(geometry_position - atmosphere_center[instanceID]) );
	//	sunZenith = dot( normalize(sun_direction), normalize(geometry_position - atmosphere_center[instanceID]) );
	//
	//	rgb_linear -= computeSkyColour(viewSun,altitude,viewZenith,sunZenith);
	}
	
	frag_colour = vec4(rgb_linear,1.0);
	
}