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
#include "blurstage.h"
#include "ex/gltextrenderer.h"

class BasePipeline : public lithium::RenderPipeline
{
    public:
        BasePipeline(const glm::ivec2& resolution);
        
        virtual void render() override;

        lithium::SimpleCamera* camera() const
        {
            return _camera;
        }

        void setLight(lithium::Light* light)
        {
            _light = light;
        }

        void setLightSpaceMatrix(const glm::mat4 lightSpaceMatrix)
        {
            _lightSpaceMatrix = lightSpaceMatrix;
        }

        void setOcean(lithium::Object* ocean)
        {
            _ocean = ocean;
        }


    private:
        lithium::SimpleCamera* _camera{nullptr};
        lithium::UniformBufferObject* _sceneUBO;
        lithium::RenderBuffer *_renderBuffer{nullptr};
        std::shared_ptr<lithium::FrameBuffer> _shadowMapBuffer{nullptr};
        std::shared_ptr<lithium::FrameBuffer> _intermediateFBO{nullptr};
        std::shared_ptr<lithium::FrameBuffer> _frameBuffer{nullptr};
        std::shared_ptr<lithium::FrameBuffer> _borderDepthFBO{nullptr};
        std::shared_ptr<lithium::FrameBuffer> _bloomFBO{nullptr};
        std::shared_ptr<lithium::FrameBuffer> _waterDetectFBO{nullptr};
        std::shared_ptr<lithium::ShaderProgram> _blurShader{nullptr};
        lithium::ShaderProgram* _depthShader{nullptr};
        lithium::ShaderProgram* _depthSkinningShader{nullptr};
        lithium::ShaderProgram* _sdfTextShader{nullptr};
        lithium::ShaderProgram* _debugDepthShader{nullptr};
        lithium::ShaderProgram* _screenMsaaShader{nullptr};
        lithium::ShaderProgram* _borderShader{nullptr};
        lithium::ShaderProgram* _normalShader{nullptr};
        lithium::ShaderProgram* _normalSkinningShader{nullptr};
        lithium::ShaderProgram* _linearDepthProgram{nullptr};
        lithium::ShaderProgram* _colorProgram{nullptr};
        lithium::ShaderProgram* _shaderProgram{nullptr};
        lithium::ShaderProgram* _islandProgram{nullptr};
        lithium::ShaderProgram* _waterProgram{nullptr};
        lithium::ShaderProgram* _skinningShader{nullptr};
        lithium::ShaderProgram* _instShader{nullptr};
        lithium::Object* _ocean{nullptr};
        glm::mat4 _lightSpaceMatrix;
        lithium::Text* _text;
        lithium::Light* _light{nullptr};
        std::shared_ptr<lithium::Mesh> _screenMesh{nullptr};
        lithium::ShaderProgram* _lightShader{nullptr};
        std::shared_ptr<lithium::RenderGroup> _objects;
        std::shared_ptr<lithium::RenderGroup> _staticObjects;
        std::shared_ptr<lithium::RenderGroup> _terrainObjects;
        std::shared_ptr<lithium::RenderGroup> _skinnedObjects;
        std::shared_ptr<lithium::RenderGroup> _textObjects;

        std::shared_ptr<lithium::ExTextRenderer> _textRenderer;

        std::shared_ptr<lithium::RenderStage> _shadowMapStage;
        std::shared_ptr<lithium::RenderStage> _waterDetectStage;
        std::shared_ptr<lithium::RenderStage> _mainStage;
        std::shared_ptr<BlurStage> _blurStage;
        std::shared_ptr<lithium::RenderStage> _depthStage;
        std::shared_ptr<lithium::RenderStage> _finalStage;
};