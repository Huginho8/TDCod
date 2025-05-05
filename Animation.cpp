#include "Animation.h"
#include <iostream>

// Constructors //
Animation::Animation() { done_ = true; } // deflault

Animation::Animation(uint32_t sheetW, uint32_t sheetH, uint32_t spriteW, uint32_t spriteH, uint32_t frameCount, float speed) :
    sheetW_(sheetW),
    sheetH_(sheetH),
    spriteW_(spriteW),
    spriteH_(spriteH),
    startTime_(0),
    frameCount_(frameCount),
    animationSpeed_(speed)
{ done_ = false; }

void Animation::setTextureID(uint64_t textureID) { textureID_ = textureID; }

void Animation::resetTime(float time) { startTime_ = time; }

sf::IntRect Animation::getTexCoord(float time) {
    uint32_t currentSprite = uint32_t(animationSpeed_ * (time - startTime_));
    if (currentSprite >= frameCount_)
        done_ = true;

    currentSprite %= frameCount_;

    const int32_t textureX = currentSprite % sheetW_;
    const int32_t textureY = currentSprite / sheetW_;

    return sf::IntRect(spriteW_ * textureX, spriteH_ * textureY, spriteW_, spriteH_);
}

void Animation::applyOnQuad(sf::VertexArray& quad, float time) {
    uint32_t currentSprite = uint32_t(animationSpeed_ * (time - startTime_));
    if (currentSprite >= frameCount_)
        done_ = true;

    currentSprite %= frameCount_;

    float textureX = static_cast<float>((currentSprite % sheetW_) * spriteW_);
    float textureY = static_cast<float>((currentSprite / sheetW_) * spriteH_);

    quad[0].texCoords = sf::Vector2f(textureX, textureY);
    quad[1].texCoords = sf::Vector2f(textureX + spriteW_, textureY);
    quad[2].texCoords = sf::Vector2f(textureX + spriteW_, textureY + spriteH_);
    quad[3].texCoords = sf::Vector2f(textureX, textureY + spriteH_);
}

bool Animation::isDone() const { return done_; }