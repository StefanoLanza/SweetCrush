#pragma once

#include <engine/StringTable.h>

namespace Wind {

struct UIBitmapDesc;
struct UITextDesc;
struct DrawTextArgs;
struct UISize;
class UIPanel;
class UIButton;
class UIText;
class UICheckBox;
class UICanvas;
struct Color;
struct UITextStyle;

} // namespace Wind

extern const Wind::UIBitmapDesc noIconDesc;
extern const Wind::UITextDesc   noLabelDesc;
extern const Wind::UITextStyle defaultTextStyle;
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
extern const Wind::Color panel1_color;

Wind::UIPanel    MakeInfoPanel();
Wind::UIText     MakeTitleText(Wind::StringId label);
Wind::UIText     MakeSubTitleText(Wind::StringId label);
Wind::UIText     MakeScreenText(Wind::StringId label, float y);
Wind::UIText     MakeDynScreenText(float y);
Wind::UIButton   MakeMenuButton(float y, Wind::StringId label, const Wind::Color& color = button0_color, const char* icon = nullptr);
Wind::UIButton   MakeBackButton();
Wind::UIButton   MakeCloseButton();
Wind::UICheckBox MakeCheckBox(float y, Wind::StringId label, const Wind::Color& color);
Wind::UICanvas   MakeCanvas();
