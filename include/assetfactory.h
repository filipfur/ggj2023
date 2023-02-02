#pragma once

#include "glfont.h"
#include "globject.h"
#include "glanimatedobject.h"
#include "gltinyobjloader.h"
#include "gltfloader.h"
#include "glskinnedobject.h"

class AssetFactory
{
public:
    struct Meshes
    {
        lithium::Mesh* screen;
        lithium::Mesh* block;
    } _meshes;

    struct Textures
    {
        lithium::ImageTexture* permanentMarkerSdf;
        lithium::ImageTexture* blockDiffuse;
    } _textures;

    struct Objects
    {
        lithium::Object* block;
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