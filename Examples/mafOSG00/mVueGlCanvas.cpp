/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mVueGlCanvas.cpp,v $
Language:  C++
Date:      $Date: 2006-02-13 15:49:13 $
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

#include "mVueGlCanvas.h"
#include "DragAndDrop.h"
#include "wxWinEventAdapter.h"

//------------------------------------------------------------------------------
// event table for mVueGlCanvas
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mVueGlCanvas, wxGLCanvas)
  EVT_LEFT_DOWN(mVueGlCanvas::OnLeftDown)
  EVT_MIDDLE_DOWN(mVueGlCanvas::OnMiddleDown)
  EVT_RIGHT_DOWN(mVueGlCanvas::OnRightDown)

  EVT_LEFT_UP(mVueGlCanvas::OnLeftUp)
  EVT_MIDDLE_UP(mVueGlCanvas::OnMiddleUp)
  EVT_RIGHT_UP(mVueGlCanvas::OnRightUp)

  EVT_MOTION(mVueGlCanvas::OnMotion)
  EVT_MOUSEWHEEL(mVueGlCanvas::OnMouseWheel)

  EVT_CHAR(mVueGlCanvas::OnChar)
  EVT_KEY_DOWN(mVueGlCanvas::OnKeyDown)
  EVT_KEY_UP(mVueGlCanvas::OnKeyUp)

  EVT_SIZE(mVueGlCanvas::OnSize)
  EVT_PAINT(mVueGlCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(mVueGlCanvas::OnEraseBackground)
  EVT_TIMER(RenderTimer, mVueGlCanvas::OnTimer)

END_EVENT_TABLE()
//------------------------------------------------------------------------------
mVueGlCanvas::mVueGlCanvas(wxWindow *parent, wxWindowID id,const  wxPoint &pos, const wxSize &size, long style)
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
  SetDropTarget(new DragAndDrop(this));

  // create the sceneview
  m_SceneView = new osgUtil::SceneView();
  m_SceneView->setDefaults();

  // the rest of the Initialization is delayed at
  // the first Paint-Event -- need to have an OpenGL Context set.
}
//------------------------------------------------------------------------------
mVueGlCanvas::~mVueGlCanvas(void)
//------------------------------------------------------------------------------
{
    // stop database pager thread
    osgDB::Registry::instance()->setDatabasePager(0);

    if (m_Timer) { delete m_Timer; m_Timer = NULL; }
}


