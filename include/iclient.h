#pragma once

class Character;

class IClient
{
public:
    virtual Character* onCharacterSpawned(uint8_t clientId, uint8_t charId) = 0;

    virtual void onCharacterDespawned(uint8_t clientId, uint8_t charId) = 0;

    virtual void onReceivedFromServer(uint8_t clientId, uint8_t characterId, uint8_t health, uint8_t state, float force, float aimDirection, const glm::vec3& xrz) = 0;

    virtual glm::vec3 mouseToWorldCoordinates() const = 0;

    virtual float playerToMouseAngle() const = 0;

    virtual float playerElevation() const  =0;

private:

};