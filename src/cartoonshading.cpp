#include "cartoonshading.h"
#include "gltinyobjloader.h"
#include "assetfactory.h"

CartoonShading::CartoonShading(const glm::ivec2& resolution) : BasePipeline{resolution}
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    _linearDepthProgram = new lithium::ShaderProgram( "shaders/cartoon.vert", "shaders/borderinput.frag" );
    _colorProgram = new lithium::ShaderProgram( "shaders/cartoon.vert", "shaders/color.frag" );

    _shaderProgram = new lithium::ShaderProgram( "shaders/cartoon.vert", "shaders/cartoon.frag" );
    _shaderProgram->setUniform("u_texture_0", 0);
    _shaderProgram->setUniform("u_shadow_map_0", 1);

    _islandProgram = new lithium::ShaderProgram( "shaders/cartoon.vert", "shaders/island.frag" );
    _islandProgram->setUniform("u_texture_0", 0);
    _islandProgram->setUniform("u_shadow_map_0", 1);

    _waterProgram = new lithium::ShaderProgram( "shaders/cartoon.vert", "shaders/water.frag" );
    _waterProgram->setUniform("u_texture_0", 0);
    _waterProgram->setUniform("u_shadow_map_0", 1);

    _bloomProgram = new lithium::ShaderProgram( "shaders/cartoon.vert", "shaders/cartoonbloom.frag" );
    _bloomProgram->setUniform("u_texture_0", 0);

    _skinningShader = new lithium::ShaderProgram("shaders/skinning.vert", "shaders/skinning.frag");
    _skinningShader->use();
    _skinningShader->setUniform("u_texture_0", 0);
    _skinningShader->setUniform("u_shadow_map_0", 1);

    _borderShader = new lithium::ShaderProgram("shaders/screen.vert", "shaders/borders.frag");
    _borderShader->setUniform("diffuseTexture", 0);
    _borderShader->setUniform("depthTexture", 1);
    _borderShader->setUniform("normalTexture", 2);
    _borderShader->setUniform("brightTexture", 3);
    _borderShader->setUniform("waterMaskTexture", 4);
    _borderShader->setUniform("terrainDepthTexture", 5);

    _blurShader = new lithium::ShaderProgram("shaders/screen.vert", "shaders/blur.frag");
    _blurShader->setUniform("u_horizontal", 0);
    _blurShader->setUniform("diffuseTexture", 0);

    _normalShader = new lithium::ShaderProgram( "shaders/cartoon.vert", "shaders/normal.frag" );
    _normalSkinningShader = new lithium::ShaderProgram( "shaders/skinning.vert", "shaders/normal.frag" );

    _instShader = new lithium::ShaderProgram("shaders/cartooninst.vert", "shaders/cartoon.frag");
    _instShader->setUniform("u_texture_0", 0);
    _instShader->setUniform("u_shadow_map_0", 1);

    _borderDepthFBO = new lithium::FrameBuffer(resolution, lithium::FrameBuffer::Mode::DEFAULT);
    _borderDepthFBO->bind();
        _borderDepthFBO->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _borderDepthFBO->createTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
        //glDrawBuffer(GL_NONE);
        //glReadBuffer(GL_NONE);
        _borderDepthFBO->bindTexture(GL_DEPTH_ATTACHMENT);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(GL_TEXTURE_2D, 0);
        /*_borderDepthFBO->declareBuffers();
        _borderDepthFBO->createRenderBuffer(lithium::RenderBuffer::Mode::DEFAULT, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);*/
    _borderDepthFBO->unbind();

    _frameBuffer = new lithium::FrameBuffer(resolution, lithium::FrameBuffer::Mode::MULTISAMPLED);
    _frameBuffer->bind();
        _frameBuffer->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _frameBuffer->createTexture(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _frameBuffer->declareBuffers();
        _frameBuffer->createRenderBuffer(lithium::RenderBuffer::Mode::MULTISAMPLED, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    _frameBuffer->unbind();

    _waterDetectFBO = new lithium::FrameBuffer(resolution, lithium::FrameBuffer::Mode::DEFAULT);
    _waterDetectFBO->bind();
        _waterDetectFBO->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _waterDetectFBO->createTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
        //_waterDetectFBO->createRenderBuffer(lithium::RenderBuffer::Mode::DEFAULT, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    _waterDetectFBO->unbind();

    _horizontalBlurFBO = new lithium::FrameBuffer(resolution, lithium::FrameBuffer::Mode::DEFAULT);
    _horizontalBlurFBO->bind();
        _horizontalBlurFBO->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        //_horizontalBlurFBO->createRenderBuffer(lithium::RenderBuffer::Mode::MULTISAMPLED, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    _horizontalBlurFBO->unbind();

    _verticalBlurFBO = new lithium::FrameBuffer(resolution, lithium::FrameBuffer::Mode::DEFAULT);
    _verticalBlurFBO->bind();
        _verticalBlurFBO->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        //_verticalBlurFBO->createRenderBuffer(lithium::RenderBuffer::Mode::MULTISAMPLED, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    _verticalBlurFBO->unbind();

    _bloomFBO = new lithium::FrameBuffer(resolution, lithium::FrameBuffer::Mode::DEFAULT);
    _bloomFBO->bind();
        _bloomFBO->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    _bloomFBO->unbind();

    _intermediateFBO = new lithium::FrameBuffer(resolution, lithium::FrameBuffer::Mode::DEFAULT);
    _intermediateFBO->bind();
        _intermediateFBO->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _intermediateFBO->createTexture(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _intermediateFBO->declareBuffers();
    _intermediateFBO->unbind();

    _sceneUBO = new lithium::UniformBufferObject(244, "SceneBlock", 0);
    _sceneUBO->bindBufferBase({
        _shaderProgram,
        _islandProgram,
        _skinningShader,
        _normalShader,
        _normalSkinningShader,
        _lightShader,
        _instShader,
        _waterProgram,
        _colorProgram
    });


}

CartoonShading::~CartoonShading() noexcept
{

}

void CartoonShading::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, 4096, 4096);
    _depthMapBuffer->bind();
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0); // ?
        _staticObjects->render(_depthShader);
        _skinnedObjects->render(_depthSkinningShader);
    }
    _depthMapBuffer->unbind();

    glViewport(0, 0, _resolution.x, _resolution.y);
    _bloomFBO->bind();
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _staticObjects->render(_bloomProgram);
    }
    _bloomFBO->unbind();

    _waterDetectFBO->bind();
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _colorProgram->setUniform("iTime", _ocean->time());
        _colorProgram->setUniform("u_color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        _staticObjects->forEach([this](lithium::Renderable* renderable){
            auto object = dynamic_cast<lithium::Object*>(renderable);
            if(object->texture() == AssetFactory::getTextures()->dirtDiffuse)
            {
                if(object->modelInvalidated())
                {
                    object->updateModel();
                }
                _colorProgram->setUniform("u_model", object->model());
                object->draw();
            }
        });
        if(_ocean->modelInvalidated())
        {
            _ocean->updateModel();
        }
        _colorProgram->setUniform("u_model", _ocean->model());
        _colorProgram->setUniform("u_color", glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
        //glDepthFunc(GL_LESS);
        _ocean->draw();
        //glDepthFunc(GL_LESS);
    }
    _waterDetectFBO->bind();

    _frameBuffer->bind();
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1);
        _depthMapBuffer->bindTexture(GL_DEPTH_ATTACHMENT);

