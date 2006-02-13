/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgCanvas.cpp,v $
Language:  C++
Date:      $Date: 2006-02-13 15:49:41 $
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

#include "mafOsgCanvas.h"
#include "mafOsgCanvasEventAdapter.h"

#if !wxUSE_DRAG_AND_DROP
    #error This application requires drag and drop support in the library
#endif
#include <wx/dnd.h>

//-----------------------------------------------------------------------
// forward declaration
//-----------------------------------------------------------------------
class mafOsgCanvas;

//-----------------------------------------------------------------------
// class mafOsgCanvasDropTarget
//-----------------------------------------------------------------------
/** description: handle drag and drop operations.  Requires knowledge of 
    mafOsgCanvas class.  On drop, will call mafOsgCanvas::LoadModel */

class mafOsgCanvasDropTarget : public wxFileDropTarget
{
public:
    mafOsgCanvasDropTarget(mafOsgCanvas *canvas) { m_Canvas = canvas; }
   ~mafOsgCanvasDropTarget() {}

    /** description: handle drop file operation.  Attempt to load a model.
     - param: x,y  - mouse position
     - param: filenames - filename list
     - return: true if dropped file was accepted, else false */
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
        bool ret = false;
        if (filenames.GetCount() == 1 && m_Canvas != NULL ) 
                ret = m_Canvas->LoadModel(filenames[0].c_str());
        return ret;
    }
private:
    mafOsgCanvas *m_Canvas;
};


