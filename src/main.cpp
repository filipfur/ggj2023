#include "glapplication.h"
#include "glrenderpipeline.h"
#include "gltinyobjloader.h"
#include "glsimplecamera.h"

using attr = lithium::VertexArrayBuffer::AttributeType;
static constexpr attr POSITION{attr::VEC3};
static constexpr attr NORMAL{attr::VEC3};
static constexpr attr UV{attr::VEC2};

class BasicPipeline : public lithium::RenderPipeline
{
public:
    BasicPipeline(const glm::ivec2& resolution) : lithium::RenderPipeline{resolution},
        _camera{new lithium::SimpleCamera(glm::perspective(glm::radians(45.0f), (float)resolution.x / (float)resolution.y, 0.1f, 100.0f))}
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(3.0f);
        _screenMesh = new lithium::Mesh({attr::VEC3, attr::VEC3, attr::VEC2}, {
            -1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
            -1.0,  1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 1.0,
            1.0,  1.0, 0.0f,	0.0f, 1.0f, 0.0f,	1.0, 1.0,
            1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	1.0, 0.0f,
        },
        {
            0, 2, 1,
            0, 3, 2
        });
        _mesh = lithium::tinyobjloader_load("assets/block.obj", {POSITION, NORMAL, UV});
        _blockShader = new lithium::ShaderProgram("assets/blockshader.vert", "assets/blockshader.frag");
        _blockShader->setUniform("u_texture_0", 0);
        _blockShader->setUniform("u_projection", _camera->projection());
        _borderShader = new lithium::ShaderProgram("assets/blockshader.vert", "assets/bordershader.frag");
        _borderShader->setUniform("u_texture_0", 0);
        _borderShader->setUniform("u_projection", _camera->projection());
        _screenShader = new lithium::ShaderProgram("assets/screenshader.vert", "assets/screenshader.frag");
        _diffuseTexture = lithium::ImageTexture::load("assets/Kraxbox_logo_lithium_metal_2ff2069c-b84a-426c-bf92-e9831105a5df.png", GL_SRGB, GL_RGB);
        _object = new lithium::Object(_mesh, _diffuseTexture);
        _object->setPosition(glm::vec3{0.0f});
        insertObject(_object->clone());
        _camera->setPosition(glm::vec3{3.0f, 3.0f, 3.0f});
        _camera->setTarget(glm::vec3{0.0f});
    }

    ~BasicPipeline()
    {
        delete _diffuseTexture;
        delete _blockShader;
        delete _screenShader;
        delete _screenMesh;
        delete _mesh;
        delete _object;
    }

    virtual void update(float dt) override
    {
        std::for_each(_objects.begin(), _objects.end(), [this, dt](lithium::Object* o) {
            o->update(dt);
            o->setRotation(o->rotation() + glm::vec3{8.0f * dt});
            return true;
        });
        _camera->update(dt);
    }

    virtual void render() override
    {
        setViewportToResolution();
        glClearColor(0.8f, 1.0f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _blockShader->setUniform("u_view", _camera->view());
        _borderShader->setUniform("u_view", _camera->view());
        _screenShader->use();
        _screenMesh->bind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glActiveTexture(GL_TEXTURE0);
        glDepthMask(GL_FALSE);
        _screenMesh->draw();
        glDepthMask(GL_TRUE);
        std::for_each(_objects.begin(), _objects.end(), [this](lithium::Object* o) {
            o->setScale(1.0f);
            o->setColor(glm::vec3{1.0f, 0.85f, 0.55f});
            o->shade(_blockShader);
            o->draw();
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            o->setScale(1.15f);
            o->setColor(glm::vec3{0.3f, 1.0f, 0.0f});
            o->shade(_borderShader);
            o->draw();
            return true;
        });

    }

private:
    lithium::ShaderProgram* _blockShader{nullptr};
    lithium::ShaderProgram* _borderShader{nullptr};
    lithium::ShaderProgram* _screenShader{nullptr};
    lithium::SimpleCamera* _camera{nullptr};
    lithium::ImageTexture* _diffuseTexture;
    lithium::Object* _object;
    lithium::Mesh* _mesh;
    lithium::Mesh* _screenMesh;
};

class BasicApp : public lithium::Application
{
public:
    BasicApp() : Application{"A simple application", glm::ivec2{600}, lithium::Application::Mode::DEFAULT, false}
    {
        _renderPipeline = new BasicPipeline(defaultFrameBufferResolution());
    }

    virtual void update(float dt) override
    {
        _renderPipeline->update(dt);
        _renderPipeline->render();
    }
private:
    BasicPipeline* _renderPipeline{nullptr};
};

int main(int argc, const char* argv[])
{
    BasicApp* basicApp = new BasicApp();
    basicApp->run();
    return 0;
}