
#pragma once

#include "glshaderprogram.h"
#include "glrenderpipeline.h"
#include "glsimplecamera.h"
#include "glorthographiccamera.h"
#include "gltext.h"

class Pipeline : public lithium::RenderPipeline
{
public:
    Pipeline(const glm::ivec2& resolution);

    virtual ~Pipeline() noexcept;

    virtual void update(float dt) override;

    virtual void render() override;

private:
    lithium::ShaderProgram* _blockShader{nullptr};
    lithium::ShaderProgram* _borderShader{nullptr};
    lithium::ShaderProgram* _screenShader{nullptr};
    lithium::ShaderProgram* _sdfTextShader{nullptr};
    lithium::SimpleCamera* _camera{nullptr};
    lithium::OrthographicCamera* _orthoCamera;
    lithium::Object* _object;
    lithium::Mesh* _screenMesh;
    lithium::Text* _text;
};