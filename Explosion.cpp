#define SFML_NO_DEPRECATED_WARNINGS
#include "Explosion.hpp"
#include <algorithm>
#include <cstdlib>
#include <cmath>

using namespace Props;

size_t Explosion::_textureID;
std::vector<Explosion*> Explosion::_active;
std::vector<float> Explosion::_preCalculatedVx;
std::vector<float> Explosion::_preCalculatedVy;
// define static optional texture
sf::Texture Explosion::_texture;

// Persistent ground canvas for trace decals (stains). Lazily initialized in init().
static sf::RenderTexture _groundCanvas;
static bool _groundCanvasReady = false;

// Pending quads that were stamped before the ground canvas existed. They will be flushed
// into the render texture once it becomes available.
static std::vector<std::pair<sf::VertexArray, const sf::Texture*>> _pendingGroundQuads;

// Helper that calls RenderTexture::create while suppressing the deprecation warning
static bool createGroundCanvas(unsigned int w, unsigned int h)
{
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4996)
    bool ok = _groundCanvas.create(w, h);
#pragma warning(pop)
#else
    bool ok = _groundCanvas.create(w, h);
#endif
    return ok;
}

// Flush any pending quads into the ground canvas. Caller must ensure canvas is ready.
static void flushPendingGroundQuads()
{
    if (!_groundCanvasReady) return;
    if (_pendingGroundQuads.empty()) return;
    _groundCanvas.setView(_groundCanvas.getDefaultView());
    for (auto &p : _pendingGroundQuads) {
        const sf::VertexArray &quad = p.first;
        const sf::Texture* tex = p.second;
        if (tex) {
            sf::RenderStates rs;
            rs.blendMode = sf::BlendAlpha;
            rs.texture = tex;
            _groundCanvas.draw(quad, rs);
        } else {
            _groundCanvas.draw(quad);
        }
    }
    _pendingGroundQuads.clear();
}

// Draw a quad into the persistent ground canvas (decals/stains)
static void addQuadToGroundCanvas(const sf::VertexArray& quad, const sf::Texture* tex = nullptr) {
    if (!_groundCanvasReady) {
        // cache for later flush when canvas becomes available
        _pendingGroundQuads.emplace_back(quad, tex);
        return;
    }
    // ensure render texture uses default view so coordinates map to texture pixels
    _groundCanvas.setView(_groundCanvas.getDefaultView());
    if (tex) {
        sf::RenderStates rs;
        rs.blendMode = sf::BlendAlpha;
        rs.texture = tex;
        _groundCanvas.draw(quad, rs);
    } else {
        _groundCanvas.draw(quad);
    }
    // do NOT call display here; batch callers should call display once after many draws
}

Particle::Particle() { }
void Particle::update() { _x += _vx + (static_cast<float>(rand()%1000)/1000.0f - 0.5f) * 3.0f; _y += _vy + (static_cast<float>(rand()%1000)/1000.0f - 0.5f) * 3.0f; }

Explosion::Explosion() : _vertexArray(sf::Quads,4), _isBlood(false) {}

Explosion::Explosion(float x, float y, float openAngle, float angle, float speed, float size, size_t n, bool blood): _n(n), _openAngle(openAngle*0.5f), _max_speed(static_cast<int32_t>(speed)), _size(size), _isTrace(false), _isBlood(blood), _vertexArray(sf::Quads,4)
{
    _decrease = 0.1f;
    _particles.resize(_n);
    for (size_t i(_n); i--;)
    {
        Particle& p = _particles[i];
        const float sp = static_cast<float>( (_max_speed>0) ? (rand()%_max_speed) : 0 );
        const float a = (static_cast<int>(rand()%static_cast<int>(openAngle*2)) - static_cast<int>(openAngle))*0.0174532925f + angle;
        const int indexA = rand()%1000;
        // assign size first so we can offset initial Y to render particles visually above the source
        p._size = static_cast<float>(rand()%int(_size)+2);
        // small upward offset so particles appear above the zombie sprite initially
        p._x = x;
        p._y = y - (p._size * 0.5f + 4.0f);
        p._vx = sp * cosf(a); p._vy = sp * sinf(a);
        // guard against empty precomputed tables (should be initialized via init)
        if (!_preCalculatedVx.empty() && !_preCalculatedVy.empty()) {
            p._vax = _preCalculatedVx[indexA]; p._vay = _preCalculatedVy[indexA];
        } else {
            p._vax = cosf(a); p._vay = sinf(a);
        }
        if (_isBlood) {
            uint8_t r = static_cast<uint8_t>(150 + rand()%80); // red dominant
            uint8_t g = static_cast<uint8_t>(10 + rand()%40);
            uint8_t b = static_cast<uint8_t>(10 + rand()%40);
            uint8_t alpha = static_cast<uint8_t>(160 + rand()%95);
            p._color = sf::Color(r,g,b,alpha);
        } else {
            uint8_t color = static_cast<uint8_t>(50 + rand()%125);
            uint8_t alpha = static_cast<uint8_t>(100 + rand()%155);
            p._color = sf::Color(color,color,color, alpha);
        }
    }
    _traceOnEnd = true;
    _ratio = 1.0f;
}

