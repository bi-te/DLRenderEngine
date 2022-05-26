#pragma once

#include "../math/Intersection.h"
#include "Lighting.h"
#include "Material.h"

float smoothstep(float edge0, float edge1, float x);

void calc_direct_light(vec3& color, const DirectLight& dirlight, const Intersection& record, const vec3& camera_pos, const Material& m);
void calc_point_light(vec3& color, const PointLight& plight, const Intersection& record, const vec3& camera_pos, const Material& m);
void calc_spotlight(vec3& color, const Spotlight& spotlight,const Intersection& record, const vec3& camera_pos, const Material& m);