#pragma once

#include "glfont.h"
#include "globject.h"
#include "glanimatedobject.h"
#include "gltinyobjloader.h"
#include "gltfloader.h"
#include "glskinnedobject.h"
#include "mapsegment.h"
#include "options.h"

class AssetFactory
{
public:
    struct Meshes
    {
        lithium::Mesh* screen;
        lithium::Mesh* block;
        lithium::Mesh* potato;
        lithium::Mesh* ocean;
        lithium::Mesh* tiles[numUniqueTiles];
        lithium::Mesh* grass[numUniqueTiles];
        lithium::Mesh* trees[numUniqueTiles];
    } _meshes;

    struct Textures
    {
        lithium::ImageTexture* permanentMarkerSdf;
        lithium::ImageTexture* blockDiffuse;
        lithium::ImageTexture* potatoDiffuse;
        lithium::ImageTexture* oceanDiffuse;
        lithium::ImageTexture* dirtDiffuse;
        lithium::ImageTexture* grassDiffuse;
        lithium::ImageTexture* treeDiffuse;
    } _textures;

    struct Objects
    {
        lithium::SkinnedObject* potato;
        lithium::Object* block;
        lithium::Object* ocean;
        lithium::Object* tiles[numUniqueTiles];
        lithium::Object* grass[numUniqueTiles];
        lithium::Object* trees[numUniqueTiles];
    } _objects;

    struct Fonts
    {
        lithium::Font* permanentMarker;
    } _fonts;

    MapSegment* mapSegments[numUniqueTiles];

    AssetFactory();
    virtual ~AssetFactory() noexcept;
    AssetFactory(AssetFactory const&) = delete;
    void operator=(AssetFactory const&)  = delete;

    static void loadMeshes();
    static void loadTextures();
    static void loadObjects();
    static void loadFonts();

    static AssetFactory& getInstance();

    static const std::vector<lithium::VertexArrayBuffer::AttributeType> objectAttributes;
    static const std::vector<lithium::VertexArrayBuffer::AttributeType> modelAttributes;

    static const AssetFactory::Meshes* getMeshes();
    static const AssetFactory::Textures* getTextures();
    static const AssetFactory::Objects* getObjects();
    static const AssetFactory::Fonts* getFonts();

    gltf::Loader _gltfLoader;
};