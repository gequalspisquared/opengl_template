#pragma once 

#include <vector>

#include "Shader.h"
#include "VertexArray.h"

typedef unsigned int SkyboxId;
typedef std::vector<const char*> SkyboxFacePaths;

class  Skybox {
public: 
    Skybox(const SkyboxFacePaths& paths, Shader* skybox_shader);

    void draw() const;

    SkyboxId m_id;
    Shader* m_shader;
    VertexArray m_vao;
private: 
};