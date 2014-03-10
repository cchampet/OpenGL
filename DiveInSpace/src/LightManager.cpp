#include "LightManager.h"

#include "glm/glm.hpp"

LightManager::LightManager(float nbLights){
	m_nbLights = nbLights;

	m_lightIntensity = 10.0;
}

float* LightManager::getCustomPositionOfLight(float i){
	m_tmpPos[0] = sinf(i) * 10.f;
	m_tmpPos[1] = -0.5f;
	m_tmpPos[2] = cosf(i) * 10.f;
	return m_tmpPos;
}

float* LightManager::getCustomColorOfLight(float i){
	m_tmpColor[0] = sinf(i) *  1.f;
	m_tmpColor[1] = 1.f - cosf(i);
	m_tmpColor[2] = -sinf(i);
	return m_tmpColor;
}

float* LightManager::getRedColor() {
	m_tmpColor[0] = 1.f;
	m_tmpColor[1] = 0.f;
	m_tmpColor[2] = 0.f;
	return m_tmpColor;
}