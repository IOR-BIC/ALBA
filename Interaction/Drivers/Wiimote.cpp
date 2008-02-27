// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

#include "defines.h"
#include "assert.h"
#include "Wiimote.h"
#include "Utils.h"

#include <iostream>

Wiimote::Wiimote()
{
	// Add all Wii buttons to the our internal list
	m_buttons.push_back(Button("1",		0x0002));
	m_buttons.push_back(Button("2",		0x0001));
	m_buttons.push_back(Button("A",		0x0008));
	m_buttons.push_back(Button("B",		0x0004));
	m_buttons.push_back(Button("+",		0x1000));
	m_buttons.push_back(Button("-",		0x0010));
	m_buttons.push_back(Button("Home",	0x0080));
	m_buttons.push_back(Button("Up",	0x0800));
	m_buttons.push_back(Button("Down",	0x0400));
	m_buttons.push_back(Button("Right", 0x0200));
	m_buttons.push_back(Button("Left",	0x0100));

	m_leds.push_back(LED(1));
	m_leds.push_back(LED(2));
	m_leds.push_back(LED(3));
	m_leds.push_back(LED(4));

	m_input_listening = false;

	m_listener_thread_id = -1;
	m_listener_thread = NULL;

	m_controller_id = -1;
}

void Wiimote::SetLEDs(LED::LEDSet & set)
{
	SetLEDs(set.led1, set.led2, set.led3, set.led4);
}

void Wiimote::SetLEDs(bool led1, bool led2, bool led3, bool led4)
{
	char out = 0;

	if (led1) out |= 0x10;
	if (led2) out |= 0x20;
	if (led3) out |= 0x40;
	if (led4) out |= 0x80;

	char rpt[2] = { 0x11, out };

	WriteOutputReport(rpt);
}

void Wiimote::SetRumble(bool on)
{
	char rpt[2] = { 0x13, on ? 0x01 : 0x00 };

	WriteOutputReport(rpt);
}

unsigned CALLBACK Wiimote::InputReportListener(void * param)
{
	Wiimote * device = reinterpret_cast<Wiimote*>(param);

	while (device->m_input_listening)
	{
		unsigned char input_report[256];
		ZeroMemory(input_report, 256);
		
		DWORD result, bytes_read;
		
		// Issue a read request
		if (device->m_read_handle != INVALID_HANDLE_VALUE)
		{
			result = ReadFile(device->m_read_handle, 
				input_report, 
				device->m_capabilities.InputReportByteLength, 
				&bytes_read, 
				(LPOVERLAPPED)&device->m_hid_overlapped);																						  
		}
	 
		// Wait for read to finish
		result = WaitForSingleObject(device->m_event_object, 300);

		ResetEvent(device->m_event_object);

		// If the wait didn't result in a sucessful read, try again
		if (result != WAIT_OBJECT_0)
			continue;

		if (INPUT_REPORT_BUTTONS == input_report[0])
		{
			int key_state = Util::GetInt2(input_report, 1);

			vector<Button> changed = device->SetButtons(key_state);

			for (int i = 0; i < changed.size(); i++)
				changed[i].Pressed() ? device->ButtonPressed(changed[i]) : device->ButtonReleased(changed[i]);
		}
		else if (INPUT_REPORT_MOTION == input_report[0])
		{
			MotionData m(input_report[3], input_report[4], input_report[5]);

			int key_state = Util::GetInt2(input_report, 1);
			vector<Button> changed = device->SetButtons(key_state);

			device->m_last_motion = m;
			device->ReceivedMotionData(m);
		}
	}

	return 0;
}

void Wiimote::RequestMotionData()
{
	char o[] = { INPUT_REPORT_SET_MOTION, 0x00, INPUT_REPORT_MOTION };

	WriteOutputReport(o);
}

void Wiimote::ReceivedMotionData(MotionData & m) 
{
	//odprintf("x: %i y: %i z: %i", m.x, m.y, m.z);
}

bool Button::SetCode(const int code)
{
	bool pressed = (bool)(code & m_code);
	bool released = m_state && !pressed;

	m_state = pressed;

	return (pressed || released);
}

vector<Button> Wiimote::SetButtons(int code)
{
	vector<Button> changed_state;

	for (int i = 0; i < m_buttons.size(); i++)
	{
		if (m_buttons[i].SetCode(code))
			changed_state.push_back(m_buttons[i]);
	}

	return changed_state;
}

Button Wiimote::GetButton(string label)
{
	for (int i = 0; i < m_buttons.size(); i++)
	{
		if (m_buttons[i].GetName().compare(label) == 0)
			return m_buttons[i];
	}

	throw exception();
}

Button Wiimote::GetButton(int code)
{
	for (int i = 0; i < m_buttons.size(); i++)
	{
		if (m_buttons[i].GetCode() == code)
			return m_buttons[i];
	}

	throw exception();
}

bool Wiimote::StartListening()
{
	odprintf("Started listening: %i", m_controller_id);

	SetLEDs(0,0,0,0);

	m_input_listening = true;
	m_listener_thread = (HANDLE)_beginthreadex(
		0, 
		0, 
		InputReportListener, 
		this, 
		0, 
		&m_listener_thread_id); 

	odprintf("Created thread: %i", m_listener_thread_id);

	return false;
}

bool Wiimote::StopListening()
{
	odprintf("Stopped listening: %i", m_controller_id);

	m_input_listening = false;

	WaitForSingleObject(m_event_object, 300);

	// Needs to clean up thread -- Heap corruption exception?
	//CloseHandle(m_listener_thread);
	m_listener_thread = NULL;

	return true;
}

void Wiimote::Disconnect()
{
	if (IsListening())
		StopListening();

	CloseHandle(m_listener_thread);
	CloseHandle(m_device_handle);
	CloseHandle(m_read_handle);
	CloseHandle(m_write_handle);
}

bool Wiimote::NoButtonsPressed()
{
	for (int i = 0; i < m_buttons.size(); i++)
	{
		if (m_buttons[i].Pressed())
			return false;
	}

	return true;
}

void Wiimote::ButtonPressed(Button & b)
{
	odprintf("Button pressed: %s", b.GetName().c_str());
	SetLEDs(1, 0, 1, 0);

	SetRumble(true);
}

void Wiimote::ButtonReleased(Button & b)
{
	odprintf("Button released: %s", b.GetName().c_str());
	SetLEDs(0, 0, 0, 0);

	if (NoButtonsPressed())
		SetRumble(false);
}
