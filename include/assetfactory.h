#pragma once

#include "glfont.h"
#include "globject.h"
#include "glanimatedobject.h"
#include "gltinyobjloader.h"
#include "gltfloader.h"
#include "glskinnedobject.h"
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
    } _meshes;

    struct Textures
    {
        lithium::ImageTexture* permanentMarkerSdf;
        lithium::ImageTexture* blockDiffuse;
        lithium::ImageTexture* treeDiffuse;
        lithium::ImageTexture* potatoDiffuse;
        lithium::ImageTexture* oceanDiffuse;
        lithium::ImageTexture* dirtDiffuse;
    } _textures;

    struct Objects
    {
        lithium::Object* potato;
        lithium::Object* block;
        lithium::Object* ocean;
        lithium::Object* tiles[numUniqueTiles];
    } _objects;

    struct Fonts
    {
        lithium::Font* permanentMarker;
    } _fonts;

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