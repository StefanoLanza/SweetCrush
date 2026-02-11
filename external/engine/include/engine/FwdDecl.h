#pragma once

#include <cstdint>
#include <memory>

namespace Wind {

class Engine;
class Graphics;
class Input;
class Audio;
class BitmapRenderer;
class UITextRenderer;
class Blitter;
class Sdl;
class SdlWindow;
class SdlSound;
class Texture;
class SdlMusic;
class GlProgram;
class GlFrameBuffer;
class Font;
struct UIRect;
class UIButton;
class UICanvas;
class UIPanel;
class UIBitmap;
class UIText;
class UICheckBox;
class UIRenderer;
class FontManager;
using TexturePtr = std::shared_ptr<Texture>;
using FontPtr = std::shared_ptr<Font>;
using SoundPtr = std::shared_ptr<SdlSound>;
using MusicPtr = std::shared_ptr<SdlMusic>;
enum class ProgramHandle : uint32_t;
enum class PipelineHandle : uint32_t;
class ActionMgr;
struct Color;
struct Vec2;
struct Vec3;
struct Vec4;
class INIParser;

} // namespace Wind
