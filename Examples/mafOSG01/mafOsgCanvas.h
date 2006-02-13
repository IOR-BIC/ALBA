/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgCanvas.h,v $
Language:  C++
Date:      $Date: 2006-02-13 15:49:42 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafOsgCanvas_h__
#define __mafOsgCanvas_h__

#if !wxUSE_GLCANVAS
#error Please set wxUSE_GLCANVAS to 1 in setup.h.
#endif

// forward declaration
class MessageDlg;
class mafOsgCanvasEventAdapter;

#include <assert.h>

#include "wx/notebook.h"
#include "wx/statline.h"
#include "wx/tglbtn.h"
#include "wx/glcanvas.h"
#include "wx/dnd.h"
#include "wx/textctrl.h"

#include <osg/GL>
#include <osg/GLU>
#include <osg/Fog>
#include <osg/Light>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Node>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Timer>
#include <osg/lightsource>
#include <osg/Notify>
#include <osg/Quat>

#include <osgGA/GUIActionAdapter>
#include <osgGA/MatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/MatrixManipulator>
#include <osgGA/StateSetManipulator>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/DatabasePager>

#include <osgUtil/SceneView>
#include <osgUtil/Optimizer>
#include <osgUtil/UpdateVisitor>
#include <osgUtil/IntersectVisitor>

#include <osgProducer/EventAdapter>

#include <OpenThreads/Mutex>

//--------------------------------------------------------------------------------
class mafOsgCanvas: public wxGLCanvas, public osgGA::GUIActionAdapter
//--------------------------------------------------------------------------------
{
public:
    // EventQueue
    typedef std::vector< osg::ref_ptr<mafOsgCanvasEventAdapter> > EventQueueType;

    // timer IDs
    enum Timers {
        RenderTimer = 1000
    };

    // wxWindows mouse button IDs
    enum MouseButtonId {
        LeftMouseButtonId = 1,
        MiddleMouseButtonId = 2,
        RightMouseButtonId = 3
    };

    // default matrix manipulator IDs
    enum ManipulatorId {
        TrackballManipulatorId = 0,
        FlightManipulatorId = 1,
        DriveManipulatorId = 2,
        AuditionManipulatorId = 3
    };

public:
    mafOsgCanvas(wxWindow *parent,const wxWindowID id = -1, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,long style = 0);
   ~mafOsgCanvas(void);

private:
    void OnLeftDown(wxMouseEvent &event);
    void OnMiddleDown(wxMouseEvent &event);
    void OnRightDown(wxMouseEvent &event);

    void OnLeftUp(wxMouseEvent &event);
    void OnMiddleUp(wxMouseEvent &event);
    void OnRightUp(wxMouseEvent &event);

    void OnMotion(wxMouseEvent &event);
    void OnMouseWheel(wxMouseEvent &event);

    void OnChar(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent &event)   {event.Skip();};
    void OnKeyDown(wxKeyEvent &event) {event.Skip();};

    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnEraseBackground(wxEraseEvent &event) {/* Do nothing, to avoid flashing on MSW */};
    void OnTimer(wxTimerEvent &event) {Render();};

public:
    /** add camera manipulator to the matrix manipulator list. Return index to camera in list. */
    unsigned int AddCameraManipulator(osgGA::MatrixManipulator *cm);

    /** set current manipulator by index */
    bool SetCurrentManipulator(const unsigned int index);

    /** return current manipulator index */
    const unsigned int CurrentManipulator() { return m_CurrentManipulatorIndex; }

    /** reset current manipulator to home positionq */
    void ResetManipulator();

    /** attempt to load a model by filename */
    bool LoadModel(const char *filename);   

    /** save model to file */
    bool SaveModel(const char *filename);

    /** clear currently loaded model */
    void ClearModel();

    /** enable lighting */
    bool EnableLighting(bool enable = true);

    /** return true if lighting is enabled */
    bool LightingIsEnabled();

protected:
    bool InitOsg();
        // Init OpenSceneGraph
    void Render();
        // Render
    double GetTime() { return osg::Timer::instance()->delta_s(m_StartTick, osg::Timer::instance()->tick()); }
        // get time
    void EventQueue(EventQueueType& queue);
        // return event queue
    void AddEvent(osg::ref_ptr<mafOsgCanvasEventAdapter> ea);
        // add event to the queue
    void UpdateCurrentManipulator(osg::FrameStamp *fs);
        // update current matrix manipulator
    void SetManipulatorNode(osg::Node *node) { m_ManipulatorNode = node; }
        // set manipulator node

    // GuiActionAdapter methods

    virtual void requestRedraw() {}
        // request redraw (ignore)
    virtual void requestContinuousUpdate(bool) {}
        // request continuous update (ignore)
    virtual void requestWarpPointer(float x, float y);
        // request mouse cursor position

private:
    // WDR: member variable declarations for mafOsgCanvas
    wxTimer *m_Timer;
        // timer
    osg::Timer_t m_StartTick;
        // start tick
    EventQueueType m_EventQueue;
        // event queue
    OpenThreads::Mutex m_EventQueueMutex;
        // event queue lock
    int m_ViewWidth;
        // viewport width
    int m_ViewHeight;
        // viewport height
    osg::ref_ptr<osgUtil::SceneView> m_SceneView;
        // scene view
    std::vector<osg::ref_ptr<osgGA::MatrixManipulator> > m_ManipulatorList;
        // list of  matrix manipulator
    osgGA::MatrixManipulator *m_CurrentManipulator;
        // current matrix manipulator
    unsigned int m_CurrentManipulatorIndex;
        // current manipulator index
    osg::ref_ptr<osg::Node> m_ManipulatorNode;
        // manipulatotr scene graph
    osg::ref_ptr<osg::Group> m_SceneGraph;
        // the root of our scenegraph
    osg::ref_ptr<osg::Group> m_ModelGroup;
        // model group
    osg::ref_ptr<osg::StateSet> m_GlobalStateSet;
        // global state set
    osg::ref_ptr<osgGA::StateSetManipulator> m_StateSetManipulator;
        // state set manipulator
    unsigned int m_FrameNumber;
        // frame number
    osg::ref_ptr<osg::NodeVisitor> m_UpdateVisitor;
        // animation / particle update visitor
    osgDB::DatabasePager* m_DatabasePager;
        // database pager

    bool isInit;

private:
    DECLARE_EVENT_TABLE()
};

#endif //__mafOsgCanvas_h__



