#ifndef _H_IMGUI_H_
#define _H_IMGUI_H_

// Font buffers
extern const unsigned char DroidSans_ttf[];
extern const unsigned int DroidSans_ttf_len;    


struct GUIStates
{
    bool panLock;
    bool turnLock;
    bool zoomLock;
    int lockPositionX;
    int lockPositionY;
    int camera;
    double time;
    bool playing;
    static const float MOUSE_PAN_SPEED;
    static const float MOUSE_ZOOM_SPEED;
    static const float MOUSE_TURN_SPEED;
};


const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;


void init_gui_states(GUIStates & guiStates){
    guiStates.panLock = false;
    guiStates.turnLock = false;
    guiStates.zoomLock = false;
    guiStates.lockPositionX = 0;
    guiStates.lockPositionY = 0;
    guiStates.camera = 0;
    guiStates.time = 0.0;
    guiStates.playing = false;
}

void init_imgui(){
	if (!imguiRenderGLInit(DroidSans_ttf, DroidSans_ttf_len)){
        fprintf(stderr, "Could not init GUI renderer.\n");
        exit(EXIT_FAILURE);
    }
}

void drawGUI(int width, int height, ShaderManager& shaderManager, LightManager& lightManager, float fps, int leftButton){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);

    unsigned char mbut = 0;
    int mscroll = 0;
    int mousex; int mousey;
    glfwGetMousePos(&mousex, &mousey);
    mousey = height - mousey;

    if( leftButton == GLFW_PRESS )
        mbut |= IMGUI_MBUT_LEFT;

    imguiBeginFrame(mousex, mousey, mbut, mscroll);
    
        int logScroll_1 = 0;
        char lineBuffer[512];
        imguiBeginScrollArea("Dive In Space", width - 210, height - 350, 200, 300, &logScroll_1);
            sprintf(lineBuffer, "FPS %f", fps);
            imguiLabel(lineBuffer);
            imguiSlider("Gamma", shaderManager.getGamma(), 0.0, 3.0, 0.1);
            imguiSlider("Sobel", shaderManager.getSobelCoeff(), 0.0, 1.0, 1.0);
            imguiSlider("Blur Samples", shaderManager.getBlurSamples(), 1.0, 100.0, 1.0);
            imguiSlider("Focus plane", shaderManager.getFocusPlane(), 1.0, 100.0, 1.0);
            imguiSlider("Near plane", shaderManager.getNearPlane(), 1.0, 100.0, 1.0);
            imguiSlider("Far plane", shaderManager.getFarPlane(), 1.0, 100.0, 1.0);
        imguiEndScrollArea();

        int logScroll_2 = 1;
        imguiBeginScrollArea("Lights", 10, height - 500, 200, 450, &logScroll_2);
            sprintf(lineBuffer, "%d PointLights", lightManager.getNumPointLight());
            imguiLabel(lineBuffer);
            int button_addPL = imguiButton("Add PointLight");
            if(button_addPL)
            {
                unsigned int nbL = lightManager.getNumPointLight();
                srand(time(NULL));
                lightManager.addPointLight( glm::vec3(0, 15, 5),
                                            glm::vec3(cos(nbL), sin(nbL), 1),
                                            glm::vec3(1, 1, 1),
                                            5.f);
            }

            for (unsigned int i = 0; i < lightManager.getNumPointLight(); ++i)
            {
                std::ostringstream ss;
                ss << (i+1);
                std::string s(ss.str());
                int toggle = imguiCollapse("Point Light", s.c_str(), lightManager.getPLCollapse(i));
                if(lightManager.getPLCollapse(i))
                {
                    imguiIndent();
                        float intens = lightManager.getPLIntensity(i);
                        imguiSlider("Intensity", &intens, 0, 10, 0.1); lightManager.setPLIntensity(i, intens);
                        imguiLabel("Position :");
                        imguiIndent();
                            glm::vec3 pos = lightManager.getPLPosition(i);
                            imguiSlider("x", &pos.x, -30, 30, 0.1);
                            imguiSlider("y", &pos.y, -30, 30, 0.1);
                            imguiSlider("z", &pos.z, -30, 30, 0.1);
                            lightManager.setPLPosition(i, pos);
                        imguiUnindent();
                        imguiLabel("Diffuse :");
                        imguiIndent();
                            glm::vec3 diff = lightManager.getPLDiffuse(i);
                            imguiSlider("r", &diff.x, 0, 1, 0.01);
                            imguiSlider("g", &diff.y, 0, 1, 0.01);
                            imguiSlider("b", &diff.z, 0, 1, 0.01);
                            lightManager.setPLDiffuse(i, diff);
                        imguiUnindent();
                        imguiLabel("Specular :");
                        imguiIndent();
                            glm::vec3 spec = lightManager.getPLSpec(i);
                            imguiSlider("r", &spec.x, 0, 1, 0.01);
                            imguiSlider("g", &spec.y, 0, 1, 0.01);
                            imguiSlider("b", &spec.z, 0, 1, 0.01);
                            lightManager.setPLSpec(i, spec);
                        imguiUnindent();
                        int removeLight = imguiButton("Remove"); 
                        if(removeLight)
                            lightManager.removePointLight(i);
                    imguiUnindent();
                }
                if(toggle) { lightManager.setPLCollapse(i, !lightManager.getPLCollapse(i)); }
            }

            imguiSeparatorLine();

            sprintf(lineBuffer, "%d DirLights", lightManager.getNumDirLight());
            imguiLabel(lineBuffer);
            int button_addDL = imguiButton("Add DirLight");
            if(button_addDL)
            {
                lightManager.addDirLight( glm::vec3(0.5, -0.5, -0.5),
                                            glm::vec3(1, 1, 1),
                                            glm::vec3(1, 1, 0.5),
                                            0.6f);
            }

            for (unsigned int i = 0; i < lightManager.getNumDirLight(); ++i)
            {
                std::ostringstream ss;
                ss << (i+1);
                std::string s(ss.str());
                int toggle = imguiCollapse("Dir Light", s.c_str(), lightManager.getDLCollapse(i));
                if(lightManager.getDLCollapse(i))
                {
                    imguiIndent();
                        float intens = lightManager.getDLIntensity(i);
                        imguiSlider("Intensity", &intens, 0, 10, 0.1); lightManager.setDLIntensity(i, intens);
                        imguiLabel("Direction :");
                        imguiIndent();
                            glm::vec3 dir = lightManager.getDLDirection(i);
                            imguiSlider("x", &dir.x, -30, 30, 0.1);
                            imguiSlider("y", &dir.y, -30, 30, 0.1);
                            imguiSlider("z", &dir.z, -30, 30, 0.1);
                            lightManager.setDLDirection(i, dir);
                        imguiUnindent();
                        imguiLabel("Diffuse :");
                        imguiIndent();
                            glm::vec3 diff = lightManager.getDLDiffuse(i);
                            imguiSlider("r", &diff.x, 0, 1, 0.01);
                            imguiSlider("g", &diff.y, 0, 1, 0.01);
                            imguiSlider("b", &diff.z, 0, 1, 0.01);
                            lightManager.setDLDiffuse(i, diff);
                        imguiUnindent();
                        imguiLabel("Specular :");
                        imguiIndent();
                            glm::vec3 spec = lightManager.getDLSpec(i);
                            imguiSlider("r", &spec.x, 0, 1, 0.01);
                            imguiSlider("g", &spec.y, 0, 1, 0.01);
                            imguiSlider("b", &spec.z, 0, 1, 0.01);
                            lightManager.setDLSpec(i, spec);
                        imguiUnindent();
                        int removeLight = imguiButton("Remove"); 
                        if(removeLight)
                            lightManager.removeDirLight(i);
                    imguiUnindent();
                }
                if(toggle) { lightManager.setDLCollapse(i, !lightManager.getDLCollapse(i)); }
            }

            imguiSeparatorLine();

            sprintf(lineBuffer, "%d SpotLights", lightManager.getNumSpotLight());
            imguiLabel(lineBuffer);
            int button_addSPL = imguiButton("Add SpotLight");
            if(button_addSPL)
            {
                lightManager.addSpotLight( glm::vec3(5, 5, 5),
                                            glm::vec3(-1, -1, -1),
                                            glm::vec3(1, 1, 0),
                                            glm::vec3(1, 0, 1),
                                            2.f,
                                            1.f,
                                            1.f);
            }

            for (unsigned int i = 0; i < lightManager.getNumSpotLight(); ++i)
            {
                std::ostringstream ss;
                ss << (i+1);
                std::string s(ss.str());
                int toggle = imguiCollapse("Spot Light", s.c_str(), lightManager.getSPLCollapse(i));
                if(lightManager.getSPLCollapse(i))
                {
                    imguiIndent();
                        float intens = lightManager.getSPLIntensity(i);
                        imguiSlider("Intensity", &intens, 0, 10, 0.1); lightManager.setSPLIntensity(i, intens);
                        imguiLabel("Position :");
                        imguiIndent();
                            glm::vec3 pos = lightManager.getSPLPosition(i);
                            imguiSlider("x", &pos.x, -30, 30, 0.1);
                            imguiSlider("y", &pos.y, -30, 30, 0.1);
                            imguiSlider("z", &pos.z, -30, 30, 0.1);
                            lightManager.setSPLPosition(i, pos);
                        imguiUnindent();
                        imguiLabel("Direction :");
                        imguiIndent();
                            glm::vec3 dir = lightManager.getSPLDirection(i);
                            imguiSlider("x", &dir.x, -1, 1, 0.01);
                            imguiSlider("y", &dir.y, -1, 1, 0.01);
                            imguiSlider("z", &dir.z, -1, 1, 0.01);
                            lightManager.setSPLDirection(i, dir);
                        imguiUnindent();
                        imguiLabel("Diffuse :");
                        imguiIndent();
                            glm::vec3 diff = lightManager.getSPLDiffuse(i);
                            imguiSlider("r", &diff.x, 0, 1, 0.01);
                            imguiSlider("g", &diff.y, 0, 1, 0.01);
                            imguiSlider("b", &diff.z, 0, 1, 0.01);
                            lightManager.setSPLDiffuse(i, diff);
                        imguiUnindent();
                        imguiLabel("Specular :");
                        imguiIndent();
                            glm::vec3 spec = lightManager.getSPLSpec(i);
                            imguiSlider("r", &spec.x, 0, 1, 0.01);
                            imguiSlider("g", &spec.y, 0, 1, 0.01);
                            imguiSlider("b", &spec.z, 0, 1, 0.01);
                            lightManager.setSPLSpec(i, spec);
                        imguiUnindent();
                        float extangle = lightManager.getSPLExternalAngle(i);
                        imguiSlider("External angle", &extangle, 0, 2, 0.01); lightManager.setSPLExternalAngle(i, extangle);
                        float intangle = lightManager.getSPLInternalAngle(i);
                        imguiSlider("Internal angle", &intangle, 0, 2, 0.01); lightManager.setSPLInternalAngle(i, intangle);
                        int removeLight = imguiButton("Remove"); 
                        if(removeLight)
                            lightManager.removeSpotLight(i);
                    imguiUnindent();
                }
                if(toggle) { lightManager.setSPLCollapse(i, !lightManager.getSPLCollapse(i)); }
            }
        imguiEndScrollArea();

    imguiEndFrame();

    imguiRenderGLDraw(width, height); 

    glDisable(GL_BLEND);
}

#endif