#pragma once

#include "glrenderpipeline.h"
#include "glsimplecamera.h"
#include "glrenderbuffer.h"
#include "glframebuffer.h"
#include "glshaderprogram.h"
#include "gluniformbufferobject.h"
#include "gllight.h"
#include "glorthographiccamera.h"
#include "gltext.h"

class BasePipeline : public lithium::RenderPipeline
{
    public:
        BasePipeline(const glm::ivec2& resolution);
        
        virtual void update(float dt) override;

        virtual void render() override;

        lithium::SimpleCamera* camera() const
        {
            return _camera;
        }

        void setLight(lithium::Light* light)
        {
            _light = light;
        }

        void renderText(float x, float y, const std::string& str);

        void textColor(const glm::vec3& color)
        {
            _text->setColor(color);
        }

        void textScale(float scale)
        {
            _text->setScale(scale);
        }

    protected:
        lithium::SimpleCamera* _camera{nullptr};
        lithium::OrthographicCamera* _orthoCamera;
        lithium::UniformBufferObject* _sceneUBO;
        lithium::RenderBuffer *_renderBuffer{nullptr};
        lithium::FrameBuffer *_depthMapBuffer{nullptr};
        lithium::ShaderProgram* _depthShader{nullptr};
        lithium::ShaderProgram* _depthSkinningShader{nullptr};
        lithium::ShaderProgram* _sdfTextShader{nullptr};
        lithium::ShaderProgram* _debugDepthShader{nullptr};
        glm::mat4 _lightSpaceMatrix;
        lithium::Text* _text;
        lithium::Light* _light{nullptr};
        lithium::Mesh* _screenMesh{nullptr};
        lithium::ShaderProgram* _lightShader{nullptr};
};