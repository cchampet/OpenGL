#ifndef _H_LIGHT_MANAGER_H_
#define _H_LIGHT_MANAGER_H_

#include <vector>
#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3

/**
* Manage lights.
*/
class LightManager
{

struct PointLight
{
	glm::vec3 pos;
	glm::vec3 diffcolor;
	glm::vec3 speccolor;
	float intensity;
	bool guicollapse;
	PointLight(glm::vec3 pos, glm::vec3 dif, glm::vec3 spec, float inte, bool coll) : pos(pos), diffcolor(dif), speccolor(spec), intensity(inte), guicollapse(coll) {}
};

struct DirLight
{
	glm::vec3 dir;
	glm::vec3 diffcolor;
	glm::vec3 speccolor;
	float intensity;
	bool guicollapse;
	DirLight(glm::vec3 dir, glm::vec3 dif, glm::vec3 spec, float inte, bool coll) : dir(dir), diffcolor(dif), speccolor(spec), intensity(inte), guicollapse(coll) {}
};

struct SpotLight
{
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 diffcolor;
	glm::vec3 speccolor;
	float intensity;
	float extangle;
	float intangle;
	bool guicollapse;
	SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 dif, glm::vec3 spec, float intensity, float extA, float intA, float inte, bool coll)
		: pos(pos), dir(dir), diffcolor(dif), speccolor(spec), intensity(inte), extangle(extA), intangle(intA), guicollapse(coll) {}
};

