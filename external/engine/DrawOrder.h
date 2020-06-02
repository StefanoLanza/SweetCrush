#pragma once

namespace Wind {

using DrawOrderType = unsigned int;

enum DrawOrder : DrawOrderType {
	background = 0,
	user,
	UI = 100,
	mousePointer = 200,
};

} // namespace Wind