Explosion::~Explosion() {}

Explosion* Explosion::add(float x, float y, float openAngle, float angle, float speed, float size, size_t n, bool blood){
    // Ensure pre-calculated random direction tables are initialized before use
    if (_preCalculatedVx.empty() || _preCalculatedVy.empty()) {
        Explosion::init();
    }
    Explosion* e = new Explosion(x,y,openAngle,angle,speed,size,n,blood);
    _active.push_back(e);
    return e;
}

void Explosion::setTexture(const sf::Texture& tex) {
    Explosion::_texture = tex;
}

// Allow Game to initialize ground canvas to map pixel size so decals align with world coords
void Explosion::setGroundCanvasSize(unsigned int width, unsigned int height) {
    if (width == 0 || height == 0) return;
    // recreate if not ready or size differs
    sf::Vector2u existing = _groundCanvas.getTexture().getSize();
    if (_groundCanvasReady && existing.x == width && existing.y == height) return;
    if (createGroundCanvas(width, height)) {
        _groundCanvas.clear(sf::Color::Transparent);
        _groundCanvas.display();
        _groundCanvasReady = true;
        // flush any quads that were queued while canvas wasn't ready
        flushPendingGroundQuads();
    } else {
        _groundCanvasReady = false;
    }
}

void Explosion::update(void* world){
    for (Particle& p : _particles) p.update();
    _ratio -= _decrease;
    _ratio = std::max(0.0f, _ratio);
}

