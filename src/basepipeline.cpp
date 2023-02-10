#include "basepipeline.h"

#include "assetfactory.h"
#include "terrainobject.h"
//#include <glm/gtc/matrix_transform.hpp>

#define SIMPLE_CAMERA_NEAR 0.1f
#define SIMPLE_CAMERA_FAR 300.0f
#define SIMPLE_CAMERA_FOV glm::radians(45.0f)

using attr = lithium::VertexArrayBuffer::AttributeType;
static constexpr attr POSITION{attr::VEC3};
static constexpr attr NORMAL{attr::VEC3};
static constexpr attr UV{attr::VEC2};
static constexpr attr BONE_IDS{attr::VEC4};
static constexpr attr BONE_WEIGHTS{attr::VEC4};

BasePipeline::BasePipeline(const glm::ivec2& resolution)
    : lithium::RenderPipeline{resolution},
    _shadowMapBuffer{std::make_shared<lithium::FrameBuffer>(glm::ivec2{4096})},
    _frameBuffer{std::make_shared<lithium::FrameBuffer>(resolution, lithium::FrameBuffer::Mode::MULTISAMPLED)},
    _bloomFBO{std::make_shared<lithium::FrameBuffer>(resolution, lithium::FrameBuffer::Mode::DEFAULT)},
    _waterDetectFBO{std::make_shared<lithium::FrameBuffer>(resolution, lithium::FrameBuffer::Mode::DEFAULT)}
{
    _camera = new lithium::SimpleCamera(glm::perspective(SIMPLE_CAMERA_FOV, (float)resolution.x / (float)resolution.y, SIMPLE_CAMERA_NEAR, SIMPLE_CAMERA_FAR));
    
    _objects = createRenderGroup([](lithium::Renderable* renderable) {
        return dynamic_cast<lithium::Object*>(renderable);
    });

    _terrainObjects = createRenderGroup([](lithium::Renderable* renderable) {
        return dynamic_cast<TerrainObject*>(renderable);
    });

    _staticObjects = createRenderGroup([](lithium::Renderable* renderable) {
        return dynamic_cast<lithium::Object*>(renderable)
            && dynamic_cast<TerrainObject*>(renderable) == nullptr
            && dynamic_cast<lithium::SkinnedObject*>(renderable) == nullptr;
    });

    _skinnedObjects = createRenderGroup([](lithium::Renderable* renderable) {
        return dynamic_cast<lithium::SkinnedObject*>(renderable);
    });
    
    std::vector<attr> attributes{POSITION, NORMAL, UV};
    _screenMesh = new lithium::Mesh(attributes, {
        -1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
        -1.0,  1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 1.0,
        1.0,  1.0, 0.0f,	0.0f, 1.0f, 0.0f,	1.0, 1.0,
        1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	1.0, 0.0f,
    },
    {
        0, 2, 1,
        0, 3, 2
    });
    _lightShader = new lithium::ShaderProgram("shaders/light.vert", "shaders/light.frag");
    _depthShader = new lithium::ShaderProgram( "shaders/shadowdepth.vert", "shaders/shadowdepth.frag" );
    _depthSkinningShader = new lithium::ShaderProgram( "shaders/shadowdepthskinning.vert", "shaders/shadowdepth.frag" );
    _debugDepthShader = new lithium::ShaderProgram( "shaders/screen.vert", "shaders/debugdepth.frag" );
    _debugDepthShader->setUniform("depthMap", 0);
    _debugDepthShader->setUniform("near_plane", SIMPLE_CAMERA_NEAR);
    _debugDepthShader->setUniform("far_plane", SIMPLE_CAMERA_FAR);

    _shadowMapBuffer->bind();
    {
        _shadowMapBuffer->createTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        _shadowMapBuffer->bindTexture(GL_DEPTH_ATTACHMENT);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    _shadowMapBuffer->unbind();

    glViewport(0, 0, resolution.x, resolution.y);

    _orthoCamera = new lithium::OrthographicCamera(0, resolution.x, 0, resolution.y, -10000.0f, 10000.0f);
    _sdfTextShader = new lithium::ShaderProgram("shaders/sdfTextOrtho.vert", "shaders/sdfText.frag");
    _orthoCamera->attachShader(_sdfTextShader);
    _orthoCamera->update(0);

    _camera->setPosition(glm::vec3{10.0f, 30.0f, 10.0f});
    _camera->setTarget(glm::vec3{0.0f});


    _text = new lithium::Text(AssetFactory::getFonts()->permanentMarker, "Hello ggj2023!", 1.6f);
    //_text->setPosition(800.0f, 400.0f, 0.0f);
    _text->setAlignment(lithium::Text::Alignment::LEFT);
    _text->setColor(glm::vec3{1.0f, 0.0f, 1.0f});

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
    {
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
    }
    _borderDepthFBO->unbind();

    _frameBuffer->bind();
        _frameBuffer->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _frameBuffer->createTexture(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        _frameBuffer->declareBuffers();
        _frameBuffer->createRenderBuffer(lithium::RenderBuffer::Mode::MULTISAMPLED, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    _frameBuffer->unbind();

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

    _shadowMapStage = createRenderStage(_shadowMapBuffer,
        glm::ivec4{0, 0, 4096, 4096},
        [this](){
        glClear(GL_DEPTH_BUFFER_BIT);
        _staticObjects->render(_depthShader);
        _skinnedObjects->render(_depthSkinningShader);
    });

    glm::ivec4 viewport{0, 0, _resolution.x, _resolution.y};

    _bloomStage = createRenderStage(_bloomFBO, viewport, [this](){
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _staticObjects->render(_bloomProgram);
    });

    _waterDetectStage = createRenderStage(_waterDetectFBO, viewport, [this](){
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _colorProgram->setUniform("iTime", _ocean->time());
        _colorProgram->setUniform("u_color", glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        _terrainObjects->forEach([this](lithium::Renderable* renderable){
            auto object = dynamic_cast<lithium::Object*>(renderable);
            if(object->modelInvalidated())
            {
                object->updateModel();
            }
            _colorProgram->setUniform("u_model", object->model());
            //glCullFace(GL_FRONT);
            object->draw();
        });
        glCullFace(GL_BACK);
        if(_ocean->modelInvalidated())
        {
            _ocean->updateModel();
        }
        _colorProgram->setUniform("u_model", _ocean->model());
        _colorProgram->setUniform("u_color", glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
        //glDepthFunc(GL_LESS);
        _ocean->draw();
        //glDepthFunc(GL_LESS);
    });

    _mainStage = createRenderStage(_frameBuffer, viewport, [this](){
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1);
        _shadowMapBuffer->bindTexture(GL_DEPTH_ATTACHMENT);

#ifdef WIREFRAME_MODE
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

        _terrainObjects->render(_islandProgram);
        _staticObjects->render(_shaderProgram);
        _skinnedObjects->render(_skinningShader);
        _waterProgram->setUniform("iTime", _ocean->time());
        _ocean->shade(_waterProgram);
        _ocean->draw();

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
    });
}

void BasePipeline::render()
{
    _sceneUBO->bufferSubData(0, _camera->view());
    _sceneUBO->bufferSubData(64, _camera->projection());
    _sceneUBO->bufferSubData(128, _lightSpaceMatrix);
    _sceneUBO->bufferSubData(192, _camera->position());
    _sceneUBO->bufferSubData(208, _light->position());
    _sceneUBO->bufferSubData(224, _light->rgb());
    //_sceneUBO->bufferSubData(240, t);
    _depthShader->setUniform("u_view", _lightSpaceMatrix);
    _depthSkinningShader->setUniform("u_view", _lightSpaceMatrix);
    

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shadowMapStage->render();

    glm::ivec4 viewport{0, 0, _resolution.x, _resolution.y};
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    
    _bloomStage->render();

    _waterDetectStage->render();

    _mainStage->render();

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
        _terrainObjects->render(_normalShader);
        _staticObjects->render(_normalShader);
        _skinnedObjects->render(_normalSkinningShader);
    }
    _borderDepthFBO->unbind();

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

void BasePipeline::renderText(float x, float y, const std::string& str)
{
    _text->setPosition(x, y, 0.0f);
    _text->setText(str);
    _text->update(0);
    _text->shade(_sdfTextShader);
    _text->draw();
}