//..............................................................................
void mVueGlCanvas::EventQueue(EventQueueType &queue)
//..............................................................................
{
  queue.clear();
  m_EventQueueMutex.lock();
  m_EventQueue.swap(queue);
  m_EventQueueMutex.unlock();
}
//..............................................................................
void mVueGlCanvas::AddEvent(osg::ref_ptr<wxWinEventAdapter> ea)
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
void mVueGlCanvas::OnLeftDown(wxMouseEvent &event)
//..............................................................................
{
  CaptureMouse();

  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  const int button = LeftMouseButtonId;
  ea->adaptButtonPress(GetTime(), event.GetX(), m_ViewHeight - event.GetY(),button);
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnMiddleDown(wxMouseEvent &event)
//..............................................................................
{
  CaptureMouse();

  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  const int button = MiddleMouseButtonId;
  ea->adaptButtonPress(GetTime(), event.GetX(), m_ViewHeight - event.GetY(),button);
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnRightDown(wxMouseEvent &event)
//..............................................................................
{
  CaptureMouse();

  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  const int button = RightMouseButtonId;
  ea->adaptButtonPress(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnLeftUp(wxMouseEvent &event)
//..............................................................................
{
  if (HasCapture()) ReleaseMouse();

  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  const int button = LeftMouseButtonId;
  ea->adaptButtonRelease(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnMiddleUp(wxMouseEvent &event)
//..............................................................................
{
  if (HasCapture()) ReleaseMouse();

  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  const int button = MiddleMouseButtonId;
  ea->adaptButtonRelease(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnRightUp(wxMouseEvent &event)
//..............................................................................
{
  if (HasCapture()) ReleaseMouse();

  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  const int button = RightMouseButtonId;
  ea->adaptButtonRelease(GetTime(), event.GetX(), m_ViewHeight - event.GetY(), button);
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnMotion(wxMouseEvent &event)
//..............................................................................
{
  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  ea->adaptMouseMotion(GetTime(), event.GetX(), m_ViewHeight - event.GetY());
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnMouseWheel(wxMouseEvent &event)
//..............................................................................
{
  int wheelIncr = event.GetWheelRotation() / event.GetWheelDelta();

  wxWinEventAdapter::ScrollingMotion sm = wxWinEventAdapter::ScrollNone;

  if (wheelIncr > 0) {
    sm = wxWinEventAdapter::ScrollUp;
  } else if (wheelIncr < 0) {
    sm = wxWinEventAdapter::ScrollDown;
  }

  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  ea->adaptMouseScroll(GetTime(), sm);
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::OnChar(wxKeyEvent& event)
//..............................................................................
{
  osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
  ea->adaptKeyPress(GetTime(), wxWinEventAdapter::KeySymbol(event.GetKeyCode()));
  AddEvent(ea);
}
//..............................................................................
void mVueGlCanvas::ResetManipulator()
//..............................................................................
{
    osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
    ea->adaptFrame(0.0f);

    // assert that current manipulator exists
    assert(m_CurrentManipulator != NULL);

    if (m_CurrentManipulator != NULL) {
        // reset manipulator to home position
        m_CurrentManipulator->home(*ea, *this);
    }
}













//------------------------------------------------------------------------------
bool mVueGlCanvas::EnableLighting(bool enable)
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
bool mVueGlCanvas::LightingIsEnabled()
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
void mVueGlCanvas::ClearModel()
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
bool mVueGlCanvas::SaveModel(const char *filename)
//------------------------------------------------------------------------------
{
  bool ret = false;

  if (m_ModelGroup != NULL) 
    ret = osgDB::writeNodeFile(*(osg::Node*)m_ModelGroup.get(), filename);

  return ret;
}
//------------------------------------------------------------------------------
bool mVueGlCanvas::LoadModel(const char *filename)
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
unsigned int mVueGlCanvas::AddCameraManipulator(osgGA::MatrixManipulator *cm)
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
void mVueGlCanvas::requestWarpPointer(float x, float y)
//------------------------------------------------------------------------------
{
  WarpPointer(x, y);
}
//------------------------------------------------------------------------------
bool mVueGlCanvas::SetCurrentManipulator(const unsigned int index) 
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
/** mVueGlCanvas::UpdateCurrentManipulator
description: update current matrix manipulator
param: fs - frame stamp */
void mVueGlCanvas::UpdateCurrentManipulator(osg::FrameStamp *fs)
//------------------------------------------------------------------------------
{
    // get the event since the last frame.
    EventQueueType queue;
    EventQueue(queue);    //mah - SIL

    // create an event to signal the new frame.
    osg::ref_ptr<wxWinEventAdapter> frameEvent = new wxWinEventAdapter;
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
void mVueGlCanvas::OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
{
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);

  // get updated viewport size
  GetClientSize(&m_ViewWidth, &m_ViewHeight);

  if (m_SceneView != NULL) {

    // create event
    osg::ref_ptr<wxWinEventAdapter> ea = new wxWinEventAdapter;
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
void mVueGlCanvas::OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
{
  /* must always be here */
  mVueGlCanvas *T = this;
  
  wxPaintDC dc(this);

  //!!!@@###@!! ---- una variabile locale statica che si comporta come una statica di classe ??
  //static bool isInit = false;

  if (!isInit) 
  {
    if (!GetContext()) 
      return;

    // set current OpenGl context
    SetCurrent();

    // initialize OSG
    InitOsg();

    isInit = true;
  }
}
//------------------------------------------------------------------------------
bool mVueGlCanvas::InitOsg()
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
void mVueGlCanvas::Render()
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
