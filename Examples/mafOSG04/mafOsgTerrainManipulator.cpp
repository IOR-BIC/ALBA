/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgTerrainManipulator.cpp,v $
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

#include <fstream>

//#include <osgGA/mafOsgTerrainManipulator>
#include "mafOsgTerrainManipulator.h"

#include <osg/Quat>
#include <osg/Node>
#include <osg/Group>
#include <osg/Notify>
#include <osg/io_utils>
#include <osgUtil/IntersectVisitor>

//#include <osg/ShapeDrawable>
#include <osg/ShapeDrawable>
#include <osg/Geode>

using namespace osg;
using namespace osgGA;

//--------------------------------------------------------------------
mafOsgTerrainManipulator::mafOsgTerrainManipulator()  
//--------------------------------------------------------------------
{
    _distance = 1.0;
    _thrown = false;

    _f1.valid = false;
    _f2.valid = false;
    _flying = false;
    _request_fly = false;
    _begin_fly_time =0;
    _flying_duration = 5;

    _yaw =0;
    _pitch =0;
    _needIntersect = false;
    _hitfound = false;
}
//--------------------------------------------------------------------
mafOsgTerrainManipulator::~mafOsgTerrainManipulator()
//--------------------------------------------------------------------
{
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
  return osg::Matrixd::translate(0.0,0.0,_distance)*osg::Matrixd::rotate(_rotation)*osg::Matrix::translate(_center);
}
//--------------------------------------------------------------------
osg::Matrixd mafOsgTerrainManipulator::getInverseMatrix() const
//--------------------------------------------------------------------
{
  return osg::Matrix::translate(-_center)*osg::Matrixd::rotate(_rotation.inverse())*osg::Matrixd::translate(0.0,0.0,-_distance);
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
    if(node == _NODE.get()) return;

    _NODE = node;

    if (_NODE.get())
    {
        const osg::BoundingSphere& bs=_NODE->getBound();
        const float minimumDistanceScale = 0.0001f;
        _minimumDistance = osg::clampBetween(
            bs._radius * minimumDistanceScale,
            0.00001f,
            1.0f);
            
        osg::notify(osg::INFO)<<"Setting terrain manipulator _minimumDistance to "<<_minimumDistance<<std::endl;
    }
    if (getAutoComputeHomePosition()) computeHomePosition();    
}
/*
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::computeHomePosition()
//--------------------------------------------------------------------
{
  if(getNode())
  {
    const osg::BoundingSphere& bs=getNode()->getBound();
    osg::Vec3d c = bs._center;
    double     r = bs._radius;

    setHomePosition(
      c +osg::Vec3( 0.0,  r/2  ,0.0f),  //eye  --- cambiata la distance
      c,                                //center
      osg::Vec3(0.0f,0.0f,1.0f),        //up
      _autoComputeHomePosition);
  }
}
*///--------------------------------------------------------------------
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
  
  if(_NODE.get())
  {
    const osg::BoundingSphere& bs=_NODE->getBound();
    _center = bs._center;
    _distance = bs._radius /2;
  }
  else
  {
    _center = osg::Vec3d(0.0, 0.0, 0.0);
    _distance = 10;
  }

  _pitch =75;
  _yaw =0;
  YawPitchToRot(_yaw, _pitch, _rotation);

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
            _flying = false; //
            return true;
        }

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

        case(GUIEventAdapter::DRAG):
        {
            addMouseEvent(ea);
            if (calcMovement()) us.requestRedraw();
            us.requestContinuousUpdate(false);
            _thrown = false;
            return true;
        }

        case(GUIEventAdapter::MOVE):
        {
            return false;
        }

        case(GUIEventAdapter::KEYDOWN):
            
            //osg::notify(WARN)<<" key: " << ea.getKey() <<std::endl;

            if (ea.getKey()==' ')
            {
                flushMouseEventStack();
                _thrown = false;
                home(ea,us);

                us.requestRedraw();
                us.requestContinuousUpdate(false);
                return true;
            }
            //F1 ---------------------------------------------
            // Store Position
            if ( ea.getKey()==65470 && !_flying )
            {
              _f2.valid     = true;
              _f2.center    = _center;
              _f2.rotation  = _rotation;
              _f2.yaw  = _yaw;
              _f2.pitch  = _pitch;
              _f2.distance  = _distance;
              return true;  
            }

            //F2 ---------------------------------------------
            // FlyTo Position
            if ( ea.getKey()==65471 && !_flying )
            {
              if (_f2.valid ) 
              {
                _f1.center    = _center;
                _f1.rotation  = _rotation;
                _f1.distance  = _distance;
                _f1.yaw  = _yaw;
                _f1.pitch  = _pitch;

                _flying = true;
                _begin_fly_time = ea.time();
                _thrown = false;
              }
              return true;  
            }
            return false;

        case(GUIEventAdapter::FRAME):

          moveGizmo();

          if(_text.get())
          {
            char s[500];
            sprintf(s,"pos= (%.0f %.0f %.0f)%s yaw=%.0f pitch=%.0f dist=%.0f ",
              _center.x(), 
              _center.y(), 
              _center.z(), 
              (_hitfound)? "+" : "-",
              _yaw,
              _pitch,
              _distance);
            _text->setText(s);
          }


          if( _request_fly )
          {
            if (_f2.valid ) 
            {
              _f1.center    = _center;
              _f1.rotation  = _rotation;
              _f1.yaw       = _yaw;
              _f1.pitch     = _pitch;
              _f1.distance  = _distance;
              _flying = true;
              _begin_fly_time = ea.time();
              _thrown = false;

              // avoid yaw rotation > than 180
              if( (_f2.yaw - _f1.yaw) < -180) 
              _f2.yaw += 360;
              if( (_f2.yaw - _f1.yaw) > 180)  
              _f2.yaw -= 360;

            }
            _request_fly = false;
            return true;  
          }

          if (_flying)
          {
            double dt = (ea.time() - _begin_fly_time) / _flying_duration;
            double t = 0.5 + 0.5 * sin( dt * osg::PI - osg::PI_2 );

            _center   = _f1.center   *(1-t) + _f2.center   * t;
            //_rotation = _f1.rotation *(1-t) + _f2.rotation * t;
            _distance = _f1.distance *(1-t) + _f2.distance * t;
            _yaw   = _f1.yaw *(1-t) + _f2.yaw * t;
            _pitch = _f1.pitch *(1-t) + _f2.pitch * t;
            
            YawPitchToRot(_yaw,_pitch,_rotation); // x aggiornare l'hud - forse e' pesante
            //RotToYawPitch(osg::Matrixd::rotate(_rotation),_yaw,_pitch); // x aggiornare l'hud - forse e' pesante

            if( ea.time() > _begin_fly_time + _flying_duration )
            {
              //RotToYawPitch(osg::Matrixd::rotate(_rotation),_yaw,_pitch);
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

    _center = eye+ lookVector;
    _distance = lookVector.length();
    RotToYawPitch(matrix,_yaw,_pitch);
    _pitch = clampBetween( _pitch, 2.0, 80.0 );
    YawPitchToRot(_yaw,_pitch,_rotation);

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
  _yaw   -= 30*dx;
  _pitch += 20*dy;

  while(_yaw < 0 ) _yaw += 360;
  while(_yaw > 360) _yaw -= 360;
  if ( _pitch < 2  ) _pitch =2;
  if ( _pitch > 80 ) _pitch = 80;

  YawPitchToRot(_yaw, _pitch, _rotation);

  _needIntersect = true;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::pan(double dx, double dy)
//--------------------------------------------------------------------
{
  // cambia l'effetto a seconda della distance
  
  float scale = -0.3f*_distance;

  osg::Matrix rotation_matrix;
  rotation_matrix.set(_rotation);
  osg::Vec3 dv(dx*scale,dy*scale,0.0f);
  _center += dv*rotation_matrix;

  _needIntersect = true;

}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::zoom(double dx, double dy)
//--------------------------------------------------------------------
{
  _distance *= (1.0f + dy);
  if( _distance < _minimumDistance ) _distance = _minimumDistance;

  _needIntersect = true;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::RotToYawPitch(osg::Matrixd m, double& yaw, double& pitch  )
//--------------------------------------------------------------------
{
  osg::Vec3d v = -getUpVector(m); // questo sarebbe il LookVector
  osg::Vec2d y(v[0], v[1]);
  osg::Vec2d p( y.length(), v[2] );
  
  double ToDegree = 180/osg::PI;
  yaw   = atan2(  y[1], y[0] ) *ToDegree -90;  // +  e - 90 trovati per tentativi, per matchare YawPitchToRot
  pitch = atan2(  p[1], p[0] ) *ToDegree +90;
  
  if( yaw<0 ) yaw += 360;
  if(_pitch < 2)  _pitch =2;
  if(_pitch > 80)  _pitch =80;

  //yaw = (int)yaw;
  //pitch = (int)pitch;
  //osg::notify(osg::WARN)<<" v= " << v[0] << " " << v[1] <<" " << v[2] <<std::endl;
  //osg::notify(osg::WARN)<<"  yaw= " << yaw << " pitch= " << pitch <<std::endl;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::YawPitchToRot(double yaw, double pitch, osg::Quat& r  )
//--------------------------------------------------------------------
{
  static double ToRad = osg::PI / 180.0; 
  r = 
    osg::Quat( _pitch*ToRad, osg::Vec3(0.1,0.0,0.0)) 
    * 
    osg::Quat( _yaw*ToRad,   osg::Vec3(0.0,0.0,1.0));

  osg::notify(osg::INFO)<<"@ yaw= " << yaw << " pitch= " << pitch << std::endl;
  //double a,b;
  //RotToYawPitch(osg::Matrixd::rotate(_rotation),a,b);
}
//--------------------------------------------------------------------
// verifica che il center sia sul terreno
// eventualmente lo sposta e corregge la distance
void mafOsgTerrainManipulator::IntersectTerrain()
//--------------------------------------------------------------------
{
   _needIntersect = false;
   
   if(!_NODE.get()) return;

   //devo creare un segmento - orientato come il lookAt
   // che parte da eye e arriva a ???
   osg::Matrix rotation_matrix(_rotation);
   osg::Vec3d lookVector = -getUpVector(rotation_matrix);
   lookVector.normalize();
   double d = 2 * _NODE->getBound().radius();
   osg::Vec3d eye= _center - lookVector * _distance;
   osg::Vec3d p1 = _center - lookVector * d;
   osg::Vec3d p2 = _center + lookVector * d;
   osg::Vec3d new_center;

   bool hitFound = Intersect( p1, p2, new_center );

   // sposto il centro e aggiorno la 
   // distance per mantenera la posizione di Eye
   if(hitFound)
   {
     osg::Vec3d delta = eye - new_center;
     _distance = delta.length();
     if( _distance < _minimumDistance ) _distance = _minimumDistance;
     //if( _distance < 100 ) _distance = 100;
     _center = new_center;
   }
}
//--------------------------------------------------------------------
bool mafOsgTerrainManipulator::Intersect( osg::Vec3d p1, osg::Vec3d p2, osg::Vec3d& result)
//--------------------------------------------------------------------
{
  osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment;
  seg->set(p1,p2);
  //osg::notify(WARN)  << " eye " << eye << " center " << _center  <<std::endl;
  //osg::notify(WARN)<<" near "<< near << " far "<< far  <<std::endl;

  osgUtil::IntersectVisitor iv;
  //iv.setTraversalMask(_intersectTraversalMask);
  iv.setTraversalMode(osgUtil::IntersectVisitor::TRAVERSE_ACTIVE_CHILDREN); //SIL Only visible children
  //iv.setLODSelectionMode(osgUtil::IntersectVisitor::LODSelectionMode::USE_SEGMENT_START_POINT_AS_EYE_POINT_FOR_LOD_LEVEL_SELECTION);
  iv.addLineSegment(seg.get());
  _NODE->accept(iv);
  if (iv.hits())
  {
    osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
    if (!hitList.empty())
    {
      for(osgUtil::IntersectVisitor::HitList::iterator hitr=hitList.begin();  hitr!=hitList.end();  ++hitr )
      {
        if( hitr->_geode.valid() )
        {
          if( hitr->_geode.get() != _exclude_node.get() )
          {
            osg::Vec3 p= hitr->getWorldIntersectPoint();
            if(!_hitfound) 
              result = p;
            else
              if(result[2]<p[2]) result = p;
            _hitfound=true; 
          }
          else
          {
            //osg::notify(INFO)<<"palla dribbled" << std::endl;
          }
        }
      }
    }
  }
  //if(_hitfound) osg::notify(WARN)<<"picked point: "<< result <<std::endl;
  return _hitfound;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::FlyToXY(double x, double y)
//--------------------------------------------------------------------
{
  if( !_NODE.get() ) return;
  double     r = _NODE->getBound().radius();
  osg::Vec3d c = _NODE->getBound().center();
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
  osg::Vec3d delta = p-_center;
  delta[2]=0;
  delta.normalize();
  double yaw  = atan2(  delta[1], delta[0] ) -osg::PI/2;  

  double fixed_yaw = yaw;
  /*
  if( (yaw-_yaw) < -180) 
    fixed_yaw += 360;
  if( (yaw-_yaw) > 180)  
    fixed_yaw -= 360;
  */

  static double ToRad = osg::PI / 180.0; 
  _f2.valid     = true;
  _f2.center    = p;
  _f2.distance  = _distance;
  _f2.pitch     = _pitch;
  _f2.yaw       = fixed_yaw;
  _f2.rotation  = osg::Quat( _pitch*ToRad,     osg::Vec3(0.1,0.0,0.0)) * 
                  osg::Quat(  fixed_yaw*ToRad, osg::Vec3(0.0,0.0,1.0));
  
  _request_fly = true;
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::FlyToXY_YPD(double x,double y,double yaw,double pitch, double dist)
//--------------------------------------------------------------------
{
  if( !_NODE.get() ) return;
  double     r = _NODE->getBound().radius();
  osg::Vec3d c = _NODE->getBound().center();
  osg::Vec3d p1(x, y, c[2] - r );
  osg::Vec3d p2(x, y, c[2] + r );
  osg::Vec3d p;
  bool found = Intersect( p1, p2, p );
  if(!found) 
  {
    osg::notify(WARN)<<"FlyToXY: failed to find Z" <<std::endl;
    return;
  }

  double fixed_yaw = yaw;
  /*
  if( (yaw-_yaw) < -180) 
    fixed_yaw += 360;
  if( (yaw-_yaw) > 180)  
    fixed_yaw -= 360;
  */
  static double ToRad = osg::PI / 180.0; 

  _f2.valid     = true;
  _f2.center    = p;
  _f2.distance  = dist;
  _f2.pitch     = pitch;
  _f2.yaw       = fixed_yaw;
  _f2.rotation  = osg::Quat( pitch*ToRad,       osg::Vec3(0.1,0.0,0.0)) * 
                  osg::Quat( fixed_yaw*ToRad,   osg::Vec3(0.0,0.0,1.0));

  _request_fly = true;
}
//---------------------------------------
void mafOsgTerrainManipulator::showGizmo(bool show)
//---------------------------------------
{
  if(show)
  {
    //assert(_gizmo.get() == NULL );
    if(_gizmo.get() == NULL)
    {
      float sz = 0.5;
      if(_NODE.get())
      {
        const osg::BoundingSphere& bs=_NODE->getBound();
        //const float scale = 0.0001f;
        //sz = osg::clampBetween(bs._radius * scale, 0.00001f, 1.0f );
        sz = bs._radius * 0.001f;
      }

      osg::ref_ptr<osg::Sphere>        gizmoShape = new osg::Sphere(osg::Vec3d(0,0,0),sz);
      osg::ref_ptr<osg::ShapeDrawable> gizmoDrawable = new osg::ShapeDrawable(gizmoShape.get());
      osg::ref_ptr<osg::Geode>         gizmoGeode = new osg::Geode();
      gizmoGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
      gizmoGeode->addDrawable(gizmoDrawable.get());

      _gizmo = new osg::MatrixTransform();
      _gizmo->addChild(gizmoGeode.get());
      setExcludeNode(gizmoGeode.get());
    }
    if(_gizmoParent.get() ) _gizmoParent->addChild(_gizmo.get());   
  }
  else
  {
    if(_gizmo.get() != NULL)
    {
      if(_gizmoParent.get() ) _gizmoParent->removeChild(_gizmo.get());   
      _gizmo = NULL;
    }
  }
}
//---------------------------------------
void mafOsgTerrainManipulator::moveGizmo()
//---------------------------------------
{
  if(_gizmo.get() == NULL ) return;
  _gizmo->setMatrix( osg::Matrixd::translate(_center));
}
//---------------------------------------
void mafOsgTerrainManipulator::setGizmoParent(osg::Group *gizmoParent)
//---------------------------------------
{
  if( gizmoParent == _gizmoParent.get() ) return;

  if(_gizmo.get() && _gizmoParent.get() ) _gizmoParent->removeChild(_gizmo.get());   

  _gizmoParent = gizmoParent;

  if(_gizmo.get() && _gizmoParent.get() ) _gizmoParent->addChild(_gizmo.get());   
  
}
//--------------------------------------------------------------------
void mafOsgTerrainManipulator::showHud(bool show)
//--------------------------------------------------------------------
{
  if(show)
  {
    if(_hud.get() == NULL)
      createHud();
    if(_gizmoParent.get() ) _gizmoParent->addChild(_hud.get());   
  }
  else
  {
    if(_hud.get() != NULL)
    {
      if(_gizmoParent.get() ) _gizmoParent->removeChild(_hud.get());   
      _hud = NULL;
      _text = NULL;
    }
  }
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

