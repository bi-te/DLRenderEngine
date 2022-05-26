#pragma once
#include "Mesh.h"

class CubeMesh: public Mesh
{
public:
	CubeMesh()
	{
        //z-
        add_vertex(-0.5f, -0.5f, -0.5f);
        add_vertex(-0.5f, 0.5f, -0.5f);
        add_vertex(0.5f, 0.5f, -0.5f);

        add_vertex(-0.5f, -0.5f, -0.5f);
        add_vertex(0.5f, 0.5f, -0.5f);
        add_vertex(0.5f, -0.5f, -0.5f);


        //z+
        add_vertex(0.5f, -0.5f, 0.5f);
        add_vertex(0.5f, 0.5f, 0.5f);
        add_vertex(-0.5f, 0.5f, 0.5f);

        add_vertex(0.5f, -0.5f, 0.5f);
        add_vertex(-0.5f, 0.5f, 0.5f);
        add_vertex(-0.5f, -0.5f, 0.5f);


        //x-
        add_vertex(-0.5f, -0.5f, 0.5f);
        add_vertex(-0.5f, 0.5f, 0.5f);
        add_vertex(-0.5f, 0.5f, -0.5f);

        add_vertex(-0.5f, -0.5f, 0.5f);
        add_vertex(-0.5f, 0.5f, -0.5f);
        add_vertex(-0.5f, -0.5f, -0.5f);


        //x+
        add_vertex(0.5f, -0.5f, -0.5f);
        add_vertex(0.5f, 0.5f, -0.5f);
        add_vertex(0.5f, 0.5f, 0.5f);

        add_vertex(0.5f, -0.5f, -0.5f);
        add_vertex(0.5f, 0.5f, 0.5f);
        add_vertex(0.5f, -0.5f, 0.5f);


        //y-
        add_vertex(0.5f, -0.5f, -0.5f);
        add_vertex(0.5f, -0.5f, 0.5f);
        add_vertex(-0.5f, -0.5f, 0.5f);

        add_vertex(0.5f, -0.5f, -0.5f);
        add_vertex(-0.5f, -0.5f, 0.5f);
        add_vertex(-0.5f, -0.5f, -0.5f);


        //y+
        add_vertex(-0.5f, 0.5f, -0.5f);
        add_vertex(-0.5f, 0.5f, 0.5f);
        add_vertex(0.5f, 0.5f, 0.5f);

        add_vertex(-0.5f, 0.5f, -0.5f);
        add_vertex(0.5f, 0.5f, 0.5f);
        add_vertex(0.5f, 0.5f, -0.5f);
	}
};