void Explosion::render(sf::RenderWindow& window){
    if (_ratio > 0) {
        bool hasTex = (_isBlood && Explosion::_texture.getSize().x > 0);
        for (const Particle& p : _particles) {
            float x = p._x;
            float y = p._y;
            float sx, sy;
            if (_isTrace) {
                int indexA = rand()%1000;
                sx = p._size*_ratio*getRandVx(indexA);
                sy = p._size*_ratio*getRandVy(indexA);
            } else {
                sx = p._size*_ratio*p._vax;
                sy = p._size*_ratio*p._vay;
            }
            if (_committedToGround) continue; // already stamped to ground, no longer render above entities
            if (hasTex) {
                // draw textured sprite for blood particle
                sf::Sprite s(Explosion::_texture);
                // convert sprite into textured quad and paint into ground canvas (if available)
                sf::Vector2u ts = Explosion::_texture.getSize();
                if (ts.x == 0 || ts.y == 0) {
                    // invalid texture -> skip
                    continue;
                }
                // compute same scale as before
                float denom = static_cast<float>(std::max(1u, std::max(ts.x, ts.y)));
                float scale = (p._size * _ratio) / denom * 8.0f;
                // rotated quad half extents in world units
                float hw = (static_cast<float>(ts.x) * scale) * 0.5f;
                float hh = (static_cast<float>(ts.y) * scale) * 0.5f;
                // rotation in radians
                float rotDeg = std::atan2(p._vy, p._vx) * 180.f / 3.14159265f;
                float rot = rotDeg * 3.14159265f / 180.0f;
                float c = std::cos(rot);
                float sN = std::sin(rot);
                // local corners (unrotated): TL, TR, BR, BL
                sf::Vector2f local[4] = { {-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh} };
                sf::VertexArray quad(sf::Quads, 4);
                // texture coords map full texture
                sf::Vector2f uv[4] = { {0.f, 0.f}, {static_cast<float>(ts.x), 0.f}, {static_cast<float>(ts.x), static_cast<float>(ts.y)}, {0.f, static_cast<float>(ts.y)} };
                for (int i = 0; i < 4; ++i) {
                    float lx = local[i].x;
                    float ly = local[i].y;
                    // rotate
                    float rx = lx * c - ly * sN;
                    float ry = lx * sN + ly * c;
                    // world pos
                    quad[i].position = sf::Vector2f(x + rx, y + ry);
                    quad[i].texCoords = uv[i];
                    quad[i].color = p._color;
                }

                // For textured particles we only commit to ground when stamping is intended
                // (traces or final commit). Otherwise draw the textured sprite directly so
                // airborne blood appears above entities.
                if (!_groundCanvasReady) {
                    // lazy init default size to 2560x2560 (map default)
                    if (!createGroundCanvas(2560, 2560)) {
                        _groundCanvasReady = false;
                    } else {
                        _groundCanvas.clear(sf::Color::Transparent);
                        _groundCanvasReady = true;
                        flushPendingGroundQuads();
                    }
                }

                bool shouldCommit = false;
                if (!_isTrace) {
                    if (_traceOnEnd && _ratio - 4*_decrease < 0.0f) shouldCommit = true;
                } else {
                    // traces always commit to ground
                    shouldCommit = true;
                }

                if (shouldCommit && _groundCanvasReady) {
                    addQuadToGroundCanvas(quad, &_texture);
                } else {
                    // draw textured sprite directly to the window (airborne particle)
                    s.setOrigin(static_cast<float>(ts.x)/2.f, static_cast<float>(ts.y)/2.f);
                    s.setScale(scale, scale);
                    s.setRotation(rotDeg);
                    s.setColor(p._color);
                    s.setPosition(x, y);
                    window.draw(s);
                }
                 
             } else {
                 sf::VertexArray quad(sf::Quads, 4);
                quad[0] = sf::Vertex(sf::Vector2f(x+sx, y+sy), p._color);
                quad[1] = sf::Vertex(sf::Vector2f(x+sy, y-sx), p._color);
                quad[2] = sf::Vertex(sf::Vector2f(x-sx, y-sy), p._color);
                quad[3] = sf::Vertex(sf::Vector2f(x-sy, y+sx), p._color);

                // decide whether to commit to persistent ground canvas or render normally
                if (!_isTrace)
                {
                    if (_traceOnEnd && _ratio - 4*_decrease < 0.0f)
                    {
                        // commit final faded particles to ground (persistent decal)
                        if (!_groundCanvasReady) {
                            // lazy init ground canvas to map-size should have been set by caller; default to 2560x2560 as fallback
                            if (!createGroundCanvas(2560, 2560)) {
                                _groundCanvasReady = false;
                            } else {
                                _groundCanvas.clear(sf::Color::Transparent);
                                _groundCanvasReady = true;
                                flushPendingGroundQuads();
                            }
                        }
                        if (_groundCanvasReady) {
                            addQuadToGroundCanvas(quad);
                        } else {
                            // fallback: draw directly to window
                            window.draw(quad);
                        }
                    }
                    else {
                        // transient particle: render to window
                        window.draw(quad);
                    }
                }
                else
                {
                    // traces always go to ground canvas
                    if (!_groundCanvasReady) {
                        if (!createGroundCanvas(2560, 2560)) {
                            _groundCanvasReady = false;
                        } else {
                            _groundCanvas.clear(sf::Color::Transparent);
                            _groundCanvasReady = true;
                            flushPendingGroundQuads();
                        }
                    }
                    if (_groundCanvasReady) addQuadToGroundCanvas(quad);
                    else window.draw(quad);
                }
            }
        }
    }
}

void Explosion::commitToGround() {
    if (_committedToGround) return;
    // stamp all remaining particles as quads into the ground canvas (or pending buffer)
    bool hasTex = (_isBlood && Explosion::_texture.getSize().x > 0 && Explosion::_texture.getSize().y > 0);
    for (const Particle& p : _particles) {
        float x = p._x;
        float y = p._y;
        if (hasTex) {
            // create textured quad matching render()'s layout (without transient _ratio)
            sf::Vector2u ts = Explosion::_texture.getSize();
            float denom = static_cast<float>(std::max(1u, std::max(ts.x, ts.y)));
            float scale = (p._size) / denom * 8.0f; // similar base scale used in render
            float hw = (static_cast<float>(ts.x) * scale) * 0.5f;
            float hh = (static_cast<float>(ts.y) * scale) * 0.5f;
            float rotDeg = std::atan2(p._vy, p._vx) * 180.f / 3.14159265f;
            float rot = rotDeg * 3.14159265f / 180.0f;
            float c = std::cos(rot);
            float sN = std::sin(rot);
            sf::Vector2f local[4] = { {-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh} };
            sf::Vector2f uv[4] = { {0.f, 0.f}, {static_cast<float>(ts.x), 0.f}, {static_cast<float>(ts.x), static_cast<float>(ts.y)}, {0.f, static_cast<float>(ts.y)} };
            sf::VertexArray quad(sf::Quads, 4);
            for (int i = 0; i < 4; ++i) {
                float lx = local[i].x;
                float ly = local[i].y;
                float rx = lx * c - ly * sN;
                float ry = lx * sN + ly * c;
                quad[i].position = sf::Vector2f(x + rx, y + ry);
                quad[i].texCoords = uv[i];
                quad[i].color = p._color;
            }
            addQuadToGroundCanvas(quad, &Explosion::_texture);
        } else {
            float sx = p._size * p._vax;
            float sy = p._size * p._vay;
            sf::VertexArray quad(sf::Quads, 4);
            quad[0] = sf::Vertex(sf::Vector2f(x+sx, y+sy), p._color);
            quad[1] = sf::Vertex(sf::Vector2f(x+sy, y-sx), p._color);
            quad[2] = sf::Vertex(sf::Vector2f(x-sx, y-sy), p._color);
            quad[3] = sf::Vertex(sf::Vector2f(x-sy, y+sx), p._color);
            addQuadToGroundCanvas(quad, nullptr);
        }
    }
    _committedToGround = true;
}

