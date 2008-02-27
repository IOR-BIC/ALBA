// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

#pragma once

#include <math.h>
#include "HIDDevice.h"

const static char INPUT_REPORT_BUTTONS		= 0x30;
const static char INPUT_REPORT_SET_MOTION	= 0x12;
const static char INPUT_REPORT_MOTION		= 0x31;

class MotionData
{
public:
	char x, y, z;

	MotionData() : x(0), y(0), z(0) {}
	MotionData(unsigned char x, unsigned char y, unsigned char z)
	{
		this->x = x + 128;
		this->y = y + 128;
		this->z = z + 128;
	}
};

class LED
{
protected:
	int m_index;
	bool m_state;

public:
	LED(int index) : m_index(index), m_state(false) {};

	struct LEDSet
	{
		bool led1, led2, led3, led4;

		LEDSet() : led1(0), led2(0), led3(0), led4(0) {}
		LEDSet(int s1, int s2, int s3, int s4) : led1(!!s1), led2(!!s2), led3(!!s3), led4(!!s4) {}
	};
};

class Button
{
protected:
	string	m_name;
	int		m_code;
	bool	m_state;

public:
	Button(string name, int code) : m_name(name), m_code(code), m_state(false) {};

	string	GetName() const { return m_name; }
	int		GetCode() const { return m_code; }
	bool	Pressed() const { return m_state; }

	void SetState(const bool set) { m_state = set; }
	bool SetCode(const int code);
};

class Wiimote : public HIDDevice
{
protected:
	vector<LED>		m_leds;
	vector<Button>	m_buttons;
	bool			m_input_listening;
	bool			m_rumble;
	MotionData		m_last_motion;

	unsigned int m_listener_thread_id;
	HANDLE m_listener_thread;

public:
	Wiimote();

	bool StartListening();
	bool StopListening();
	bool IsListening() const { return m_listener_thread != NULL; }
	void Disconnect();

	void ResetAllState();

	// LEDs
	void SetLEDs(bool led1, bool led2, bool led3, bool led4);
	void SetLEDs(LED::LEDSet & set);

	// Rumble
	void SetRumble(bool on);

	// Buttons
	Button GetButton(string label);
	Button GetButton(int code);
	vector<Button> SetButtons(int code);

	bool NoButtonsPressed();
	virtual void ButtonPressed(Button & b);
	virtual void ButtonReleased(Button & b);

	// Motion
	MotionData GetLastMotionData() const { return m_last_motion; }
	void RequestMotionData();

	virtual void ReceivedMotionData(MotionData & m);

	static unsigned CALLBACK Wiimote::InputReportListener(void * param);
};