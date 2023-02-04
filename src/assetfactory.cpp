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
    instance._meshes.potato = lithium::tinyobjloader_load("assets/player/PlayerStatic.obj", AssetFactory::objectAttributes);
    instance._meshes.ocean = lithium::tinyobjloader_load("assets/ocean/Ocean.obj", AssetFactory::objectAttributes);
    for (int idx = 0; idx < numUniqueTiles; idx++) {
        std::string numString = std::to_string(idx + 1);
        std::vector<glm::vec3> vertexPositions;
        instance._meshes.tiles[idx] = lithium::tinyobjloader_load(("assets/tile" + numString + "/tile" + numString + ".obj").c_str(), AssetFactory::objectAttributes, glm::vec2{1.0f}, &vertexPositions);
        instance.mapSegments[idx] = new MapSegment(vertexPositions);
        instance._meshes.grass[idx] = lithium::tinyobjloader_load(("assets/tile" + numString + "/grassTile" + numString + ".obj").c_str(), AssetFactory::objectAttributes);
        instance._meshes.trees[idx] = lithium::tinyobjloader_load(("assets/tile" + numString + "/treeTile" + numString + ".obj").c_str(), AssetFactory::objectAttributes);
    }
}

void AssetFactory::loadTextures()
{
    AssetFactory& instance = getInstance();
    instance._textures.blockDiffuse = lithium::ImageTexture::load("assets/Kraxbox_logo_lithium_metal_2ff2069c-b84a-426c-bf92-e9831105a5df.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.potatoDiffuse = lithium::ImageTexture::load("assets/Potato.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.oceanDiffuse = lithium::ImageTexture::load("assets/Ocean/ocean.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.dirtDiffuse = lithium::ImageTexture::load("assets/Dirt.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.grassDiffuse = lithium::ImageTexture::load("assets/Grass.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.treeDiffuse = lithium::ImageTexture::load("assets/Tree.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT);
    instance._textures.permanentMarkerSdf = lithium::ImageTexture::load("assets/PermanentMarker32.png", GL_RGB, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_TEXTURE0, 1, false);
}

void AssetFactory::loadObjects()
{
    AssetFactory& instance = getInstance();
    instance._objects.block = new lithium::Object(instance._meshes.block, instance._textures.blockDiffuse);
    instance._objects.potato =  instance._gltfLoader.loadSkinnedObject("assets/player/Player.gltf"); //new lithium::Object(instance._meshes.potato, instance._textures.potatoDiffuse);
    instance._objects.ocean = new lithium::Object(instance._meshes.ocean, instance._textures.oceanDiffuse);
    for (int idx = 0; idx < numUniqueTiles; idx++) {
        instance._objects.tiles[idx] = new lithium::Object(instance._meshes.tiles[idx], instance._textures.dirtDiffuse);
        instance._objects.grass[idx] = new lithium::Object(instance._meshes.grass[idx], instance._textures.grassDiffuse);
        instance._objects.trees[idx] = new lithium::Object(instance._meshes.trees[idx], instance._textures.treeDiffuse);
    }
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
