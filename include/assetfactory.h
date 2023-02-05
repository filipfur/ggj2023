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
        lithium::Mesh* tiles[goptions::numUniqueTiles];
        lithium::Mesh* grass[goptions::numUniqueTiles];
        lithium::Mesh* trees[goptions::numUniqueTiles];
        lithium::Mesh* chest;
    } _meshes;

    struct Textures
    {
        lithium::ImageTexture* permanentMarkerSdf;
        lithium::ImageTexture* blockDiffuse;
        lithium::ImageTexture* potatoDiffuse;
        lithium::ImageTexture* oceanDiffuse;
        lithium::ImageTexture* dirtDiffuse;
        lithium::ImageTexture* grassDiffuse;
        lithium::ImageTexture* houseDiffuse;
        lithium::ImageTexture* treeDiffuse;
        lithium::ImageTexture* chestDiffuse;
    } _textures;

    struct Objects
    {
        lithium::SkinnedObject* potato;
        lithium::Object* block;
        lithium::Object* ocean;
        lithium::Object* tiles[goptions::numUniqueTiles];
        lithium::Object* grass[goptions::numUniqueTiles];
        lithium::Object* trees[goptions::numUniqueTiles];
        lithium::Object* chest;
    } _objects;

    struct Fonts
    {
        lithium::Font* permanentMarker;
    } _fonts;

    MapSegment* _mapSegments[goptions::numUniqueTiles];

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
    static MapSegment** getMapSegments();

    gltf::Loader _gltfLoader;
};