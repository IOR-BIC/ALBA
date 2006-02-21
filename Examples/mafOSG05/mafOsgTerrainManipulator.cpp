/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgTerrainManipulator.cpp,v $
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

#include <fstream>

//#include <osgGA/mafOsgTerrainManipulator>
#include "mafOsgTerrainManipulator.h"

#include <osg/Quat>
#include <osg/Node>
#include <osg/Group>
#include <osg/Notify>
#include <osg/io_utils>
#include <osgUtil/IntersectVisitor>

#include <osg/ShapeDrawable>
#include <osg/Geode>

using namespace osg;
using namespace osgGA;

//--------------------------------------------------------------------
mafOsgTerrainManipulator::mafOsgTerrainManipulator()  
//--------------------------------------------------------------------
{
    _thrown           = false;
    _flying           = false;
    _request_fly      = false;
    _needIntersect    = false;
    _intersectionfound= false;

    _begin_fly_time   = 0;
    _flying_duration  = 5;

    createHud();
    createGizmo();

}
//--------------------------------------------------------------------
mafOsgTerrainManipulator::~mafOsgTerrainManipulator()
//--------------------------------------------------------------------
{
  showGizmo(false);
  showHud(false);
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::init(const GUIEventAdapter& ,GUIActionAdapter& )
//--------------------------------------------------------------------
{
  flushMouseEventStack();
}
//--------------------------------------------------------------------
osg::Matrixd mafOsgTerrainManipulator::getMatrix() const
//--------------------------------------------------------------------
{
  return _camera._matrix;
}
//--------------------------------------------------------------------
osg::Matrixd mafOsgTerrainManipulator::getInverseMatrix() const
//--------------------------------------------------------------------
{
  return _camera._inversematrix;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::addMouseEvent(const GUIEventAdapter& ea)
//--------------------------------------------------------------------
{
  _ga_t1 = _ga_t0;
  _ga_t0 = &ea;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::flushMouseEventStack()
//--------------------------------------------------------------------
{
  _ga_t1 = NULL;
  _ga_t0 = NULL;
}
//--------------------------------------------------------------------
bool mafOsgTerrainManipulator::isMouseMoving()
//--------------------------------------------------------------------
{
  if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

  static const float velocity = 0.1f;

  float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
  float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
  float len = sqrtf(dx*dx+dy*dy);
  float dt = _ga_t0->time()-_ga_t1->time();

  return (len>dt*velocity);
}





//--------------------------------------------------------------------
void mafOsgTerrainManipulator::setNode(osg::Node* node)
//--------------------------------------------------------------------
{
    _terrain = node;

    if (_terrain.get())
    {
        const osg::BoundingSphere& bs=_terrain->getBound();
        _minimumDistance = (bs._radius * 0.0001f);
        //_minimumDistance = osg::clampBetween(_minimumDistance, 0.00001f, 1.0f);
        _gizmoShape->setRadius(bs._radius * 0.001f);

    }
    if (getAutoComputeHomePosition()) computeHomePosition();    
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::home(const GUIEventAdapter& ,GUIActionAdapter& us)
//--------------------------------------------------------------------
{
    if (getAutoComputeHomePosition()) computeHomePosition();
    computePosition(_homeEye, _homeCenter, _homeUp);
    us.requestRedraw();
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::computePosition(const osg::Vec3d& eye,const osg::Vec3d& center,const osg::Vec3d& up)
//--------------------------------------------------------------------
{
  // ---- eye/center/up non li uso
  if(_terrain.get())
  {
    const osg::BoundingSphere& bs=_terrain->getBound();
    _camera._center = bs._center;
    _camera._distance = bs._radius /2;
  }
  else
  {
    _camera._center = osg::Vec3d(0.0, 0.0, 0.0);
    _camera._distance = 10;
  }

  _camera._pitch =75;
  _camera._yaw =0;
  _camera.UpdateRotation();
  IntersectTerrain();
}
//--------------------------------------------------------------------
bool mafOsgTerrainManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
//--------------------------------------------------------------------
{
    switch(ea.getEventType())
    {
        case(GUIEventAdapter::PUSH):
        {
            flushMouseEventStack();
            addMouseEvent(ea);
            if (calcMovement()) us.requestRedraw();
            us.requestContinuousUpdate(false);
            _thrown = false;
            _flying = false; // any user action during a flight will interrupt the flight
            return true;
        }
        break;

        case(GUIEventAdapter::RELEASE):
        {
            if(_needIntersect)
                IntersectTerrain();

            if (ea.getButtonMask()==0)
            {
                if (isMouseMoving())
                {
                    if (calcMovement())
                    {
                        us.requestRedraw();
                        us.requestContinuousUpdate(true);
                        _thrown = true;
                    }
                }
                else
                {
                    flushMouseEventStack();
                    addMouseEvent(ea);
                    if (calcMovement()) us.requestRedraw();
                    us.requestContinuousUpdate(false);
                    _thrown = false;
                }

            }
            else
            {
                flushMouseEventStack();
                addMouseEvent(ea);
                if (calcMovement()) us.requestRedraw();
                us.requestContinuousUpdate(false);
                _thrown = false;
            }
            return true;
        }
        break;

        case(GUIEventAdapter::DRAG):
        {
            addMouseEvent(ea);
            if (calcMovement()) us.requestRedraw();
            us.requestContinuousUpdate(false);
            _thrown = false;
            return true;
        }
        break;

        case(GUIEventAdapter::MOVE):
        {
            return false;
        }
        break;

        case(GUIEventAdapter::KEYDOWN):
            // SPACE = reset camera to home position
            if (ea.getKey()==' ')
            {
                flushMouseEventStack();
                _thrown = false;
                home(ea,us);

                us.requestRedraw();
                us.requestContinuousUpdate(false);
                return true;
            }
            //F1 = Store Flight Position
            if ( ea.getKey()==65470 && !_flying )
            {
              _f2 = _camera;
              return true;  
            }

            //F2 = FlyTo stored Position
            if ( ea.getKey()==65471 && !_flying )
            {
              _request_fly = true; // flight will begin at the next frame
              return true;  
            }
            return false;
        break;

        case(GUIEventAdapter::FRAME):

          if(_gizmo.get() != NULL )
            _gizmo->setMatrix( osg::Matrixd::translate(_camera._center));

          if(_text.get())
          {
            char s[500];
            sprintf(s,"pos= (%.0f %.0f %.0f)%s yaw=%.0f pitch=%.0f dist=%.0f ",
              _camera._center.x(), 
              _camera._center.y(), 
              _camera._center.z(), 
              (_intersectionfound)? "+" : "-",
              _camera._yaw,
              _camera._pitch,
              _camera._distance);
            _text->setText(s);
          }

          if( _request_fly )
          {
              _request_fly = false;
              _flying = true;
              _thrown = false;
              _f1 = _camera;
              _begin_fly_time = ea.time();

              // avoid yaw rotation > than 180
              if( (_f2._yaw - _f1._yaw) < -180) 
              _f2._yaw += 360;
              if( (_f2._yaw - _f1._yaw) > 180)  
              _f2._yaw -= 360;
              _f2.UpdateRotation();
          }

          if (_flying)
          {
            double dt = (ea.time() - _begin_fly_time) / _flying_duration;
            double t = 0.5 + 0.5 * sin( dt * osg::PI - osg::PI_2 ); // accelerate/decelerate
            _camera.Interpolate(_f1,_f2,t);
            //_camera.InterpolateUsingQuaternion(_f1,_f2,t);  //c'e' un errore - verificare
            
            if( ea.time() > _begin_fly_time + _flying_duration )
            {
              IntersectTerrain();
              _flying = false;
            }
            us.requestRedraw();
          }
          
          if (_thrown)
          {
            if (calcMovement()) us.requestRedraw();
          }
          return false;
          break;

        default:
          return false;
    }
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::setByMatrix(const osg::Matrixd& matrix)
//--------------------------------------------------------------------
{
    osg::Vec3 lookVector(- matrix(2,0),-matrix(2,1),-matrix(2,2));
    osg::Vec3 eye(matrix(3,0),matrix(3,1),matrix(3,2));
    osg::notify(INFO)<<"eye point "<<eye<<std::endl;
    osg::notify(INFO)<<"lookVector "<<lookVector<<std::endl;

    _camera._center = eye+ lookVector;
    _camera._distance = lookVector.length();
    Camera::RotToYawPitch(matrix,_camera._yaw,_camera._pitch);
    _camera._pitch = clampBetween( _camera._pitch, 2.0, 80.0 );
    _camera.UpdateRotation();

    IntersectTerrain();
}
//--------------------------------------------------------------------
bool mafOsgTerrainManipulator::calcMovement()
//--------------------------------------------------------------------
{
  // return if less then two events have been added.
  if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

  double dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
  double dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();

  // return if there is no movement.
  if (dx==0 && dy==0) return false;

  unsigned int buttonMask = _ga_t1->getButtonMask();
  if (buttonMask==GUIEventAdapter::LEFT_MOUSE_BUTTON)
  {
    rotate(dx,dy);
    return true;
  }
  else if (buttonMask==GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
    buttonMask==(GUIEventAdapter::LEFT_MOUSE_BUTTON|GUIEventAdapter::RIGHT_MOUSE_BUTTON))
  {
    pan(dx,dy);
    return true;
  }
  else if (buttonMask==GUIEventAdapter::RIGHT_MOUSE_BUTTON)
  {
    zoom(dx,dy);
    return true;
  }
  return false;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::rotate(double dx, double dy)
//--------------------------------------------------------------------
{
  _camera._yaw   -= 30*dx;
  _camera._pitch += 20*dy;

  while(_camera._yaw < 0 )   _camera._yaw += 360;
  while(_camera._yaw > 360)  _camera._yaw -= 360;
  if ( _camera._pitch < 2  ) _camera._pitch =2;
  if ( _camera._pitch > 180 ) _camera._pitch = 180;
  _camera.UpdateRotation();

  _needIntersect = true;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::pan(double dx, double dy)
//--------------------------------------------------------------------
{
  // the magnitude of the effect depends on distance

  float scale = -0.3f * _camera._distance;

  osg::Matrix rotation_matrix;
  rotation_matrix.set(_camera._rotation);
  osg::Vec3 dv(dx*scale,dy*scale,0.0f);
  _camera._center += dv*rotation_matrix;

  _camera.UpdateMatrix();
  _needIntersect = true;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::zoom(double dx, double dy)
//--------------------------------------------------------------------
{
  _camera._distance *= (1.0f + dy);
  if( _camera._distance < _minimumDistance ) _camera._distance = _minimumDistance;
  
  _camera.UpdateMatrix();
  _needIntersect = true;
}
//--------------------------------------------------------------------
// verifica che il center sia sul terreno
// eventualmente lo sposta e corregge la distance
void mafOsgTerrainManipulator::IntersectTerrain()
//--------------------------------------------------------------------
{
   _needIntersect = false;
   
   if(!_terrain.get()) return;

   //devo creare un segmento - orientato come il lookAt
   // che parte da eye e arriva a ???
   osg::Matrix rotation_matrix(_camera._rotation);
   osg::Vec3d lookVector = -getUpVector(rotation_matrix);
   lookVector.normalize();
   double d = 2 * _terrain->getBound().radius();
   osg::Vec3d eye= _camera._center - lookVector * _camera._distance;
   osg::Vec3d p1 = _camera._center - lookVector * d;
   osg::Vec3d p2 = _camera._center + lookVector * d;
   osg::Vec3d new_center;

   _intersectionfound = Intersect( p1, p2, new_center );

   // sposto il centro e aggiorno la 
   // distance per mantenera la posizione di Eye
   if(_intersectionfound)
   {
     osg::Vec3d delta = eye - new_center;
     _camera._distance = delta.length();
     if( _camera._distance < _minimumDistance ) _camera._distance = _minimumDistance;
     _camera._center = new_center;
   }
}
//--------------------------------------------------------------------
bool mafOsgTerrainManipulator::Intersect( osg::Vec3d p1, osg::Vec3d p2, osg::Vec3d& result)
//--------------------------------------------------------------------
{
  bool found = false;

  osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment;
  seg->set(p1,p2);

  osgUtil::IntersectVisitor iv;
  iv.setTraversalMode(osgUtil::IntersectVisitor::TRAVERSE_ACTIVE_CHILDREN); //SIL Only visible children
  iv.addLineSegment(seg.get());
  _terrain->accept(iv);
  if (iv.hits())
  {
    osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
    if (!hitList.empty())
    {
      for(osgUtil::IntersectVisitor::HitList::iterator hitr=hitList.begin();  hitr!=hitList.end();  ++hitr )
      {
        if( hitr->_geode.valid() )
        {
          if( hitr->_geode.get() != _gizmoGeode.get() )
          {
            osg::Vec3 p= hitr->getWorldIntersectPoint();
            if(!found) 
              result = p;
            else
              if(result[2]<p[2]) // cosi ritorno l'intersezione con la z maggiore - dovrei ritornare la piu vicina
                result = p;
            found=true; 
          }
        }
      }
    }
  }
  return found;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::FlyToXY(double x, double y)
//--------------------------------------------------------------------
{
  if( !_terrain.get() ) return;
  double     r = _terrain->getBound().radius();
  osg::Vec3d c = _terrain->getBound().center();
  osg::Vec3d p1(x, y, c[2] - r );
  osg::Vec3d p2(x, y, c[2] + r );
  osg::Vec3d p;
  bool found = Intersect( p1, p2, p );
  if(!found) 
  {
     osg::notify(WARN)<<"FlyToXY: failed to find Z" <<std::endl;
     return;
  }
 
  // calc new yaw 
  osg::Vec3d delta = p - _camera._center;
  delta[2]=0;
  delta.normalize();
  double yaw  = atan2(  delta[1], delta[0] ) -osg::PI/2;  

  _f2._center    = p;
  _f2._distance  = _camera._distance;
  _f2._pitch     = _camera._pitch;
  _f2._yaw       = yaw;
  _f2.UpdateRotation();

  _request_fly = true;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::FlyToXY_YPD(double x,double y,double yaw,double pitch, double dist)
//--------------------------------------------------------------------
{
  if( !_terrain.get() ) return;

  double     r = _terrain->getBound().radius();
  osg::Vec3d c = _terrain->getBound().center();
  osg::Vec3d p1(x, y, c[2] - r );
  osg::Vec3d p2(x, y, c[2] + r );
  osg::Vec3d p;
  bool found = Intersect( p1, p2, p );
  if(!found) 
  {
    osg::notify(WARN)<<"FlyToXY: failed to find Z" <<std::endl;
    return;
  }

  _f2._center    = p;
  _f2._distance  = dist;
  _f2._pitch     = pitch;
  _f2._yaw       = yaw;
  _f2.UpdateRotation();

  _request_fly = true;
}
//---------------------------------------
void mafOsgTerrainManipulator::setGizmoParent(osg::Group *gizmoParent)
//---------------------------------------
{
  if( gizmoParent == _gizmoParent.get() ) return;

  bool showG = isGizmoShown();
  bool showH = isHudShown();
  if ( showG ) showGizmo(false);
  if ( showH ) showHud(false);

  _gizmoParent = gizmoParent;

  if( _gizmoParent.get() )
  {
    if ( showG ) showGizmo(true);
    if ( showH ) showHud(true);
  }
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::showHud(bool show)
//--------------------------------------------------------------------
{
  assert(_gizmoParent.get() != NULL); // please call SetGizmoParent before

  if(show)
    _gizmoParent->addChild(_hud.get());   
  else
    _gizmoParent->removeChild(_hud.get());   
}
//---------------------------------------
void mafOsgTerrainManipulator::showGizmo(bool show)
//---------------------------------------
{
  assert(_gizmoParent.get() != NULL); // please call SetGizmoParent before

  if(show)
    _gizmoParent->addChild(_gizmo.get());   
  else
    _gizmoParent->removeChild(_gizmo.get());   
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::createHud()
//--------------------------------------------------------------------
{
  osg::ref_ptr<osg::Geode> geode = new osg::Geode();

  std::string timesFont("fonts/arial.ttf");

  // turn lighting off for the text and disable depth test to ensure its always ontop.
  osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
  stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

  osg::Vec3 position(10.0f,10.0f,0.0f);

  _text = new  osgText::Text;
  geode->addDrawable( _text.get() );

  _text->setFont(timesFont);
  _text->setPosition(position);
  _text->setText("Hud");
  _text->setCharacterSize(20);

  osg::ref_ptr<osg::CameraNode> camera = new osg::CameraNode;

  // set the projection matrix
  camera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280,0,1024));

  // set the view matrix    
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setViewMatrix(osg::Matrix::identity());

  // only clear the depth buffer
  camera->setClearMask(GL_DEPTH_BUFFER_BIT);

  // draw subgraph after main camera view.
  camera->setRenderOrder(osg::CameraNode::POST_RENDER);

  camera->addChild(geode.get());

  _hud = camera.get();
}
//---------------------------------------
void mafOsgTerrainManipulator::createGizmo()
//---------------------------------------
{
  float sz = 0.5;
  if(_terrain.get()) sz = _terrain->getBound()._radius * 0.001f;

  _gizmoShape = new osg::Sphere(osg::Vec3d(0,0,0),sz);
  osg::ref_ptr<osg::ShapeDrawable> gizmoDrawable = new osg::ShapeDrawable(_gizmoShape.get());
  _gizmoGeode = new osg::Geode();
  _gizmoGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
  _gizmoGeode->addDrawable(gizmoDrawable.get());
  _gizmo = new osg::MatrixTransform();
  _gizmo->addChild(_gizmoGeode.get());
}

