#include "pipeline.h"

#include "assetfactory.h"

#define RENDER_TEXT(x, y, stream) { \
    std::stringstream ss{}; \
    ss << stream; \
    renderText(x, y, ss.str()); \
} \

using attr = lithium::VertexArrayBuffer::AttributeType;
static constexpr attr POSITION{attr::VEC3};
static constexpr attr NORMAL{attr::VEC3};
static constexpr attr UV{attr::VEC2};

Pipeline::Pipeline(const glm::ivec2& resolution)  : lithium::RenderPipeline{resolution},
        _camera{new lithium::SimpleCamera(glm::perspective(glm::radians(45.0f), (float)resolution.x / (float)resolution.y, 0.1f, 100.0f))}
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(3.0f);
    _screenMesh = AssetFactory::getMeshes()->screen;
    _blockShader = new lithium::ShaderProgram("shaders/blockshader.vert", "shaders/blockshader.frag");
    _blockShader->setUniform("u_texture_0", 0);
    _blockShader->setUniform("u_projection", _camera->projection());
    _borderShader = new lithium::ShaderProgram("shaders/blockshader.vert", "shaders/bordershader.frag");
    _borderShader->setUniform("u_texture_0", 0);
    _borderShader->setUniform("u_projection", _camera->projection());
    _screenShader = new lithium::ShaderProgram("shaders/screenshader.vert", "shaders/screenshader.frag");
    _object = AssetFactory::getObjects()->tile1->clone();
    _object->setPosition(glm::vec3{0.0f});
    insertObject(_object->clone());
    _camera->setPosition(glm::vec3{3.0f, 3.0f, 3.0f});
    _camera->setTarget(glm::vec3{0.0f});

    _orthoCamera = new lithium::OrthographicCamera(0, resolution.x, 0, resolution.y, -10000.0f, 10000.0f);
    _sdfTextShader = new lithium::ShaderProgram("shaders/sdfTextOrtho.vert", "shaders/sdfText.frag");
    _orthoCamera->attachShader(_sdfTextShader);
    _orthoCamera->update(0);

    _text = new lithium::Text(AssetFactory::getFonts()->permanentMarker, "Hello ggj2023!", 1.6f);
    //_text->setPosition(800.0f, 400.0f, 0.0f);
    _text->setAlignment(lithium::Text::Alignment::LEFT);
    _text->setColor(glm::vec3{1.0f, 0.0f, 1.0f});
}

Pipeline::~Pipeline() noexcept
{
    delete _blockShader;
    delete _screenShader;
    delete _screenMesh;
    delete _object;
}

void Pipeline::update(float dt)
{
    std::for_each(_objects.begin(), _objects.end(), [this, dt](lithium::Object* o) {
        o->update(dt);
        o->setRotation(o->rotation() + glm::vec3{8.0f * dt});
        return true;
    });
    _camera->update(dt);
}

void Pipeline::render()
{
    setViewportToResolution();
    glClearColor(0.8f, 1.0f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _blockShader->setUniform("u_view", _camera->view());
    _borderShader->setUniform("u_view", _camera->view());
    _screenShader->use();
    _screenMesh->bind();
    glActiveTexture(GL_TEXTURE0);
    glDepthMask(GL_FALSE);
    AssetFactory::getTextures()->treeDiffuse->bind();
    _screenMesh->draw();
    glDepthMask(GL_TRUE);
    std::for_each(_objects.begin(), _objects.end(), [this](lithium::Object* o) {
        o->setScale(0.2f);
        o->setColor(glm::vec3{1.0f, 0.85f, 0.55f});
        o->shade(_blockShader);
        o->draw();
        return true;
    });

    //RENDER_TEXT(32.0f, 32.0f, "Hello ggj2023! <3")
}

void Pipeline::renderText(float x, float y, const std::string& str)
{
    _text->setPosition(x, y, 0.0f);
    _text->setText(str);
    _text->update(0);
    _text->shade(_sdfTextShader);
    _text->draw();
}