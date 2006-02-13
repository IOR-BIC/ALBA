/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: wxWinEventAdapter.h,v $
Language:  C++
Date:      $Date: 2006-02-13 15:49:13 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __wxWinEventAdapter_h__
#define __wxWinEventAdapter_h__

#include <osgGA/GUIEventAdapter>

//--------------------------------------------------------------------------------
class wxWinEventAdapter : public osgGA::GUIEventAdapter
//--------------------------------------------------------------------------------
{
public:
    enum ScrollingMotion {
        ScrollNone,
        ScrollUp,
        ScrollDown
    };

public:
    wxWinEventAdapter();
    virtual ~wxWinEventAdapter() {}

    /** Get the EventType of the GUI event.*/
    virtual EventType getEventType() const { return _eventType; }

    /** key pressed, return -1 if inappropriate for this event. */
    virtual int getKey() const { return _key; }

    /** button pressed/released, return -1 if inappropriate for this event.*/
    virtual int getButton() const { return _button; }

    /** window minimum x. */
    virtual float getXmin() const  { return _Xmin; }

    /** window maximum x. */
    virtual float getXmax() const  { return _Xmax; }

    /** window minimum y. */
    virtual float getYmin() const { return _Ymin; }

    /** window maximum y. */
    virtual float getYmax() const { return _Ymax; }

    /** current mouse x position.*/
    virtual float getX() const { return _mx; }

    /** current mouse y position.*/
    virtual float getY() const { return _my; }

    /** current mouse button state */
    virtual unsigned int getButtonMask() const { return _buttonMask; }

    /** time in seconds of event. */
    virtual double time() const { return _time; }

    virtual unsigned int getModKeyMask() const { return _modKeyMask; }

    /** static method for setting window dimensions.*/
    static void setWindowSize(float Xmin, float Ymin, float Xmax, float Ymax);

    /** static method for setting button state.*/
    static void setButtonMask(unsigned int buttonMask);


    /** method for adapting resize events. */
    void adaptResize(double t, float Xmin, float Ymin, float Xmax, float Ymax);

    /** method for adapting mouse scroll wheel events. */
    void adaptMouseScroll(double t, ScrollingMotion sm);

    /** method for adapting mouse motion events whilst mouse buttons are pressed.*/
    void adaptMouseMotion(double t, float x, float y);

    void adaptButtonPress(double t,float x, float y, unsigned int button);
    
    void adaptButtonRelease(double t,float x, float y, unsigned int button);

    /** method for adapting keyboard events.*/
    void adaptKeyPress( double t, KeySymbol key);

    void adaptKeyRelease( double t, KeySymbol key);

    /** method for adapting frame events, i.e. idle/display callback.*/
    void adaptFrame(double t);


    void copyStaticVariables();

protected:
    
    EventType _eventType;
    int _key;
    int _button;
    float _Xmin,_Xmax;
    float _Ymin,_Ymax;
    float _mx;
    float _my;
    unsigned int _buttonMask;
    unsigned int _modKeyMask;
    double _time;

public:

    // used to accumulate the button mask state, it represents
    // the current button mask state, which is modified by the
    // adaptMouse() method which then copies it to value _buttonMask
    // which required the mouse buttons state at the time of the event.
    static unsigned int _s_accumulatedButtonMask;
    
    // used to store current button value
    static int _s_button;

    // used to store window min and max values.
    static float _s_Xmin;
    static float _s_Xmax;
    static float _s_Ymin;
    static float _s_Ymax;
    static float _s_mx;
    static float _s_my;
    static int _s_modKeyMask;
};

#endif //__wxWinEventAdapter_h__
