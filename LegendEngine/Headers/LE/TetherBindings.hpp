#pragma once

#include <Tether/Tether.hpp>

namespace le
{
	namespace Input = Tether::Input;
	namespace Utils = Tether;

	using Window = Tether::Window;
	using Keycodes = Tether::Keycodes;
	using KeyInfo = Input::KeyInfo;
	using RawMouseMoveInfo = Input::RawMouseMoveInfo;
	using MouseClickInfo = Input::MouseClickInfo;
	using InputListener = Input::InputListener;
	using InputType = Input::InputType;
}
