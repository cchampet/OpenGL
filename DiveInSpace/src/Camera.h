#ifndef _H_CAMERA_H_
#define _H_CAMERA_H_

#include "glm/glm.hpp"

struct Camera
{
	void camera_compute();
	void camera_defaults();
	void camera_zoom(float factor);
	void camera_turn(float phi, float theta);
	void camera_pan(float x, float y);

    float m_radius;
    float m_theta;
    float m_phi;
    glm::vec3 m_o;
    glm::vec3 m_eye;
    glm::vec3 m_up;
};


#endif