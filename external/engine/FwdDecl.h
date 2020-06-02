#pragma once

#include <cstdint>
#include <memory>

namespace Wind {

class Engine;
class Graphics;
class Input;
class Audio;
class BitmapRenderer;
class TextRenderer;
class Blitter;
class Sdl;
class SdlWindow;
class SdlSound;
class SdlSurface;
class SdlMusic;
class ActionMgr;
class GlProgram;
class GlFrameBuffer;
class Font;
class UIButton;
class UICanvas;
class UIPanel;
class UIBitmap;
class UIText;
using BitmapPtr = std::shared_ptr<SdlSurface>;
using FontPtr = std::shared_ptr<Font>;
using SoundPtr = std::shared_ptr<SdlSound>;
using MusicPtr = std::shared_ptr<SdlMusic>;
enum class ProgramHandle : uint32_t;
struct Color;
struct Vec2;

} // namespace Wind
