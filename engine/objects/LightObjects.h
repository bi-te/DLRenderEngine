#pragma once
#include "SphereObject.h"
#include "../render/Lighting.h"

class PointLightObject: public SphereObject
{
public :
	PointLight plight;
};

class SpotlightObject: public  SphereObject
{
public:
	Spotlight spotlight;
};