public:

	LightManager() {}

	//
	// Manager point lights
	//
	void addPointLight(glm::vec3 pos, glm::vec3 diffColor, glm::vec3 specColor, float intensity)
	{
		PointLight p(pos, diffColor, specColor, intensity, false);
		m_pointLights.push_back(p);
	}

	void removePointLight(size_t i)
	{
		m_pointLights.erase(m_pointLights.begin() + i);
	}

	unsigned int getNumPointLight() const { return m_pointLights.size(); }

	void setPLPosition(size_t i, glm::vec3 pos) { m_pointLights[i].pos = pos; }
	void setPLDiffuse(size_t i, glm::vec3 col) { m_pointLights[i].diffcolor = col; }
	void setPLSpec(size_t i, glm::vec3 col) { m_pointLights[i].speccolor = col; }
	void setPLIntensity(size_t i, float intensity) { m_pointLights[i].intensity = intensity; }
	void setPLCollapse(size_t i, bool col) { m_pointLights[i].guicollapse = col; }

	glm::vec3 getPLPosition(size_t i) const { return m_pointLights[i].pos; }
	glm::vec3 getPLDiffuse(size_t i) const { return m_pointLights[i].diffcolor; }
	glm::vec3 getPLSpec(size_t i) const { return m_pointLights[i].speccolor; }
	float getPLIntensity(size_t i) const { return m_pointLights[i].intensity; }
	bool getPLCollapse(size_t i) const { return m_pointLights[i].guicollapse; }

	//
	// Manager directional lights
	//
	void addDirLight(glm::vec3 dir, glm::vec3 diffColor, glm::vec3 specColor, float intensity)
	{
		DirLight d(dir, diffColor, specColor, intensity, true);
		m_dirLights.push_back(d);
	}

	void removeDirLight(size_t i)
	{
		m_dirLights.erase(m_dirLights.begin() + i);
	}

	unsigned int getNumDirLight() const { return m_dirLights.size(); }

	void setDLDirection(size_t i, glm::vec3 dir) { m_dirLights[i].dir = dir; }
	void setDLDiffuse(size_t i, glm::vec3 col) { m_dirLights[i].diffcolor = col; }
	void setDLSpec(size_t i, glm::vec3 col) { m_dirLights[i].speccolor = col; }
	void setDLIntensity(size_t i, float intensity) { m_dirLights[i].intensity = intensity; }
	void setDLCollapse(size_t i, bool col) { m_dirLights[i].guicollapse = col; }

	glm::vec3 getDLDirection(size_t i) const { return m_dirLights[i].dir; }
	glm::vec3 getDLDiffuse(size_t i) const { return m_dirLights[i].diffcolor; }
	glm::vec3 getDLSpec(size_t i) const { return m_dirLights[i].speccolor; }
	float getDLIntensity(size_t i) const { return m_dirLights[i].intensity; }
	bool getDLCollapse(size_t i) const { return m_dirLights[i].guicollapse; }

	//
	// Manager spot lights
	//
	void addSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 dif, glm::vec3 spec, float extA, float intA, float intensity)
	{
		SpotLight s(pos, dir, dif, spec, intensity, extA, intA, intensity, true);
		m_spotLight.push_back(s);
	}

	void removeSpotLight(size_t i)
	{
		m_spotLight.erase(m_spotLight.begin() + i);
	}

	unsigned int getNumSpotLight() const { return m_spotLight.size(); }

	void setSPLPosition(size_t i, glm::vec3 pos) { m_spotLight[i].pos = pos; }
	void setSPLDirection(size_t i, glm::vec3 dir) { m_spotLight[i].dir = dir; }
	void setSPLDiffuse(size_t i, glm::vec3 col) { m_spotLight[i].diffcolor = col; }
	void setSPLSpec(size_t i, glm::vec3 col) { m_spotLight[i].speccolor = col; }
	void setSPLIntensity(size_t i, float intensity) { m_spotLight[i].intensity = intensity; }
	void setSPLExternalAngle(size_t i, float angle) { m_spotLight[i].extangle = angle; }
	void setSPLInternalAngle(size_t i, float angle) { m_spotLight[i].intangle = angle; }
	void setSPLCollapse(size_t i, bool col) { m_spotLight[i].guicollapse = col; }

	glm::vec3 getSPLPosition(size_t i) const { return m_spotLight[i].pos; }
	glm::vec3 getSPLDirection(size_t i) const { return m_spotLight[i].dir; }
	glm::vec3 getSPLDiffuse(size_t i) const { return m_spotLight[i].diffcolor; }
	glm::vec3 getSPLSpec(size_t i) const { return m_spotLight[i].speccolor; }
	float getSPLIntensity(size_t i) const { return m_spotLight[i].intensity; }
	float getSPLExternalAngle(size_t i) const { return m_spotLight[i].extangle; }
	float getSPLInternalAngle(size_t i) const { return m_spotLight[i].intangle; }
	bool getSPLCollapse(size_t i) const { return m_spotLight[i].guicollapse; }


	//
	// Manage specific scenes
	//
	void createHalLights(){
		// Core
		addSpotLight(glm::vec3(0.f, 0.f, 0.f),	// Position
					glm::vec3(0.f, 0.f, -1.f),	// Direction
					glm::vec3(1.f, 0.3f, 0.f),	// Diffuse
					glm::vec3(1.f, 0.f, 0.f), 	// Specular
					0.2f,						// External angle
					0.f,						// Internal angle
					10.f);						// Intensity

		// Halo
		addSpotLight(glm::vec3(0.f, 0.f, 0.f),	// Position
			glm::vec3(0.f, 0.f, -1.f),			// Direction
			glm::vec3(1.f, 0.f, 0.f),			// Diffuse
			glm::vec3(1.f, 0.f, 0.f),			// Specular
			1.5f,								// External angle
			0.4f,								// Internal angle
			1.f);								// Intensity
	}

	void createTravel1Lights(){
		// Ambiante white light, to see the sphere
		addDirLight(glm::vec3(0.f, -10.f, -15.f), 
					glm::vec3(0.2f, 0.2f, 1.f), 
					glm::vec3(0.2f, 0.2f, 0.2f), 
					2);
		//white light, from the monolythe
		addDirLight(glm::vec3(0.f, -10.f, 0.f), 
					glm::vec3(1.f, 1.f, 1.f), 
					glm::vec3(0.2f, 0.2f, 0.2f), 
					10);
	}

	void updateHalLights(double t){
		// Between 0.5 and 1.5
		setSPLExternalAngle(1, 1.7 + cos(t));
	}

	void updateTravel1Lights(double t){
		glm::vec3 oldColor = getDLDiffuse(0);
		glm::vec3 newColor = glm::vec3((oldColor.r + cos(t))/2.f, (oldColor.g + sin(t))/2.f, 0.5f+(oldColor.b + cos(t))/2.f);
		setDLDiffuse(0, newColor);
	}


private:
	std::vector<PointLight> m_pointLights;
	std::vector<DirLight> m_dirLights;
	std::vector<SpotLight> m_spotLight;
};


#endif