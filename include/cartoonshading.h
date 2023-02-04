
#pragma once

#include "basepipeline.h"
#include "glparticlesystem.h"

class CartoonShading : public BasePipeline
{
public:
    CartoonShading(const glm::ivec2& resolution);
    virtual ~CartoonShading() noexcept;

    virtual void update(float dt) override;

    virtual void render() override;

    virtual void beforeDiffusePass()
    {
        
    }

    virtual void afterDiffusePass()
    {
        
    }

    virtual void beforeBorderPass()
    {
        
    }

    virtual void afterBorderPass()
    {
        
    }

    lithium::ShaderProgram* shaderProgram() const
    {
        return _shaderProgram;
    }

protected:
    lithium::ShaderProgram* _instShader{nullptr};

private:
    lithium::ShaderProgram* _screenMsaaShader{nullptr};
    lithium::ShaderProgram* _blurShader{nullptr};
    lithium::FrameBuffer *_frameBuffer{nullptr};
    lithium::FrameBuffer *_borderFBO{nullptr};
    lithium::FrameBuffer *_borderDepthFBO{nullptr};
    lithium::FrameBuffer *_bloomFBO{nullptr};
    lithium::FrameBuffer *_horizontalBlurFBO{nullptr};
    lithium::FrameBuffer *_verticalBlurFBO{nullptr};
    lithium::FrameBuffer *_intermediateFBO{nullptr};
    lithium::ShaderProgram* _borderShader{nullptr};
    lithium::ShaderProgram* _normalShader{nullptr};
    lithium::ShaderProgram* _normalSkinningShader{nullptr};
    lithium::ShaderProgram* _colorProgram{nullptr};
    lithium::ShaderProgram* _shaderProgram{nullptr};
    lithium::ShaderProgram* _bloomProgram{nullptr};
    lithium::ShaderProgram* _skinningShader{nullptr};
};