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
        std::shared_ptr<lithium::Mesh> screen;
        std::shared_ptr<lithium::Mesh> block;
        std::shared_ptr<lithium::Mesh> potato;
        std::shared_ptr<lithium::Mesh> ocean;
        std::shared_ptr<lithium::Mesh> tiles[goptions::numUniqueTiles];
        std::shared_ptr<lithium::Mesh> grass[goptions::numUniqueTiles];
        std::shared_ptr<lithium::Mesh> trees[goptions::numUniqueTiles];
        std::shared_ptr<lithium::Mesh> chest;
    } _meshes;

    struct Textures
    {
        std::shared_ptr<lithium::ImageTexture> permanentMarkerSdf;
        std::shared_ptr<lithium::ImageTexture> blockDiffuse;
        std::shared_ptr<lithium::ImageTexture> potatoDiffuse;
        std::shared_ptr<lithium::ImageTexture> oceanDiffuse;
        std::shared_ptr<lithium::ImageTexture> dirtDiffuse;
        std::shared_ptr<lithium::ImageTexture> grassDiffuse;
        std::shared_ptr<lithium::ImageTexture> houseDiffuse;
        std::shared_ptr<lithium::ImageTexture> treeDiffuse;
        std::shared_ptr<lithium::ImageTexture> chestDiffuse;
    } _textures;

    struct Objects
    {
        std::shared_ptr<lithium::SkinnedObject> potato;
        std::shared_ptr<lithium::Object> block;
        std::shared_ptr<lithium::Object> ocean;
        std::shared_ptr<lithium::Object> tiles[goptions::numUniqueTiles];
        std::shared_ptr<lithium::Object> grass[goptions::numUniqueTiles];
        std::shared_ptr<lithium::Object> trees[goptions::numUniqueTiles];
        std::shared_ptr<lithium::Object> chest;
    } _objects;

    struct Fonts
    {
        std::shared_ptr<lithium::Font> permanentMarker;
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