#include "SnowOverlay.hpp"
#include "Shader.hpp"
#include "SnowScene.hpp"
#include <atlas/utils/Application.hpp>
#include "Asset.hpp"

SnowOverlay::SnowOverlay()
{    
    int factor = 50;

    float stepX = 20.0f / factor;
    float stepZ = 20.0f / factor;

    for(int i = 0; i <= factor; ++i)
    {
        GLfloat z = -10.0f + i*stepZ;
        for(int j = 0; j <= factor; ++j)
        {
            GLfloat x = -10.0f + j*stepX;
            mPositionsAlpha.push_back(glm::vec4(x, 0.005f, z, 0.0f));
        }        
    }

    for(int i = 0; i <= factor-1; ++i)
    {
        for(int j = 0; j <= factor; ++j)
        {
            mIndices.push_back((factor+1)*i + j);
            mIndices.push_back((factor+1)*(i+1) + j);
        }
        mIndices.push_back(0xFFFFFFFF);
    }

    int indexOffset = mPositionsAlpha.size();
    
    //Allow heigt for snow overlay

    for(int i = 0; i <= factor; ++i)
    {
        GLfloat x = -10.0f + i*stepX;
        mPositionsAlpha.push_back(glm::vec4(x, 0.005f, -10.0f, 1.0f));
    }     
    
    for(int i = 0; i <= factor; ++i)
    {
        GLfloat z = -10.0f + i*stepZ;
        mPositionsAlpha.push_back(glm::vec4(10.0f, 0.005f, z, 1.0f));
    } 

    for(int i = 0; i <= factor; ++i)
    {
        GLfloat x = 10.0f - i*stepX;
        mPositionsAlpha.push_back(glm::vec4(x, 0.005f, 10.0f, 1.0f));
    } 

    for(int i = 0; i <= factor; ++i)
    {
        GLfloat z = 10.0f - i*stepZ;
        mPositionsAlpha.push_back(glm::vec4(-10.0f, 0.005f, z, 1.0f));
    } 
    
    mIndices.push_back(0xFFFFFFFF);
    
    //indices
    for(int i = 0; i <= factor; ++i)
    {
        mIndices.push_back(i + indexOffset);
        mIndices.push_back(i);
    }

    indexOffset += factor + 1;
    mIndices.push_back(0xFFFFFFFF);    
    
    for(int i = 0; i <= factor; ++i)
    {
        mIndices.push_back(i + indexOffset);
        mIndices.push_back((factor+1)*i + factor);
    }

    indexOffset += factor + 1;
    mIndices.push_back(0xFFFFFFFF);
    
    for(int i = 0; i <= factor; ++i)
    {
        mIndices.push_back(i + indexOffset);
        mIndices.push_back((factor+1)*factor + factor - i);
    }

    indexOffset += factor + 1;
    mIndices.push_back(0xFFFFFFFF);    
    
    for(int i = 0; i <= factor; ++i)
    {
        mIndices.push_back(i + indexOffset);
        mIndices.push_back((factor+1)*(factor-i));
    }

    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mPositionAlphaBuffer);
    glGenBuffers(1, &mIndexBuffer);    

    glBindVertexArray(mVao);            
	
	glBindBuffer(GL_ARRAY_BUFFER, mPositionAlphaBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4*mPositionsAlpha.size()*sizeof(GLfloat), mPositionsAlpha.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);
       
    glBindVertexArray(0);        
    
    std::vector<atlas::gl::ShaderUnit> shaderUnits
    {
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowOverlay.vert", GL_VERTEX_SHADER),
        atlas::gl::ShaderUnit(generated::Shader::getShaderDirectory() + "/SnowOverlay.frag", GL_FRAGMENT_SHADER)
    };
    
    mShaders.push_back(atlas::gl::Shader(shaderUnits));
    
    mShaders[0].compileShaders();
    mShaders[0].linkShaders(); 
}

SnowOverlay::~SnowOverlay()
{
}


