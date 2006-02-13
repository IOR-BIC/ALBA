/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgCanvasEventAdapter.cpp,v $
Language:  C++
Date:      $Date: 2006-02-13 15:49:42 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOsgCanvasEventAdapter.h"

//--------------------------------------------------------------------------------
// const
//--------------------------------------------------------------------------------

// default to no mouse buttons being pressed.
unsigned int mafOsgCanvasEventAdapter::_s_accumulatedButtonMask = 0;

int mafOsgCanvasEventAdapter::_s_button = 0;
int mafOsgCanvasEventAdapter::_s_modKeyMask = 0;
float mafOsgCanvasEventAdapter::_s_Xmin = 0;
float mafOsgCanvasEventAdapter::_s_Xmax = 1280;
float mafOsgCanvasEventAdapter::_s_Ymin = 0;
float mafOsgCanvasEventAdapter::_s_Ymax = 1024;
float mafOsgCanvasEventAdapter::_s_mx = 0;
float mafOsgCanvasEventAdapter::_s_my = 0;

//--------------------------------------------------------------------------------
mafOsgCanvasEventAdapter::mafOsgCanvasEventAdapter():
osgGA::GUIEventAdapter(osgGA::GUIEventAdapter::Y_INCREASING_UPWARDS)
//--------------------------------------------------------------------------------
{
    _eventType = NONE;           // adaptor does not encapsulate any events.
    _key = -1;                   // set to 'invalid' key value.
    _button = -1;                // set to 'invalid' button value.
    _mx = -1;                    // set to 'invalid' position value.
    _my = -1;                    // set to 'invalid' position value.
    _buttonMask = 0;             // default to no mouse buttons being pressed.
    _modKeyMask = 0;             // default to no mouse buttons being pressed.
    _time = 0.0f;                // default to no time has been set.

    copyStaticVariables();
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::copyStaticVariables()
//--------------------------------------------------------------------------------
{
    _buttonMask = _s_accumulatedButtonMask;
    _modKeyMask = _s_modKeyMask;
    _button = _s_button;
    _Xmin = _s_Xmin;
    _Xmax = _s_Xmax;
    _Ymin = _s_Ymin;
    _Ymax = _s_Ymax;
    _mx   = _s_mx;
    _my   = _s_my;
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::setWindowSize(float Xmin, float Ymin, float Xmax, float Ymax)
//--------------------------------------------------------------------------------
{
    _s_Xmin = Xmin;
    _s_Xmax = Xmax;
    _s_Ymin = Ymin;
    _s_Ymax = Ymax;
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::setButtonMask(unsigned int buttonMask)
//--------------------------------------------------------------------------------
{
    _s_accumulatedButtonMask = buttonMask;
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::adaptResize(double time, float Xmin, float Ymin, float Xmax, float Ymax)
//--------------------------------------------------------------------------------
{
    setWindowSize(Xmin,Ymin,Xmax,Ymax);
    _eventType = RESIZE;
    _time = time;
    copyStaticVariables();
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::adaptMouseScroll(double time, ScrollingMotion sm)
//--------------------------------------------------------------------------------
{
    _time = time;

    if (sm == ScrollUp)
	_eventType = SCROLLUP;
    else if (sm == ScrollDown)
	_eventType = SCROLLDOWN;
    else
	_eventType = NONE;

    copyStaticVariables();
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::adaptButtonPress(double time,float x, float y, unsigned int button)
//--------------------------------------------------------------------------------
{
    _time = time;

    _eventType = PUSH;
    _button = button-1;

    switch(_button)
    {
        case(0): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask | LEFT_MOUSE_BUTTON; 
	    _s_button = LEFT_MOUSE_BUTTON;
	    break;
        case(1): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask | MIDDLE_MOUSE_BUTTON; 
	    _s_button = MIDDLE_MOUSE_BUTTON;
	    break;
        case(2): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask | RIGHT_MOUSE_BUTTON; 
	    _s_button = RIGHT_MOUSE_BUTTON;
	    break;
    }

    _s_mx = x;
    _s_my = y;

    copyStaticVariables();
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::adaptButtonRelease(double time,float x, float y, unsigned int button)
//--------------------------------------------------------------------------------
{
    _time = time;

    _eventType = RELEASE;
    _button = button-1;

    switch(_button)
    {
        case(0): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask & ~LEFT_MOUSE_BUTTON;
	    _s_button = LEFT_MOUSE_BUTTON;
	    break;
        case(1): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask & ~MIDDLE_MOUSE_BUTTON; 
	    _s_button = MIDDLE_MOUSE_BUTTON;
	    break;
        case(2): 
	    _s_accumulatedButtonMask = 
		_s_accumulatedButtonMask & ~RIGHT_MOUSE_BUTTON; 
	    _s_button = RIGHT_MOUSE_BUTTON;
	    break;
    }

    _s_mx = x;
    _s_my = y;

    copyStaticVariables();
}
//--------------------------------------------------------------------------------
/** method for adapting mouse motion events whilst mouse buttons are pressed.*/
void mafOsgCanvasEventAdapter::adaptMouseMotion(double time, float x, float y)
//--------------------------------------------------------------------------------
{
    
    _eventType = (_s_accumulatedButtonMask) ?
                 DRAG :
                 MOVE;

    _time = time;
    _s_mx = x;
    _s_my = y;
    copyStaticVariables();

}
//--------------------------------------------------------------------------------
/** method for adapting keyboard events.*/
void mafOsgCanvasEventAdapter::adaptKeyPress( double time, KeySymbol key)
//--------------------------------------------------------------------------------
{
    _eventType = KEYDOWN;
    _time = time;
    _key = key;
    
    switch(key)
    {
        case(KEY_Shift_L):      _s_modKeyMask = MODKEY_LEFT_SHIFT | _s_modKeyMask; break;
        case(KEY_Shift_R):      _s_modKeyMask = MODKEY_RIGHT_SHIFT | _s_modKeyMask; break;
        case(KEY_Control_L):    _s_modKeyMask = MODKEY_LEFT_CTRL | _s_modKeyMask; break;
        case(KEY_Control_R):    _s_modKeyMask = MODKEY_RIGHT_CTRL | _s_modKeyMask; break;
        case(KEY_Meta_L):       _s_modKeyMask = MODKEY_LEFT_META | _s_modKeyMask; break;
        case(KEY_Meta_R):       _s_modKeyMask = MODKEY_RIGHT_META | _s_modKeyMask; break;
        case(KEY_Alt_L):        _s_modKeyMask = MODKEY_LEFT_ALT | _s_modKeyMask; break;
        case(KEY_Alt_R):        _s_modKeyMask = MODKEY_LEFT_ALT | _s_modKeyMask; break;

        case(KEY_Caps_Lock):
        {
            if ((_s_modKeyMask & MODKEY_CAPS_LOCK)!=0) 
                _s_modKeyMask = ~MODKEY_CAPS_LOCK & _s_modKeyMask;
            else 
                _s_modKeyMask = MODKEY_CAPS_LOCK | _s_modKeyMask; 
            break;
        }
        case(KEY_Num_Lock):
        {
            if ((_s_modKeyMask & MODKEY_NUM_LOCK)!=0)
                 _s_modKeyMask = ~MODKEY_NUM_LOCK & _s_modKeyMask;
            else
                 _s_modKeyMask = MODKEY_NUM_LOCK | _s_modKeyMask;
            break;
        }
    }        

    copyStaticVariables();
}
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::adaptKeyRelease( double time, KeySymbol key)
//--------------------------------------------------------------------------------
{
    // we won't handle this correctly right now.. GUIEventAdapter isn't up to it
    _eventType = KEYUP;
    _time = time;
    _key = key;

    switch(key)
    {
        case(KEY_Shift_L):      _s_modKeyMask = ~MODKEY_LEFT_SHIFT & _s_modKeyMask; break;
        case(KEY_Shift_R):      _s_modKeyMask = ~MODKEY_RIGHT_SHIFT & _s_modKeyMask; break;
        case(KEY_Control_L):    _s_modKeyMask = ~MODKEY_LEFT_CTRL & _s_modKeyMask; break;
        case(KEY_Control_R):    _s_modKeyMask = ~MODKEY_RIGHT_CTRL & _s_modKeyMask; break;
        case(KEY_Meta_L):       _s_modKeyMask = ~MODKEY_LEFT_META & _s_modKeyMask; break;
        case(KEY_Meta_R):       _s_modKeyMask = ~MODKEY_RIGHT_META & _s_modKeyMask; break;
        case(KEY_Alt_L):        _s_modKeyMask = ~MODKEY_LEFT_ALT & _s_modKeyMask; break;
        case(KEY_Alt_R):        _s_modKeyMask = ~MODKEY_LEFT_ALT & _s_modKeyMask; break;
    }        
    copyStaticVariables();
}
//--------------------------------------------------------------------------------
/** method for adapting frame events, i.e. iddle/display callback.*/
//--------------------------------------------------------------------------------
void mafOsgCanvasEventAdapter::adaptFrame(double time)
{
    _eventType = FRAME;
    _time = time;
    copyStaticVariables();
}


