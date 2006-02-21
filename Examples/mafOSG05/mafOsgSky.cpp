/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgSky.cpp,v $
Language:  C++
Date:      $Date: 2006-02-21 16:14:10 $
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

#include "mafOsgSky.h"

#include "mafOsgTerrainManipulator.h"


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
#include <Producer/Camera>

#include <osg/Texture2D>
#include <osg/Image>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osg/array>
#include <osg/Group>
#include <osg/Notify>
#include <osg/io_utils>
#include <osgUtil/IntersectVisitor>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Depth>
//---------------------------------------
// Update Callback 
//---------------------------------------
class mafOsgSkyCallback : public osg::NodeCallback
{
public:
  mafOsgSkyCallback(mafOsgSky *obj ) {_obj = obj;};

  void operator()(osg::Node* node, osg::NodeVisitor* nv)
  { 
    if(_obj) _obj->Update();
  }
  mafOsgSky *_obj; 
};

//------------------------------------------------------------------------------
mafOsgSky::mafOsgSky( osgProducer::Viewer* viewer, mafOsgTerrainManipulator *manip)
//------------------------------------------------------------------------------
{
   assert(viewer);
   m_viewer = viewer;

   m_blend = false;
   m_radius = 10000;
   m_texfile = "D:\\__OSG_DATA\\sky2k_2.jpg";
   
   double tmp[] = {-10, -5, 0, 5, 10, 20, 40, 60, 90 };
   for(int i=0; i<9; i++)
   {
      m_pitchTable[i] = tmp[i];
      m_colorTable[i] = wxColour( 255-i*255/9.0, 255-i*255/9.0, 255 );
   }

   Create();
   SetTexture(m_texfile);
}
//------------------------------------------------------------------------------
mafOsgSky::~mafOsgSky(void)
//------------------------------------------------------------------------------
{
  osg::Group *root = (osg::Group*)m_viewer->getSceneData();
  root->removeChild(m_sky.get());

}
//------------------------------------------------------------------------------
void mafOsgSky::Setup( double radius, double pitchTable[9], wxColour colorTable[9] )
//------------------------------------------------------------------------------
{
    m_radius = radius;
    for(int i=0; i<9; i++) m_pitchTable[i] = pitchTable[i];
    for(int i=0; i<9; i++) m_colorTable[i] = colorTable[i];
    Create();
}
//------------------------------------------------------------------------------
void mafOsgSky::SetTexture( std::string filename )
//------------------------------------------------------------------------------
{
  m_texfile = filename;
  // sky was already created by definition
  assert(m_ss.get());

  osg::ref_ptr<osg::Image> img = osgDB::readImageFile(m_texfile);
  if(img.get() == NULL) return;

  osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D();
  tex->setWrap(osg::Texture2D::WrapParameter::WRAP_S,osg::Texture2D::WrapMode::CLAMP_TO_EDGE ); 
  tex->setWrap(osg::Texture2D::WrapParameter::WRAP_T,osg::Texture2D::WrapMode::CLAMP_TO_EDGE ); 
  tex->setImage(img.get());

  m_ss->setTextureAttributeAndModes(0,tex.get());
}
//------------------------------------------------------------------------------
void mafOsgSky::Update()
//------------------------------------------------------------------------------
{
  osg::Matrixd vm = m_viewer->getViewMatrix(); 
  osg::Vec3 pos( vm.getTrans() );

  if(m_sky.get()) 
    m_sky->setMatrix(osg::Matrixd::translate(pos)); // sposto la palla sopra la camera

  
  //double left,right,bottom,top,near,far,shearx,sheary;

  //m_viewer->viewer.getCamera(0)->getLensParams(left,right,bottom,top,near,far,shearx,sheary);


}
//------------------------------------------------------------------------------
void mafOsgSky::Create()
//------------------------------------------------------------------------------
{
  osg::ref_ptr<osg::Geometry>         geom = new osg::Geometry();
  osg::ref_ptr<osg::Vec3Array>        vert = new osg::Vec3Array;
  osg::ref_ptr<osg::Vec3Array>        norm = new osg::Vec3Array;
  osg::ref_ptr<osg::Vec2Array>        texcoords = new osg::Vec2Array();
  osg::ref_ptr<osg::DrawElementsUInt> cell = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
  osg::ref_ptr<osg::Vec4Array>        colors= new osg::Vec4Array();

  int resP = 8; 
  int resY = 64; 

  double p0 = (osg::PI * m_pitchTable[0])    / 180 ;
  double p1 = (osg::PI * m_pitchTable[resP]) / 180 ;

  for(int yaw = 0; yaw<=resY; yaw++) 
  {
    for(int pitch = 0; pitch<=resP; pitch++) 
    {
      double y = 2 * (osg::PI * yaw) /resY ;
      double p =     (osg::PI * m_pitchTable[pitch]) / 180 ;

      vert->push_back( osg::Vec3( m_radius*cos(y)*cos(p), m_radius*sin(y)*cos(p), m_radius*sin(p) )); 
      norm->push_back( osg::Vec3( -cos(y)*cos(p), -sin(y)*cos(p), -sin(p) )); 
      texcoords->push_back( osg::Vec2( (yaw*1.0)/resY, (p-p0)/(p1-p0) ) ); 
      colors->push_back( osg::Vec4(m_colorTable[pitch].Red()   / 255.0, 
                                   m_colorTable[pitch].Green() / 255.0, 
                                   m_colorTable[pitch].Blue()  / 255.0, 1.0 ) ); 

      if(pitch < resP && yaw < resY )
      {
        cell->push_back(  yaw   *(resP+1) + pitch);
        cell->push_back(  yaw   *(resP+1) + pitch + 1);
        cell->push_back( (yaw+1)*(resP+1) + pitch +1 );
        cell->push_back( (yaw+1)*(resP+1) + pitch    );
      }
    }
  }

  geom->addPrimitiveSet(cell.get()); 
  geom->setVertexArray( vert.get() ); 
  geom->setNormalArray( norm.get() ); 
  geom->setNormalBinding( osg::Geometry::AttributeBinding::BIND_PER_VERTEX );
  geom->setTexCoordArray(0,texcoords.get());
  geom->setColorArray(colors.get());
  geom->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

  osg::ref_ptr<osg::Depth> depthFunction = new osg::Depth(osg::Depth::Function::ALWAYS);
  depthFunction->setWriteMask(false);

  m_ss = new osg::StateSet();
  m_ss->setMode(GL_LIGHTING,  osg::StateAttribute::OFF);
  m_ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON); 
  m_ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  m_ss->setAttribute(depthFunction.get());
  m_ss->setRenderBinDetails(-1,"RenderBin"); //draw before the rest

  //SetTexture( m_texfile); //m_ss must be set;

  osg::ref_ptr<osg::Geode> skyGeode = new osg::Geode();
  skyGeode->addDrawable(geom.get());
  skyGeode->setStateSet(m_ss.get());

  m_sky  = new osg::MatrixTransform();
  m_sky->addChild(skyGeode.get());

  osg::Group *root = (osg::Group*)m_viewer->getSceneData();
  root->addChild(m_sky.get());

  m_cb = new mafOsgSkyCallback(this);
  m_sky->setUpdateCallback(m_cb.get());
}
