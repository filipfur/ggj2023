#include "assetfactory.h"

AssetFactory::AssetFactory()
{

}

AssetFactory::~AssetFactory() noexcept
{

}

using attr = lithium::VertexArrayBuffer::AttributeType;
static constexpr attr POSITION{attr::VEC3};
static constexpr attr NORMAL{attr::VEC3};
static constexpr attr UV{attr::VEC2};
static constexpr attr BONE_IDS{attr::VEC4};
static constexpr attr BONE_WEIGHTS{attr::VEC4};

const std::vector<attr> AssetFactory::objectAttributes{POSITION, NORMAL, UV};
const std::vector<attr> AssetFactory::modelAttributes{POSITION, NORMAL, UV, BONE_IDS, BONE_WEIGHTS};

void AssetFactory::loadMeshes()
{
    AssetFactory& instance = getInstance();
    instance._meshes.screen = new lithium::Mesh({POSITION, NORMAL, UV, attr::VEC3},{
        -1.0f, -1.0f, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 1.0,     0.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,	    0.0f, 1.0f, 0.0f,	1.0, 1.0,      1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 	    0.0f, 1.0f, 0.0f,	1.0, 0.0f,     0.0f, 1.0f, 1.0f 
    },
    {
        0, 2, 1,
        0, 3, 2
    });
    instance._meshes.block = lithium::tinyobjloader_load("assets/block.obj", AssetFactory::objectAttributes);
    instance._meshes.tile1 = lithium::tinyobjloader_load("assets/tile1/tiles1.obj", AssetFactory::objectAttributes);
    instance._meshes.potato = lithium::tinyobjloader_load("assets/player/PlayerStatic.obj", AssetFactory::objectAttributes);
}

void AssetFactory::loadTextures()
{
    AssetFactory& instance = getInstance();
    instance._textures.blockDiffuse = lithium::ImageTexture::load("assets/Kraxbox_logo_lithium_metal_2ff2069c-b84a-426c-bf92-e9831105a5df.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.treeDiffuse = lithium::ImageTexture::load("assets/tree.jpg", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.potatoDiffuse = lithium::ImageTexture::load("assets/player/Potato.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.dirtDiffuse = lithium::ImageTexture::load("assets/tile1/Dirt.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.permanentMarkerSdf = lithium::ImageTexture::load("assets/PermanentMarker32.png", GL_RGB, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE0, 1, false);
}

void AssetFactory::loadObjects()
{
    AssetFactory& instance = getInstance();
    instance._objects.block = new lithium::Object(instance._meshes.block, instance._textures.blockDiffuse);
    instance._objects.potato = new lithium::Object(instance._meshes.potato, instance._textures.potatoDiffuse);
    instance._objects.tile1 = new lithium::Object(instance._meshes.tile1, instance._textures.dirtDiffuse);
}

void AssetFactory::loadFonts()
{
    AssetFactory& instance = getInstance();
    instance._fonts.permanentMarker = new lithium::Font(instance._textures.permanentMarkerSdf, "assets/PermanentMarker32.json");
}

const AssetFactory::Meshes* AssetFactory::getMeshes()
{
    return &AssetFactory::getInstance()._meshes;
}

const AssetFactory::Textures* AssetFactory::getTextures()
{
    return &AssetFactory::getInstance()._textures;
}

const AssetFactory::Objects* AssetFactory::getObjects()
{
    return &AssetFactory::getInstance()._objects;
}

const AssetFactory::Fonts* AssetFactory::getFonts()
{
    return &AssetFactory::getInstance()._fonts;
}

AssetFactory& AssetFactory::getInstance()
{
    static AssetFactory instance;
    return instance;
}
