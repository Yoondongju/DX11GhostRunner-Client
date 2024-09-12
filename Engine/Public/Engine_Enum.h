#pragma once
namespace Engine
{
	enum KEY
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		Q,
		W,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,


		A,
		S,
		D,
		F,
		G,
		Z,
		X,
		C,
		V,
		B,

		ALT,
		CTRL,
		LSHIFT,
		SPACE,
		ENTER,
		ESC,

		LBUTTON,
		RBUTTON,

		ONE,
		TWO,
		THIRD,
		FOWR,

		LAST,
	};

	enum KEY_STATE
	{
		NONE, // 눌리지않고 이전에도 안눌림
		TAP,  // 막 누른시점
		HOLD, // 누르고 있는
		AWAY, // 막 뗀 시점

	};
}