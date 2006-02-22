/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: sampleManipulator.cpp,v $
Language:  C++
Date:      $Date: 2006-02-22 16:51:27 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//#include "mafDefines.h" ---- non serve - non stiamo usando il maf
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <fstream>

#include "sampleManipulator.h"

#include <osg/Quat>
#include <osg/Node>
#include <osg/Group>
#include <osg/Notify>
#include <osg/io_utils>
#include <osgUtil/IntersectVisitor>
#include <osg/Geode>

using namespace osg;
using namespace osgGA;

//--------------------------------------------------------------------
// const
//--------------------------------------------------------------------
double ToRad = osg::PI / 180.0; 

//--------------------------------------------------------------------
sampleManipulator::sampleManipulator()  
//--------------------------------------------------------------------
{
   _yaw = 0;
   _pitch = 0;
   _position = osg::Vec3d(0,0,0);
   _height = 1;

   _step =1;
   _rstep =1;
   _walking = false;
}
//--------------------------------------------------------------------
sampleManipulator::~sampleManipulator()
//--------------------------------------------------------------------
{
}
//--------------------------------------------------------------------
osg::Matrixd sampleManipulator::getMatrix() const
//--------------------------------------------------------------------
{
  osg::Quat rot = 
    osg::Quat( (_pitch+90)*ToRad, osg::Vec3(0.1,0.0,0.0)) *
    osg::Quat( (_yaw  -90)*ToRad, osg::Vec3(0.0,0.0,1.0));

  return 
    osg::Matrixd::rotate(rot)* 
    osg::Matrix::translate(_position); 
}
//--------------------------------------------------------------------
osg::Matrixd sampleManipulator::getInverseMatrix() const
//--------------------------------------------------------------------
{
  osg::Quat rot = 
    osg::Quat( (_pitch+90)*ToRad, osg::Vec3(0.1,0.0,0.0)) *
    osg::Quat( (_yaw  -90)*ToRad, osg::Vec3(0.0,0.0,1.0));
  
  return
    osg::Matrix::translate(-_position) *
    osg::Matrixd::rotate(rot.inverse());
}
//--------------------------------------------------------------------
void sampleManipulator::setNode(osg::Node* node)
//--------------------------------------------------------------------
{
    _node = node;
    home();
}
//--------------------------------------------------------------------
void sampleManipulator::home(double currentTime )
//--------------------------------------------------------------------
{
  if( !getNode()) return;

  double radius = getNode()->getBound().radius();

  _position = getNode()->getBound().center();
  _yaw = 0;
  _pitch =0;
  _height = radius/100.0;
  _step = radius/500.0;

  IntersectTerrain();
}
//--------------------------------------------------------------------
bool sampleManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
//--------------------------------------------------------------------
{
  switch(ea.getEventType())
    {
        case(GUIEventAdapter::PUSH): //Mouse Button Down
          _x0 = ea.getXnormalized();
          _y0 = ea.getYnormalized();
          if( ea.getButtonMask() == GUIEventAdapter::RIGHT_MOUSE_BUTTON )
            _walking = true;
                                                  //Nota : -- non arrivano MOUSE_DRAG se il muose e' fermo
                                                  //          neanche con us.RequestContinousUpdate(true);
                                                  //          Pero' arrivano le FRAME ...
        break;

        case(GUIEventAdapter::RELEASE): //Mouse Button Up
          //if( ea.getButtonMask() == GUIEventAdapter::RIGHT_MOUSE_BUTTON )
            _walking = false;
        break;

        case(GUIEventAdapter::DRAG): // mouse move with buttons pressed
          {        
            if( ea.getButtonMask() == GUIEventAdapter::LEFT_MOUSE_BUTTON )
            {
              double dx = ea.getXnormalized() - _x0;
              double dy = ea.getYnormalized() - _y0;

              _yaw -= 60 * _rstep * dx;
              while( _yaw >= 360 ) _yaw -= 360;
              while( _yaw <  0   ) _yaw += 360;
              _pitch = osg::clampBetween( _pitch + 20 * _rstep * dy, -90.0, 90.0);

              _x0 = ea.getXnormalized();
              _y0 = ea.getYnormalized();
            }
          }
        break;

        case(GUIEventAdapter::MOVE): // mouse move without buttons pressed
          return false;
        break;

        case(GUIEventAdapter::KEYDOWN):
        {
            switch(ea.getKey()) 
            {
            case ' ':  // SPACE = reset camera to home position
              {
                _walking = false;
                home();
              }
            break;

            case GUIEventAdapter::KEY_Left: //turn left
            case 'a':
            case 'A':
              {
                _yaw += 3 * _rstep;
                while( _yaw >= 360 ) _yaw -= 360;
              }
            break;

            case GUIEventAdapter::KEY_Right:  //turn right
            case 'd':
            case 'D':
              {
                _yaw -= 3 * _rstep;
                while( _yaw < 0 ) _yaw += 360;
              }
            break;

            case GUIEventAdapter::KEY_Up: // step forward
            //case 'w': //-- w gia usato da 'wireframe'
            //case 'W':
              {
                _position[0] += _step * cos(_yaw*ToRad);
                _position[1] += _step * sin(_yaw*ToRad);
                IntersectTerrain();
              }
            break;

            case GUIEventAdapter::KEY_Down: // step backward
            //case 's': //-- s gia usato da 'statistics'
            //case 'S':
              {
                _position[0] -= _step * cos(_yaw*ToRad);
                _position[1] -= _step * sin(_yaw*ToRad);
                IntersectTerrain();
              }
            break;

            case GUIEventAdapter::KEY_Page_Up:  // aim up
              {
                _pitch = osg::clampBetween( _pitch + _rstep, -90.0, 90.0);
              }
            break;

            case GUIEventAdapter::KEY_Page_Down: // aim down
              {
                _pitch = osg::clampBetween( _pitch - _rstep, -90.0, 90.0);
              }
            break;


            default:
              return false; // event not handled
            break;
            }
        }
        break;

        
        case(GUIEventAdapter::FRAME): //chiamato ad ogni Render
          {
            if(_walking)
            {
              _position[0] += _step * cos(_yaw*ToRad);
              _position[1] += _step * sin(_yaw*ToRad);
              IntersectTerrain();
            }
          }
        break;

        default:
          return false; // event not handled
    }
    return true; // event handled
}
//--------------------------------------------------------------------
void sampleManipulator::setByMatrix(const osg::Matrixd& matrix)
//--------------------------------------------------------------------
{
    /*
       todo: estrarre la rotazione dalla matrice
    */
    _position = matrix.getTrans(); 
    _yaw =0;
    _pitch =0;

    IntersectTerrain();
}
//--------------------------------------------------------------------
// verifica che il center sia sul terreno
// eventualmente lo sposta 
void sampleManipulator::IntersectTerrain()
//--------------------------------------------------------------------
{
   if( !getNode() ) return;

   //creo un segmento centrato su _position e orientato lungo z
   double d = _height;
   osg::Vec3d z(0,0,1);
   osg::Vec3d p1 = _position - z * d;
   osg::Vec3d p2 = _position + z * d;
   osg::Vec3d intersection;
   
   // provo prima su un segmento corto (costa meno)
   // se non trovo intersezione lo allargo
   bool found = Intersect( p1, p2, intersection );
   if(!found)
   {
      d = getNode()->getBound().radius(); 
      p1 = _position - z * d;
      p2 = _position + z * d;
      found = Intersect( p1, p2, intersection );
   }
   if(!found)
   {
     d *= 10;  // sono finito a meno infinito ?
     p1 = _position - z * d;
     p2 = _position + z * d;
     found = Intersect( p1, p2, intersection );
   }


   if(found)
      _position = intersection + z * _height;
}
//--------------------------------------------------------------------
bool sampleManipulator::Intersect( osg::Vec3d p1, osg::Vec3d p2, osg::Vec3d& result)
//--------------------------------------------------------------------
{
  if( !getNode() ) return false;
  bool found = false;

  osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment;
  seg->set(p1,p2);

  osgUtil::IntersectVisitor iv;
  iv.setTraversalMode(osgUtil::IntersectVisitor::TRAVERSE_ACTIVE_CHILDREN); //cosi attraversa solo i figli visibili (e quindi caricati)
  iv.addLineSegment(seg.get());
  getNode()->accept(iv);
  if (iv.hits())
  {
    osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(seg.get());
    if (!hitList.empty())
    {
      for(osgUtil::IntersectVisitor::HitList::iterator hitr=hitList.begin();  hitr!=hitList.end();  ++hitr )
      {
        if( hitr->_geode.valid() )
        {
          osg::Vec3 p= hitr->getWorldIntersectPoint();
          if(!found) 
            result = p;
          else
            if(result[2]<p[2]) // cosi ritorno l'intersezione con la z maggiore
              result = p;
          found=true; 
        }
      }
    }
  }
  return found;
}
