#ifndef SnowOverlay_hpp
#define SnowOverlay_hpp

#include <atlas/utils/Geometry.hpp>
#include <vector>

class SnowOverlay : public atlas::utils::Geometry
{
    public:

        SnowOverlay();
        ~SnowOverlay();

        void renderGeometry(atlas::math::Matrix4 const &projection, atlas::math::Matrix4 const &view) override;  
        void updateGeometry(atlas::core::Time<> const &t) override;            

        void updateVertexNearestTo(glm::vec3 const &query);        
                        
    private:
        
        GLuint mVao;
        GLuint mPositionAlphaBuffer, mNormalBuffer, mIndexBuffer;               
        
        int mNumVertices;

        std::vector<glm::vec4> mPositionsAlpha;
        std::vector<glm::vec3> mNormals; 
        std::vector<GLuint> mIndices;     
};

#endif