#pragma once

#include <engine/FwdDecl.h>
#include <engine/StringTable.h>

extern const Wind::UITextStyle defaultTextStyle;

extern const float         titleY;
extern const float         subTitleY;
extern const float         buttonPadding;
extern const float         button0_y;
extern const float         button1_y;
extern const float         button2_y;
extern const float         button3_y;
extern const Wind::Color   button0_color;
extern const Wind::Color   button1_color;
extern const Wind::Color   button2_color;
extern const Wind::Color   button3_color;
extern const float         text0_y;
extern const float         text1_y;
extern const Wind::Color   panel0_color;
extern const Wind::Color   panel1_color;
extern const Wind::UITheme uiTheme;

Wind::UIPanel     MakeInfoPanel();
Wind::UIPanelDesc GetInfoPanelDesc();
Wind::UITextDesc  MakeTitle(Wind::StringId label, float y = titleY);
Wind::UITextDesc  MakeScreenText(Wind::StringId label, float y);
Wind::UITextDesc  MakeDynScreenText(float y);
Wind::UIButton    MakeMenuButton(float y, Wind::StringId label, const Wind::Color& color = button0_color, const char* icon = nullptr);
Wind::UIButton    MakeBackButton();
Wind::UIButton    MakeCloseButton();
Wind::UIButton    MakeCheckBox(float y, Wind::StringId label, const Wind::Color& color);
Wind::UICanvas    MakeCanvas();
