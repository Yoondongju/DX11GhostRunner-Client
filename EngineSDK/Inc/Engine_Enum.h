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
		NONE, // �������ʰ� �������� �ȴ���
		TAP,  // �� ��������
		HOLD, // ������ �ִ�
		AWAY, // �� �� ����

	};
}