#pragma once

#include "glrenderstage.h"
#include "gltexture.h"
#include "glmesh.h"
#include "ex/glblurstage.h"

class BlurStage : public lithium::RenderStage
{
    inline static std::shared_ptr<lithium::FrameBuffer> createFBO(const glm::vec2& resolution)
    {
        auto fbo = std::make_shared<lithium::FrameBuffer>(resolution, lithium::FrameBuffer::Mode::DEFAULT);
        fbo->bind();
            fbo->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        fbo->unbind();
        return fbo;
    }

public:

    BlurStage(const glm::ivec4& viewport, std::shared_ptr<lithium::Mesh> mesh,
        std::shared_ptr<lithium::Texture<unsigned char>> inputTexture,
        std::shared_ptr<lithium::ShaderProgram> blurShader)
        : RenderStage{nullptr, viewport, [](){ return false; }}, _resolution{inputTexture->width(), inputTexture->height()}, _mesh{mesh}, _inputTexture{inputTexture}, _blurShader{blurShader}
    {
        _vStage = std::make_shared<lithium::ExBlurStage>(createFBO(_resolution), viewport, mesh, _inputTexture, blurShader);
        _hStage = std::make_shared<lithium::ExBlurStage>(createFBO(_resolution), viewport, mesh, _vStage->outputTexture(), blurShader);
    }

    virtual ~BlurStage() noexcept
    {

    }

    virtual void render() override
    {
        _blurShader->use();
        _blurShader->setResolution(glm::vec2(_resolution.x, _resolution.y));
        bool firstIteration{true};
        _vStage->setInputTexture(_inputTexture);
        for(int i{0}; i < 3; ++i)
        {
            _blurShader->setUniform("u_horizontal", 0);
            _vStage->render();
            _vStage->setInputTexture(_hStage->outputTexture());
            _blurShader->setUniform("u_horizontal", 1);
            _hStage->render();
        }
    }

    std::shared_ptr<lithium::Texture<unsigned char>> outputTexture()
    {
        return _hStage->outputTexture();
    }

private:
    const glm::ivec2 _resolution;
    std::shared_ptr<lithium::Mesh> _mesh{nullptr};
    std::shared_ptr<lithium::Texture<unsigned char>> _inputTexture{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _blurShader{nullptr};
    std::shared_ptr<lithium::ExBlurStage> _vStage;
    std::shared_ptr<lithium::ExBlurStage> _hStage;
};