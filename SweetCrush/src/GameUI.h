#pragma once

#include <engine/StringTable.h>

namespace Wind {

struct UIBitmapDesc;
struct UIButtonDesc;
struct TextStyle;
struct UISize;
class UIButton;
class UIText;
class UICheckBox;
class UICanvas;
struct Color;

} // namespace Wind

extern const Wind::UIBitmapDesc buttonBitmapDesc;
extern const Wind::TextStyle    titleTextStyle;
extern const Wind::TextStyle    defaultTextStyle;
extern const Wind::UISize       defaultButtonSize;

extern const float       titleY;
extern const float       button0_y;
extern const float       button1_y;
extern const float       button2_y;
extern const float       button3_y;
extern const Wind::Color button0_color;
extern const Wind::Color button1_color;
extern const Wind::Color button2_color;
extern const Wind::Color button3_color;
extern const float       text0_y;
extern const float       text1_y;
extern const Wind::Color panel0_color;

Wind::UIText     MakeTitleText(Wind::StringId label);
Wind::UIText     MakeSubTitleText(Wind::StringId label);
Wind::UIText     MakeScreenText(Wind::StringId label, float y);
Wind::UIText     MakeDynScreenText(float y);
Wind::UIButton   MakeMenuButton(float y, Wind::StringId label, const Wind::Color& color = button0_color);
Wind::UIButton   MakeBackButton();
Wind::UIButton   MakeQuitButton();
Wind::UIButton   MakeCloseButton();
Wind::UICheckBox MakeCheckBox(float y, Wind::StringId label, const Wind::Color& color);
Wind::UICanvas   MakeCanvas();
