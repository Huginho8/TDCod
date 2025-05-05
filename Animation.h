#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>

class Animation {

public:
	// Constructors
	Animation();
	Animation(uint32_t sheetW, uint32_t sheetH, uint32_t spriteW, uint32_t spriteH, uint32_t frameCount, float speed);

	// Setters
	void setTextureID(uint64_t textureID);
	void setCenter(sf::Vector2f center) { spriteCenter_ = center; }
	void setDone() { done_ = true; }
	void resetTime(float time);

	// Getters
	uint64_t getTexture() const { return textureID_; }
	sf::IntRect getTexCoord(float time);
	sf::Vector2f getSpriteSize() const { return sf::Vector2f(static_cast<float>(spriteW_), static_cast<float>(spriteH_)); }
	sf::Vector2f getSpriteCenter() const { return spriteCenter_; }
	uint64_t getCurrentIndex(float time) const { return static_cast<size_t>(animationSpeed_ * (time - startTime_)); }
	bool isDone() const;

	// Apply current frame texture coords to quad
	void applyOnQuad(sf::VertexArray& quad, float time);

private:
	uint32_t sheetW_;
	uint32_t sheetH_;

	uint32_t spriteW_;
	uint32_t spriteH_;
	sf::Vector2f spriteCenter_;

	uint32_t frameCount_; // total frames
	float startTime_; 
	float animationSpeed_;
	bool done_;

	uint64_t textureID_;
};

#endif // ANIMATION_H