#ifdef WIREFRAME_MODE
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

        beforeDiffusePass();

        _staticObjects->forEach([this](lithium::Renderable* renderable){
            auto object = dynamic_cast<lithium::Object*>(renderable);
            object->shade(object->texture() == AssetFactory::getTextures()->dirtDiffuse ? _islandProgram : _shaderProgram);
            object->draw();
        });
        /*for(auto object : _skinnedObjects)
        {
            object->shade(_skinningShader);
            object->draw();
        }*/
        _skinnedObjects->render(_skinningShader);
        _waterProgram->setUniform("iTime", _ocean->time());
        _ocean->shade(_waterProgram);
        _ocean->draw();
        
        afterDiffusePass();

#ifdef WIREFRAME_MODE
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

        glDepthMask(GL_FALSE);
        _light->shade(_lightShader);
        _light->draw();
        glDepthMask(GL_TRUE);
        _frameBuffer->blit(_intermediateFBO,
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);
        _frameBuffer->blit(_intermediateFBO,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST);
    }
    _frameBuffer->unbind();

    _blurShader->use();
    _blurShader->setResolution(glm::vec2(_resolution.x, _resolution.y));
    bool firstIteration{true};
    for(int i{0}; i < 5; ++i)
    {
        _blurShader->setUniform("u_horizontal", 0);
        _verticalBlurFBO->bind();
        {
            //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            _screenMesh->bind();
            glActiveTexture(GL_TEXTURE0);
            if(firstIteration)
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                _intermediateFBO->bindTexture(GL_COLOR_ATTACHMENT1);
                //firstIteration = false;
            }
            else
            {
                _horizontalBlurFBO->bindTexture(GL_COLOR_ATTACHMENT0);
            }
            _screenMesh->draw();
        }
        _verticalBlurFBO->unbind();
        _blurShader->setUniform("u_horizontal", 1);
        _horizontalBlurFBO->bind();
        {
            //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            if(firstIteration)
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                firstIteration = false;
            }
            _screenMesh->bind();
            glActiveTexture(GL_TEXTURE0);
            _verticalBlurFBO->bindTexture(GL_COLOR_ATTACHMENT0);
            _screenMesh->draw();
        }
        _horizontalBlurFBO->unbind();
    }

    //static lithium::ImageTexture* borderMap = lithium::ImageTexture::load("assets/swagbot/BorderMap.png", GL_SRGB, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE2);


    glm::mat4 projView = camera()->projection() * camera()->view();
    _depthShader->setUniform("u_view", projView);
    _depthSkinningShader->setUniform("u_view", projView);

    _borderDepthFBO->bind();
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        beforeBorderPass();

        /*for(auto object : _objects)
        {
            object->shade(_normalShader);
            object->draw();
        }
        for(auto skinnedObject : _skinnedObjects)
        {
            skinnedObject->shade(_normalSkinningShader);
            skinnedObject->draw();
        }*/
        _staticObjects->render(_normalShader);
        _skinnedObjects->render(_normalSkinningShader);

        afterBorderPass();
    }
    _borderDepthFBO->unbind();

    /*
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _debugDepthShader->use();
    _screenMesh->bindVertexArray();
    glActiveTexture(GL_TEXTURE0);
    _borderDepthFBO->bindTexture(GL_DEPTH_ATTACHMENT);
    _screenMesh->drawElements();
    */

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _borderShader->use();
    _borderShader->setResolution(glm::vec2(_resolution.x, _resolution.y));
    _screenMesh->bind();
    glActiveTexture(GL_TEXTURE0);
    _frameBuffer->bindTexture(GL_COLOR_ATTACHMENT0);
    glActiveTexture(GL_TEXTURE1);
    _borderDepthFBO->bindTexture(GL_DEPTH_ATTACHMENT);
    glActiveTexture(GL_TEXTURE2);
    _borderDepthFBO->bindTexture(GL_COLOR_ATTACHMENT0);
    glActiveTexture(GL_TEXTURE3);
    _horizontalBlurFBO->bindTexture(GL_COLOR_ATTACHMENT0);
    glActiveTexture(GL_TEXTURE4);
    _waterDetectFBO->bindTexture(GL_COLOR_ATTACHMENT0);
    glActiveTexture(GL_TEXTURE5);
    _waterDetectFBO->bindTexture(GL_DEPTH_ATTACHMENT);
    _screenMesh->draw();
    glBindVertexArray(0);
}