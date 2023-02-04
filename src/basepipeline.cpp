#include "basepipeline.h"

#include "assetfactory.h"
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

BasePipeline::BasePipeline(const glm::ivec2& resolution) : lithium::RenderPipeline{resolution}
{
    _camera = new lithium::SimpleCamera(glm::perspective(SIMPLE_CAMERA_FOV, (float)resolution.x / (float)resolution.y, SIMPLE_CAMERA_NEAR, SIMPLE_CAMERA_FAR));
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

    _depthMapBuffer = new lithium::FrameBuffer(glm::ivec2{4096});
    _depthMapBuffer->bind();
        _depthMapBuffer->createTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_BORDER);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        _depthMapBuffer->bindTexture(GL_DEPTH_ATTACHMENT);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindTexture(GL_TEXTURE_2D, 0);
    _depthMapBuffer->unbind();

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
}

void BasePipeline::update(float dt)
{
    lithium::RenderPipeline::update(dt);
    _camera->update(dt);
    _sceneUBO->bufferSubData(0, _camera->view());
    _sceneUBO->bufferSubData(64, _camera->projection());
    _sceneUBO->bufferSubData(128, _lightSpaceMatrix);
    _sceneUBO->bufferSubData(192, _camera->position());
    _sceneUBO->bufferSubData(208, _light->position());
    _sceneUBO->bufferSubData(224, _light->rgb());
    float t = time();
    _sceneUBO->bufferSubData(240, t);

    static float near_plane = 1.0f, far_plane = 100.0f;
    static glm::mat4 lightProjection{glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane)};
    glm::mat4 lightView{glm::lookAt(_light->position(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0))};        
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    _lightSpaceMatrix = lightProjection * lightView;
    _depthShader->setUniform("u_view", _lightSpaceMatrix);
    _depthSkinningShader->setUniform("u_view", _lightSpaceMatrix);
}

void BasePipeline::render()
{

}

void BasePipeline::renderText(float x, float y, const std::string& str)
{
    _text->setPosition(x, y, 0.0f);
    _text->setText(str);
    _text->update(0);
    _text->shade(_sdfTextShader);
    _text->draw();
}