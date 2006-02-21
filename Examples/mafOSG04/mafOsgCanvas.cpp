/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgCanvas.cpp,v $
Language:  C++
Date:      $Date: 2006-02-21 16:12:39 $
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

#if !wxUSE_DRAG_AND_DROP
#error This application requires drag and drop support in the library
#endif
#include <wx/dnd.h>

#include <osg/GL>
#include <osg/GLU>
#include <osg/Fog>
#include <osg/Light>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Node>
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


#include <osg/Texture2D>
#include <osg/Image>
#include <osg/StateSet>
#include <osg/TexEnv>
#include <osg/AlphaFunc>
#include <osg/TexEnvCombine>
#include <osg/BlendFunc>

#include <osgDB/ReadFile>


#include <osg/array>

//------------------------------------------------------------------------------
// const
//------------------------------------------------------------------------------
#define TimerID 1000


//-----------------------------------------------------------------------
// class mafOsgCanvasDropTarget
//-----------------------------------------------------------------------
/** description: handle drag and drop operations.  Requires knowledge of 
    mafOsgCanvas class.  On drop, will call mafOsgCanvas::LoadModel */
class mafOsgCanvasDropTarget : public wxFileDropTarget
{
public:
    mafOsgCanvasDropTarget(mafOsgCanvas *canvas) { m_Canvas = canvas; };
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
       if (filenames.GetCount() != 1 ) return false;
       if (m_Canvas == NULL ) return false;
       return m_Canvas->LoadModel(filenames[0].c_str());
    };
    mafOsgCanvas *m_Canvas;
};

//------------------------------------------------------------------------------
// event table for mafOsgCanvas
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafOsgCanvas, wxWindow)
  EVT_SIZE(mafOsgCanvas::OnSize)
  EVT_PAINT(mafOsgCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(mafOsgCanvas::OnEraseBackground)
  EVT_TIMER(TimerID, mafOsgCanvas::OnTimer)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
mafOsgCanvas::mafOsgCanvas(wxWindow *parent, wxWindowID id,const  wxPoint &pos, const wxSize &size, long style)
//------------------------------------------------------------------------------
:   wxWindow(parent, id, pos, size, style)
{
   // setup periodic timer for rendering
    m_Timer = new wxTimer(this, TimerID);
    m_Timer->Start(5);

    // enable drag and drop of model files
    SetDropTarget(new mafOsgCanvasDropTarget(this));

    m_Viewer=new osgProducer::Viewer;

    m_Viewer->setUpViewer(
      osgProducer::Viewer::TRACKBALL_MANIPULATOR|
      //osgProducer::Viewer::DRIVE_MANIPULATOR |
      //osgProducer::Viewer::FLIGHT_MANIPULATOR |
      //osgProducer::Viewer::TERRAIN_MANIPULATOR |
      //osgProducer::Viewer::UFO_MANIPULATOR |
      osgProducer::Viewer::STATE_MANIPULATOR |
      osgProducer::Viewer::HEAD_LIGHT_SOURCE |
      osgProducer::Viewer::STATS_MANIPULATOR |  //performance indicator & fullscreen
      osgProducer::Viewer::VIEWER_MANIPULATOR 
      //osgProducer::Viewer::ESCAPE_SETS_DONE
      );

    osg::ref_ptr<Producer::RenderSurface> rs; // connect the RenderSurface to the OCX
    rs = m_Viewer->getCameraConfig()->getCamera(0)->getRenderSurface();
    rs->setWindow((HWND)GetHandle());

    m_Root = new osg::Group;
    m_Viewer->setSceneData(m_Root.get());

    m_Viewer->realize(Producer::CameraGroup::SingleThreaded); // Realize Must Follow SetSceneData
}
//------------------------------------------------------------------------------
mafOsgCanvas::~mafOsgCanvas(void)
//------------------------------------------------------------------------------
{
    if (m_Timer) { delete m_Timer; m_Timer = NULL; }

/*
    if(!m_Viewer) return;

    // get and Clear the Pager
    osgDB::DatabasePager* databasePager = osgDB::Registry::instance()->getOrCreateDatabasePager();
    if(databasePager)
      databasePager->clear();

    // get the SceneView
    Producer::Camera *cam = m_Viewer->getCameraConfig()->getCamera(0);
    if(cam)
    {
      osgProducer::OsgSceneHandler *osh = (osgProducer::OsgSceneHandler*)(cam->getSceneHandler());
      if(osh)
      {
        osgUtil::SceneView *sw = osh->getSceneView();
        if(sw)
        {
          // release the GL objects stored in the scene graph.
          sw->releaseAllGLObjects();
          // do a flush to delete all the OpenGL objects that have been deleted or released from the scene graph.
          sw->flushAllDeletedGLObjects();
          // reset the osg::State so that next time its used its in a cleaned state.
          sw->getState()->reset();
        }
      }
    }
  */
  m_Viewer->sync(); // wait all treads to close ??
  m_Viewer->setDone(true);

}
//------------------------------------------------------------------------------
bool mafOsgCanvas::SaveModel(const char *filename)
//------------------------------------------------------------------------------
{
  return osgDB::writeNodeFile(*m_Root.get(), filename);
}
//------------------------------------------------------------------------------
bool mafOsgCanvas::LoadModel(const char *filename)
//------------------------------------------------------------------------------
{
  osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);
  if( loadedModel.get() == NULL ) return false;

  osgUtil::Optimizer optimizer;
  optimizer.optimize(loadedModel.get());

  while (m_Root->getNumChildren() > 0) 
    m_Root->removeChild(m_Root->getChild(0));

  m_Root->addChild(loadedModel.get());

  Home();

  return true;
}
//------------------------------------------------------------------------------
void mafOsgCanvas::OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void mafOsgCanvas::OnTimer(wxTimerEvent &event)
//------------------------------------------------------------------------------
{
  m_Viewer->sync();
  m_Viewer->update();
  m_Viewer->frame();
}
//------------------------------------------------------------------------------
void mafOsgCanvas::Home()
//------------------------------------------------------------------------------
{
  // Reset Camera Position
  osgGA::KeySwitchMatrixManipulator *ks = m_Viewer->getKeySwitchMatrixManipulator();
  assert(ks);
  osgGA::MatrixManipulator *mm = ks->getCurrentMatrixManipulator();
  assert(mm);
  mm->setNode(m_Root.get());
  mm->home(0);

  // altro modo di richiedere Home -- cosi ferma eventuali rotazioni in atto
  osgProducer::EventAdapter ea; 
  ea._eventType = osgGA::GUIEventAdapter::KEYDOWN;
  ea._key = osgGA::GUIEventAdapter::KEY_Space;
  mm->handle(ea,*m_Viewer);
}
//------------------------------------------------------------------------------
osg::Group* mafOsgCanvas::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root.get();
}
//------------------------------------------------------------------------------
osgProducer::Viewer* mafOsgCanvas::GetViewer
()
//------------------------------------------------------------------------------
{
  return m_Viewer.get();
}
