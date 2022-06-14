#pragma once

#include "../math/Intersection.h"
#include "Lighting.h"
#include "Material.h"

float smoothstep(float edge0, float edge1, float x);

// Blinn-Phong 
void calc_direct_light(vec3& color, const DirectLight& dirlight, const Intersection& record, const vec3& camera_pos, const Material& m);
void calc_point_light(vec3& color, const PointLight& plight, const Intersection& record, const vec3& camera_pos, const Material& m);
void calc_spotlight(vec3& color, const Spotlight& spotlight,const Intersection& record, const vec3& camera_pos, const Material& m);

// Cook-Torrance
float ggx_distribution(float alpha, float cosH);
float ggx_smith(float alpha, float cosV, float cosL);
vec3 fresnel(const vec3& f0, float cosL);
void cook_torrance_aprox(vec3& color, const vec3& l, const vec3& cl, const vec3& n, 
	const vec3& v, const vec3& radiance, float dw, const Material& m);
void cook_torrance(vec3& color, const vec3& l, const vec3& n, const vec3& v,
	const vec3& radiance, float dw, const Material& m);

// PBR
void calc_direct_light_pbr(vec3& color, const DirectLight& dirlight, const vec3& norm,
	const vec3& light_vec, const vec3& view, const Material& m);
void calc_point_light_pbr(vec3& color, vec3 light_vec, const vec3& light, float light_dist, float radius,
	const vec3& norm, const vec3& view, const Material& m);
void calc_spotlight_pbr(vec3& color, const Spotlight& spotlight, float radius,
	const Intersection& record, const vec3& view, const Material& m);

// Image processing
void adjust_exposure(vec3& color, float ev100);
void aces_tonemap(vec3& color);
void gamma_correction(vec3& color);