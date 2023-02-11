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

static const std::vector<attr> screenMeshAttributes = { POSITION, NORMAL, UV };

static const std::vector<GLfloat> screenMeshVertices = {
    -1.0f, -1.0f, +0.0f, 	+0.0f, +1.0f, +0.0f,	+0.0f, +0.0f,
    -1.0f, +1.0f, +0.0f, 	+0.0f, +1.0f, +0.0f,	+0.0f, +1.0f,
    +1.0f, +1.0f, +0.0f,	+0.0f, +1.0f, +0.0f,	+1.0f, +1.0f,
    +1.0f, -1.0f, +0.0f, 	+0.0f, +1.0f, +0.0f,	+1.0f, +0.0f,
};

static const std::vector<GLuint> screenMeshIndices = {
    0, 2, 1,
    0, 3, 2
};

void AssetFactory::loadMeshes()
{
    AssetFactory& instance = getInstance();
    instance._meshes.screen = std::make_shared<lithium::Mesh>(screenMeshAttributes, screenMeshVertices, screenMeshIndices);
    instance._meshes.block = lithium::tinyobjloader_load("assets/block.obj", AssetFactory::objectAttributes);
    instance._meshes.potato = lithium::tinyobjloader_load("assets/player/PlayerStatic.obj", AssetFactory::objectAttributes);
    instance._meshes.ocean = lithium::tinyobjloader_load("assets/ocean/Ocean.obj", AssetFactory::objectAttributes);
    for (int idx = 0; idx < goptions::numUniqueTiles; idx++) {
        std::string numString = std::to_string(idx + 1);
        std::vector<glm::vec3> vertexPositions;
        instance._meshes.tiles[idx] = lithium::tinyobjloader_load(("assets/tile" + numString + "/tile" + numString + ".obj").c_str(), AssetFactory::objectAttributes, glm::vec2{1.0f}, &vertexPositions);
        instance._mapSegments[idx] = new MapSegment(vertexPositions);
        if(idx == 3)
        {
            instance._meshes.grass[idx] = lithium::tinyobjloader_load(("assets/tile" + numString + "/houseTile" + numString + ".obj").c_str(), AssetFactory::objectAttributes);
        }
        else
        {
            instance._meshes.grass[idx] = lithium::tinyobjloader_load(("assets/tile" + numString + "/grassTile" + numString + ".obj").c_str(), AssetFactory::objectAttributes);
        }
        instance._meshes.trees[idx] = lithium::tinyobjloader_load(("assets/tile" + numString + "/treeTile" + numString + ".obj").c_str(), AssetFactory::objectAttributes);
    }
    instance._meshes.chest = lithium::tinyobjloader_load("assets/chest/chest.obj", AssetFactory::objectAttributes);
}

void AssetFactory::loadTextures()
{
    AssetFactory& instance = getInstance();
    instance._textures.blockDiffuse.reset(lithium::ImageTexture::load("assets/Kraxbox_logo_lithium_metal_2ff2069c-b84a-426c-bf92-e9831105a5df.png", GL_SRGB, GL_RGB));
    instance._textures.potatoDiffuse.reset(lithium::ImageTexture::load("assets/Potato.png", GL_SRGB, GL_RGB));
    instance._textures.oceanDiffuse.reset(lithium::ImageTexture::load("assets/Ocean/ocean.png", GL_SRGB, GL_RGB));
    instance._textures.oceanDiffuse->setWrap(GL_REPEAT);
    instance._textures.dirtDiffuse.reset(lithium::ImageTexture::load("assets/Dirt.png", GL_SRGB, GL_RGB));
    instance._textures.grassDiffuse.reset(lithium::ImageTexture::load("assets/Grass.png", GL_SRGB, GL_RGB));
    instance._textures.houseDiffuse.reset(lithium::ImageTexture::load("assets/House.png", GL_SRGB, GL_RGB));
    instance._textures.treeDiffuse.reset(lithium::ImageTexture::load("assets/Tree.png", GL_SRGB, GL_RGB));
    instance._textures.chestDiffuse.reset(lithium::ImageTexture::load("assets/chest/chest-diffuse.png", GL_SRGB, GL_RGB));
    instance._textures.permanentMarkerSdf.reset(lithium::ImageTexture::load("assets/PermanentMarker32.png", GL_RGB, GL_RGBA, 1, false));
}

void AssetFactory::loadObjects()
{
    AssetFactory& instance = getInstance();
    instance._objects.block.reset(new lithium::Object(instance._meshes.block, {instance._textures.blockDiffuse}));
    instance._objects.potato = instance._gltfLoader.loadSkinnedObject("assets/player/Player.gltf"); //std::make_shared<lithium::Object>(instance._meshes.potato, instance._textures.potatoDiffuse);
    instance._objects.ocean.reset(new lithium::Object(instance._meshes.ocean, {instance._textures.oceanDiffuse}));
    for (int idx = 0; idx < goptions::numUniqueTiles; idx++) {
        instance._objects.tiles[idx].reset(new lithium::Object(instance._meshes.tiles[idx], {instance._textures.dirtDiffuse}));
        if(idx == 3)
        {
            instance._objects.grass[idx].reset(new lithium::Object(instance._meshes.grass[idx], {instance._textures.houseDiffuse}));
        }
        else
        {
            instance._objects.grass[idx].reset(new lithium::Object(instance._meshes.grass[idx], {instance._textures.grassDiffuse}));
        }
        instance._objects.trees[idx].reset(new lithium::Object(instance._meshes.trees[idx], {instance._textures.treeDiffuse}));
    }
    instance._objects.chest.reset(new lithium::Object(instance._meshes.chest, {instance._textures.chestDiffuse}));
}

void AssetFactory::loadFonts()
{
    AssetFactory& instance = getInstance();
    instance._fonts.permanentMarker = std::make_shared<lithium::Font>(instance._textures.permanentMarkerSdf, "assets/PermanentMarker32.json");
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

MapSegment** AssetFactory::getMapSegments() {
    return AssetFactory::getInstance()._mapSegments;
}

AssetFactory& AssetFactory::getInstance()
{
    static AssetFactory instance;
    return instance;
}
