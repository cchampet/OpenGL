#ifndef _H_LIGHT_MANAGER_H_
#define _H_LIGHT_MANAGER_H_

/**
* Manage lights.
*/
struct LightManager
{
	LightManager(float nbLights);

	/**
	* Getters
	*/
	float getIntensityOfLights() const {return m_lightIntensity;}
	float getNbLights() const {return m_nbLights;}
	float* getNbLightsAdress() {return &m_nbLights;} //useful for GUI

	float* getCenterPosition();
	float* getRedColor();

	/**
	* Getters custom
	*/
	float* getCustomPositionOfLight(float i);
	float* getCustomColorOfLight(float i);

private:
	float m_nbLights;
	
	/**
	* Intensity of all lights.
	*/
    float m_lightIntensity;

    /**
    * Tmp data for the current light we want to compute.
    */
    float m_tmpPos[3];
    float m_tmpColor[3];
};

#endif