void Explosion::init(){
    // idempotent init of precomputed direction vectors
    if (!_preCalculatedVx.empty() && !_preCalculatedVy.empty()) return;
    _preCalculatedVx.reserve(1000);
    _preCalculatedVy.reserve(1000);
    for (int i=0;i<1000;++i){
        float a = (static_cast<float>(rand()%1000)/1000.0f)*6.283185307f;
        _preCalculatedVx.push_back(cosf(a)); _preCalculatedVy.push_back(sinf(a));
    }
    // ensure ground canvas is initialized lazily later when needed
}

void Explosion::updateAll(float dt){
    for (int i=(int)_active.size()-1;i>=0;--i){
        _active[i]->update(nullptr);
        if (_active[i]->_ratio <= 0.0f){
            // Automatically commit traces to the ground canvas if requested and not yet committed
            if (_active[i]->_traceOnEnd && !_active[i]->_committedToGround) {
                _active[i]->commitToGround();
            }
            delete _active[i];
            _active.erase(_active.begin()+i);
        }
    }
}

void Explosion::renderAll(sf::RenderWindow& window){
    for (auto e: _active) e->render(window);
}

void Explosion::renderGround(sf::RenderWindow& window) {
    if (_groundCanvasReady) {
        // compute current view rect in world coords
        sf::View v = window.getView();
        sf::FloatRect viewRect(v.getCenter().x - v.getSize().x*0.5f, v.getCenter().y - v.getSize().y*0.5f, v.getSize().x, v.getSize().y);
        // texture size
        sf::Vector2u ts = _groundCanvas.getTexture().getSize();
        int left = static_cast<int>(std::floor(viewRect.left));
        int top = static_cast<int>(std::floor(viewRect.top));
        int w = static_cast<int>(std::ceil(viewRect.width));
        int h = static_cast<int>(std::ceil(viewRect.height));
        // clamp
        if (left < 0) left = 0;
        if (top < 0) top = 0;
        if (left >= static_cast<int>(ts.x) || top >= static_cast<int>(ts.y)) {
            // outside texture; nothing visible
        } else {
            if (left + w > static_cast<int>(ts.x)) w = static_cast<int>(ts.x) - left;
            if (top + h > static_cast<int>(ts.y)) h = static_cast<int>(ts.y) - top;
            if (w > 0 && h > 0) {
                sf::Sprite groundSprite(_groundCanvas.getTexture());
                groundSprite.setTextureRect(sf::IntRect(left, top, w, h));
                // position sprite at world coordinates matching the sub-rect
                groundSprite.setPosition(static_cast<float>(left), static_cast<float>(top));
                // draw the ground sprite
                window.draw(groundSprite);
            }
        }
        _groundCanvas.display();
    }
}

// Missing definitions for private static helpers
float Explosion::getRandVx(int i) {
    if (_preCalculatedVx.empty()) return 1.0f;
    int idx = i % static_cast<int>(_preCalculatedVx.size());
    if (idx < 0) idx += static_cast<int>(_preCalculatedVx.size());
    return _preCalculatedVx[idx];
}

float Explosion::getRandVy(int i) {
    if (_preCalculatedVy.empty()) return 0.0f;
    int idx = i % static_cast<int>(_preCalculatedVy.size());
    if (idx < 0) idx += static_cast<int>(_preCalculatedVy.size());
    return _preCalculatedVy[idx];
}