//------------------------------------------------------------------------------
// event table for mafOsgCanvas
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafOsgCanvas, wxGLCanvas)
  EVT_LEFT_DOWN(mafOsgCanvas::OnLeftDown)
  EVT_MIDDLE_DOWN(mafOsgCanvas::OnMiddleDown)
  EVT_RIGHT_DOWN(mafOsgCanvas::OnRightDown)

  EVT_LEFT_UP(mafOsgCanvas::OnLeftUp)
  EVT_MIDDLE_UP(mafOsgCanvas::OnMiddleUp)
  EVT_RIGHT_UP(mafOsgCanvas::OnRightUp)

  EVT_MOTION(mafOsgCanvas::OnMotion)
  EVT_MOUSEWHEEL(mafOsgCanvas::OnMouseWheel)

  EVT_CHAR(mafOsgCanvas::OnChar)
  EVT_KEY_DOWN(mafOsgCanvas::OnKeyDown)
  EVT_KEY_UP(mafOsgCanvas::OnKeyUp)

  EVT_SIZE(mafOsgCanvas::OnSize)
  EVT_PAINT(mafOsgCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(mafOsgCanvas::OnEraseBackground)
  EVT_TIMER(RenderTimer, mafOsgCanvas::OnTimer)

END_EVENT_TABLE()

//------------------------------------------------------------------------------
mafOsgCanvas::mafOsgCanvas(wxWindow *parent, wxWindowID id,const  wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
:   wxGLCanvas(parent, id, pos, size, style),
    m_Timer(NULL),
    m_ViewWidth(0),
    m_ViewHeight(0),

    m_SceneView(NULL),
    m_CurrentManipulator(NULL),
    m_CurrentManipulatorIndex(0),
    m_ManipulatorNode(NULL),
    m_SceneGraph(NULL),
    m_ModelGroup(NULL),
    m_GlobalStateSet(NULL),
    m_FrameNumber(0),
    m_UpdateVisitor(NULL),
    m_DatabasePager(NULL)
{
  isInit = false;

  // set notification level
  //osg::setNotifyLevel(osg::NOTICE);
  osg::setNotifyLevel(osg::FATAL);

  // store start tick
  m_StartTick = osg::Timer::instance()->tick();

  // setup periodic timer for rendering
  m_Timer = new wxTimer(this, RenderTimer);
  m_Timer->Start(5);

  // enable drag and drop of model files
  //SetDropTarget(new DragAndDrop(this));

  // create the sceneview
  m_SceneView = new osgUtil::SceneView();
  m_SceneView->setDefaults();

  // enable drag and drop of model files
  SetDropTarget(new mafOsgCanvasDropTarget(this));

  // the rest of the Initialization is delayed at
  // the first Paint-Event -- need to have an OpenGL Context set.

  assert(GetContext()); 
  SetCurrent();
  InitOsg();
}
//------------------------------------------------------------------------------
mafOsgCanvas::~mafOsgCanvas(void)
//------------------------------------------------------------------------------
{
    // stop database pager thread
    osgDB::Registry::instance()->setDatabasePager(0);

    if (m_Timer) { delete m_Timer; m_Timer = NULL; }
}


//..............................................................................
void mafOsgCanvas::EventQueue(EventQueueType &queue)
//..............................................................................
{
  queue.clear();
  m_EventQueueMutex.lock();
  m_EventQueue.swap(queue);
  m_EventQueueMutex.unlock();
}
//..............................................................................
void mafOsgCanvas::AddEvent(osg::ref_ptr<mafOsgCanvasEventAdapter> ea)
//..............................................................................
{
  // lock event queue access
  m_EventQueueMutex.lock();
  // add adapter to event queue
  m_EventQueue.push_back(ea);
  // unlock event queue access
  m_EventQueueMutex.unlock();
}
//..............................................................................
void mafOsgCanvas::OnLeftDown(wxMouseEvent &event)
//..............................................................................
{
  CaptureMouse();

  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  const int button = LeftMouseButtonId;
  ea->adaptButtonPress(GetTime(), event.GetX(), m_ViewHeight - event.GetY(),button);
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnMiddleDown(wxMouseEvent &event)
//..............................................................................
{
  CaptureMouse();

  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  const int button = MiddleMouseButtonId;
  ea->adaptButtonPress(GetTime(), event.GetX(), m_ViewHeight - event.GetY(),button);
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnRightDown(wxMouseEvent &event)
//..............................................................................
{
  CaptureMouse();

  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  const int button = RightMouseButtonId;
  ea->adaptButtonPress(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnLeftUp(wxMouseEvent &event)
//..............................................................................
{
  if (HasCapture()) ReleaseMouse();

  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  const int button = LeftMouseButtonId;
  ea->adaptButtonRelease(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnMiddleUp(wxMouseEvent &event)
//..............................................................................
{
  if (HasCapture()) ReleaseMouse();

  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  const int button = MiddleMouseButtonId;
  ea->adaptButtonRelease(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnRightUp(wxMouseEvent &event)
//..............................................................................
{
  if (HasCapture()) ReleaseMouse();

  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  const int button = RightMouseButtonId;
  ea->adaptButtonRelease(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnMotion(wxMouseEvent &event)
//..............................................................................
{
  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  ea->adaptMouseMotion(GetTime(), event.GetX(), m_ViewHeight - event.GetY());
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnMouseWheel(wxMouseEvent &event)
//..............................................................................
{
  int wheelIncr = event.GetWheelRotation() / event.GetWheelDelta();

  mafOsgCanvasEventAdapter::ScrollingMotion sm = mafOsgCanvasEventAdapter::ScrollNone;

  if (wheelIncr > 0) {
    sm = mafOsgCanvasEventAdapter::ScrollUp;
  } else if (wheelIncr < 0) {
    sm = mafOsgCanvasEventAdapter::ScrollDown;
  }

  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  ea->adaptMouseScroll(GetTime(), sm);
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::OnChar(wxKeyEvent& event)
//..............................................................................
{
  osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
  ea->adaptKeyPress(GetTime(), mafOsgCanvasEventAdapter::KeySymbol(event.GetKeyCode()));
  AddEvent(ea);
}
//..............................................................................
void mafOsgCanvas::ResetManipulator()
//..............................................................................
{
    osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
    ea->adaptFrame(0.0f);

    // assert that current manipulator exists
    assert(m_CurrentManipulator != NULL);

    if (m_CurrentManipulator != NULL) {
        // reset manipulator to home position
        m_CurrentManipulator->home(*ea, *this);
    }
}













//------------------------------------------------------------------------------
bool mafOsgCanvas::EnableLighting(bool enable)
//------------------------------------------------------------------------------
{
  if (m_GlobalStateSet == NULL) return false;

  osg::StateAttribute::GLModeValue val;
  if (enable) 
    val = osg::StateAttribute::ON;
  else 
    val = osg::StateAttribute::OVERRIDE  | osg::StateAttribute::OFF;

  m_GlobalStateSet->setMode(GL_LIGHTING, val);

  return true;
}
//------------------------------------------------------------------------------
bool mafOsgCanvas::LightingIsEnabled()
//------------------------------------------------------------------------------
{
  if (m_GlobalStateSet == NULL) return false;

  osg::StateAttribute::GLModeValue isLight = 
    m_GlobalStateSet->getMode(GL_LIGHTING);

  if (isLight == osg::StateAttribute::ON) 
    return true;
  else
    return false;
}
//------------------------------------------------------------------------------
void mafOsgCanvas::ClearModel()
//------------------------------------------------------------------------------
{
  // stop database pager thread
  osgDB::Registry::instance()->setDatabasePager(0);

  if (m_ModelGroup != NULL) 
  {
    while (m_ModelGroup->getNumChildren() > 0) 
    {
      m_ModelGroup->removeChild(m_ModelGroup->getChild(0));
    }
  }
}
//------------------------------------------------------------------------------
bool mafOsgCanvas::SaveModel(const char *filename)
//------------------------------------------------------------------------------
{
  bool ret = false;

  if (m_ModelGroup != NULL) 
    ret = osgDB::writeNodeFile(*(osg::Node*)m_ModelGroup.get(), filename);

  return ret;
}
//------------------------------------------------------------------------------
bool mafOsgCanvas::LoadModel(const char *filename)
//------------------------------------------------------------------------------
{
  bool ret = false;

  // enable node creation from an image file
  osgDB::Registry::instance()->setCreateNodeFromImage(true);

  // attempt to load the mode
  osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);

  assert(m_ModelGroup.get());

  if (loadedModel != NULL) {

    // clear current model
    ClearModel();

    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    // update the scene graph
    m_ModelGroup->addChild(loadedModel.get());

    // mark as successful
    ret = true;

    // set the scene to render
    SetManipulatorNode(m_SceneGraph.get());

    // reset manipulator (home position)
    SetCurrentManipulator(m_CurrentManipulatorIndex);

    // reset current manipulator
    ResetManipulator();
  }
  return ret;
}






//------------------------------------------------------------------------------
unsigned int mafOsgCanvas::AddCameraManipulator(osgGA::MatrixManipulator *cm)
//------------------------------------------------------------------------------
{
  int ret = 0;

  if (cm != NULL) {
    // add camera to the manipulator list
    m_ManipulatorList.push_back(cm);

    // return index of matrix manipulator list
    ret = m_ManipulatorList.size() - 1;
  }

  return ret;
}
//------------------------------------------------------------------------------
void mafOsgCanvas::requestWarpPointer(float x, float y)
//------------------------------------------------------------------------------
{
  WarpPointer(x, y);
}
//------------------------------------------------------------------------------
bool mafOsgCanvas::SetCurrentManipulator(const unsigned int index) 
//------------------------------------------------------------------------------
{
  bool ret = false;

  // assert that index is legit
  assert(m_ManipulatorList.size() > index);

  if (m_ManipulatorList.size() > index) {

    // current matrix
    osg::Matrix curMat;

    if (m_CurrentManipulator != NULL) {
      // retrieve current matrix from the manipulator
      curMat = m_CurrentManipulator->getMatrix();
    }

    // update current manipulator index
    m_CurrentManipulatorIndex = index;

    // update current manipulator
    m_CurrentManipulator = m_ManipulatorList[index].get();

    // success
    ret = true;

    // assert that current manipulator exists
    assert(m_CurrentManipulator != NULL);

    if (m_CurrentManipulator != NULL) {
      if (m_ManipulatorNode != NULL) {
        // set manipulator's scene graph
        m_CurrentManipulator->setNode(m_ManipulatorNode.get());
      }

      // update new manipulator with current matrix
      m_CurrentManipulator->setByMatrix(curMat);
    }
  }
  return ret;
}
//------------------------------------------------------------------------------
/** mafOsgCanvas::UpdateCurrentManipulator
description: update current matrix manipulator
param: fs - frame stamp */
void mafOsgCanvas::UpdateCurrentManipulator(osg::FrameStamp *fs)
//------------------------------------------------------------------------------
{
    // get the event since the last frame.
    EventQueueType queue;
    EventQueue(queue);    //mah - SIL

    // create an event to signal the new frame.
    osg::ref_ptr<mafOsgCanvasEventAdapter> frameEvent = new mafOsgCanvasEventAdapter;
    frameEvent->adaptFrame(fs->getReferenceTime());
    // add event to the queue
    queue.push_back(frameEvent);

    // dispatch the events in order of arrival.
    for(EventQueueType::iterator eventItr = queue.begin();
        eventItr != queue.end();
        ++eventItr)
    {
        if (m_CurrentManipulator != NULL) {
            // update current manipulator with event
            m_CurrentManipulator->handle(*(*eventItr), *this);
        }

        if (m_StateSetManipulator != NULL) {
            m_StateSetManipulator->handle(*(*eventItr), *this);
        }
    }

    if (m_SceneView != NULL) {
        // update view matrix from manipulator
        m_SceneView->setViewMatrix(m_CurrentManipulator->getInverseMatrix());
    }
}
//------------------------------------------------------------------------------
void mafOsgCanvas::OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
{
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  // get updated viewport size
  GetClientSize(&m_ViewWidth, &m_ViewHeight);

  if (m_SceneView != NULL) {

    // create event
    osg::ref_ptr<mafOsgCanvasEventAdapter> ea = new mafOsgCanvasEventAdapter;
    // adapt to osgGa
    ea->adaptResize(GetTime(), 0, 0, m_ViewWidth, m_ViewHeight);
    // add event to the queue
    AddEvent(ea);

    // update viewport size
    m_SceneView->setViewport(0, 0, m_ViewWidth, m_ViewHeight);

    osg::Matrix projMat;

    projMat.makePerspective(
      45.0f, 
      (double)m_ViewWidth / m_ViewHeight, 
      1.0f, 
      12000.0f);

    m_SceneView->setProjectionMatrix(projMat);
  }
}
//------------------------------------------------------------------------------
void mafOsgCanvas::OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
{
  /* must always be here */
  wxPaintDC dc(this);
}
//------------------------------------------------------------------------------
bool mafOsgCanvas::InitOsg()
//------------------------------------------------------------------------------
{
    bool ret = false;

    assert(m_SceneView != NULL);

    if (m_SceneView != NULL) {

        ret = true;

        // create the root of our scene graph
        m_SceneGraph = new osg::Group;
        // create the light group
        osg::ref_ptr<osg::Group> lightGroup = new osg::Group;
        // create the model group
        m_ModelGroup = new osg::Group;
        
        m_SceneGraph->addChild(lightGroup.get());

        // add model group as a child of the scene graph group
        lightGroup->addChild(m_ModelGroup.get());

        if (m_SceneView != NULL) {
            m_SceneView->setSceneData(m_SceneGraph.get());
        }
    
        ///////////////////////////////////////////
        // Initialize the scene view's global State
        //
        
        m_GlobalStateSet = new osg::StateSet;
        m_SceneView->setGlobalStateSet(m_GlobalStateSet.get());
        m_GlobalStateSet->setGlobalDefaults();
        
        // enable depth testing by default.
        m_GlobalStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
        // enable lighting by default
        m_GlobalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

        /////////////////////
        // create statesets
        //

        osg::StateSet* lightStateSet = new osg::StateSet;
        lightGroup->setStateSet(lightStateSet);
        osg::LightSource* lightsource = new osg::LightSource;

        osg::Light* light = new osg::Light;
        lightsource->setLight(light);
//@     lightsource->setReferenceFrame(osg::LightSource::RELATIVE_TO_ABSOLUTE);
        lightsource->setStateSetModes(*lightStateSet, osg::StateAttribute::ON);

        lightGroup->addChild(lightsource);

        ///////////////////////////////////
        // create the state set manipulator
        //

        m_StateSetManipulator = new osgGA::StateSetManipulator;
        m_StateSetManipulator->setStateSet(m_GlobalStateSet.get());

        //////////////////////////////
        // create the update visitor
        //

        m_UpdateVisitor = new osgUtil::UpdateVisitor;

        //////////////////////////////
        // create the database pager
        //

        m_DatabasePager = 
            osgDB::Registry::instance()->getOrCreateDatabasePager();

        if (m_DatabasePager != NULL) {
            // set wether the DatabasePager thread should be blocked 
            // while the scene graph is being used to render a frame
            // setting frame block to true can help achieve constant 
            // frame rates on single CPU systems.
            m_DatabasePager->setUseFrameBlock(false);

            // pass the database pager to the cull visitor 
            // so node can send requests to the pager.
            m_SceneView->getCullVisitor()->setDatabaseRequestHandler(
                m_DatabasePager);

            if (m_DatabasePager != NULL) {
                m_DatabasePager->registerPagedLODs(
                    m_SceneView->getSceneData());
            }
        }


        //////////////////////////
        // add camera manipulators
        //

        unsigned int id = 0;

        id = AddCameraManipulator(new osgGA::TrackballManipulator);
        // ensure that actual IDs match our enum
        assert(id == TrackballManipulatorId);

        id = AddCameraManipulator(new osgGA::FlightManipulator);
        // ensure that actual IDs match our enum
        assert(id == FlightManipulatorId);

        id = AddCameraManipulator(new osgGA::DriveManipulator);
        // ensure that actual IDs match our enum
        assert(id == DriveManipulatorId);

        // set first manipulator as first
        m_CurrentManipulatorIndex = 0;
        SetCurrentManipulator(m_CurrentManipulatorIndex);
    }
    return ret;
}
//------------------------------------------------------------------------------
void mafOsgCanvas::Render()
//------------------------------------------------------------------------------
{
  assert(m_SceneView != NULL);
  //@@@ assert(m_DatabasePager != NULL);

  if ((m_SceneView != NULL) /*&& (m_DatabasePager != NULL)*/ ) {

    // set up the frame stamp for current frame to record the 
    // current time and frame number so that animtion code can 
    // advance correctly
    osg::FrameStamp* frameStamp = new osg::FrameStamp;
    frameStamp->setReferenceTime(GetTime());
    frameStamp->setFrameNumber(m_FrameNumber++);

    // pass frame stamp to the SceneView so that the update, 
    // cull and draw traversals all use the same FrameStamp
    m_SceneView->setFrameStamp(frameStamp);

    // update current matrix manipulator
    UpdateCurrentManipulator(frameStamp);

    if ((m_UpdateVisitor != NULL) && (m_UpdateVisitor.valid())) {
      // set update visitor's traversal number 
      m_UpdateVisitor->setTraversalNumber(frameStamp->getFrameNumber());
      // call all node update callbacks and animations.
      m_SceneGraph->accept(*m_UpdateVisitor);
    }

    if(m_DatabasePager) m_DatabasePager->signalBeginFrame(frameStamp);
    // synchronize changes required by the DatabasePager thread 
    // to the scene graph
    if(m_DatabasePager) m_DatabasePager->updateSceneGraph(frameStamp->getReferenceTime());

    // update scene view
    m_SceneView->update();

    // cull scene 
    m_SceneView->cull();

    // SILVANO --- necessario se si usano + finestre
    SetCurrent();

    // draw scene
    m_SceneView->draw();

    // post-draw
    if(m_DatabasePager) m_DatabasePager->signalEndFrame();

    // swap buffers
    SwapBuffers();

    // post-swap
    double availableTime = 0.0025; //  2.5 ms

    // flush deleted GL objects.
    m_SceneView->flushDeletedGLObjects(availableTime);

    // compile any GL objects that are required.
    m_DatabasePager->compileGLObjects(*(m_SceneView->getState()), 
      availableTime);
  }
}