void SnowOverlay::renderGeometry(atlas::math::Matrix4 const &projection, atlas::math::Matrix4 const &view)
{

	mShaders[0].enableShaders();
    
    glm::mat4 modelViewProjection = projection * view * mModel;
	const GLint MODEL_VIEW_PROJECTION_UNIFORM_LOCATION = glGetUniformLocation(mShaders[0].getShaderProgram(), "ModelViewProjection");
    glUniformMatrix4fv(MODEL_VIEW_PROJECTION_UNIFORM_LOCATION, 1, GL_FALSE, &modelViewProjection[0][0]);


    glm::mat4 skyView = glm::lookAt(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 skyProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);            
    glm::mat4 offsetMatrix = glm::mat4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);
    glm::mat4 skyMatrix = offsetMatrix * skyProjection * skyView * mModel;
    const GLint SKY_MATRIX_UNIFORM_LOCATION = glGetUniformLocation(mShaders[0].getShaderProgram(), "SkyMatrix");
    glUniformMatrix4fv(SKY_MATRIX_UNIFORM_LOCATION, 1, GL_FALSE, &skyMatrix[0][0]);
    
    glActiveTexture(GL_TEXTURE1);
    GLint SCENE_SNOW_MAP_UNIFORM_LOCATION = glGetUniformLocation(mShaders[0].getShaderProgram(), "SnowMap");
    GLint SCENE_USE_SNOW_MAP_UNIFORM_LOCATION = glGetUniformLocation(mShaders[0].getShaderProgram(), "UseSnowMap");    
    glUniform1i(SCENE_SNOW_MAP_UNIFORM_LOCATION, 1);
    glUniform1i(SCENE_USE_SNOW_MAP_UNIFORM_LOCATION, true);    
    glBindTexture(GL_TEXTURE_2D, ((SnowScene *) atlas::utils::Application::getInstance().getCurrentScene())->getSnowFall().getSnowDepthTexture());

    glPrimitiveRestartIndex(0xFFFFFFFF);
    glEnable(GL_PRIMITIVE_RESTART);

    glBindVertexArray(mVao);    
    glDrawElements(GL_TRIANGLE_STRIP, mIndices.size(), GL_UNSIGNED_INT, (void *) 0);    
    glBindVertexArray(0);

    glDisable(GL_PRIMITIVE_RESTART);

    mShaders[0].disableShaders();
}

void SnowOverlay::updateGeometry(atlas::core::Time<> const &t)
{
    mNormals = std::vector<glm::vec3>(mPositionsAlpha.size(), glm::vec3(0.0, 0.0, 0.0));    
    bool flip = false;
    for(int i = 0; i < mIndices.size() - 2; ++i)
    {
        
        int index = mIndices[i];
        int index2 = mIndices[i+1];
        int index3 = mIndices[i+2];

        if(index == 0xFFFFFFFF || index2 == 0xFFFFFFFF || index3 == 0xFFFFFFFF)
        {
            flip = false;
            continue;
        }

        glm::vec3 a(mPositionsAlpha[index]);
        glm::vec3 b(mPositionsAlpha[index2]);
        glm::vec3 c(mPositionsAlpha[index3]);
    }

    glBindVertexArray(mVao);            
	glBindBuffer(GL_ARRAY_BUFFER, mPositionAlphaBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4*mPositionsAlpha.size()*sizeof(GLfloat), mPositionsAlpha.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

void SnowOverlay::updateVertexNearestTo(glm::vec3 const &query)
{
    float maxDist = 0.5;
    float factor = 0.001f;
    int i = 0;
    for(glm::vec4 &positionAlpha : mPositionsAlpha)
    {
        if(i >= 51*51)
        {
            break;
        }
        ++i;

        float dist = std::sqrt(
            std::pow(query.x - positionAlpha.x, 2.0f) + 
            std::pow(query.y - positionAlpha.y, 2.0f) + 
            std::pow(query.z - positionAlpha.z, 2.0f)
        );

        float amount = dist > maxDist ? 0.0f : std::min(0.005f, factor/dist);

        positionAlpha.w += amount;

        if(positionAlpha.w >= 1.0f)
        {
            positionAlpha.y += amount;            
        }
    }
}