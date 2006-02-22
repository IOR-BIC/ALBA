/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: sampleManipulator.h,v $
Language:  C++
Date:      $Date: 2006-02-22 16:51:27 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __sampleManipulator__
#define __sampleManipulator__ 1

#include <osgGA/MatrixManipulator>
#include <osg/Quat>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/ShapeDrawable>

using namespace osgGA;
//---------------------------------------------------
class sampleManipulator : public MatrixManipulator
//---------------------------------------------------
{
  public:

    sampleManipulator();
    virtual const char* className() const { return "sampleManipulator"; }

    // chiamati da fuori
    virtual void setByMatrix(const osg::Matrixd& matrix);
    virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }
    virtual osg::Matrixd getMatrix() const;
    virtual osg::Matrixd getInverseMatrix() const;

    // funzioni virtuali della classe base
    // nodo usato per impostare la HomePosition ed anche per fare l'Intersect
          void setNode(osg::Node* node); 
    osg::Node* getNode() {return _node.get();}; 

    virtual void home(double currentTime = 0 );
    virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& us);

  protected:

    virtual ~sampleManipulator();
    bool calcMovement();
    //void rotate(double dx, double dy);
    //void pan(double dx, double dy);
    //void zoom(double dx, double dy);

    void IntersectTerrain();
    bool Intersect( osg::Vec3d p1, osg::Vec3d p2, osg::Vec3d& result);

    osg::ref_ptr<osg::Node> _node;
    
    osg::Vec3d   _position; // posizione camera
    double       _yaw;      // angolo di rotazione della camera intorno a z [0..360]
    double       _pitch;    // angolo di rotazione della camera intorno a x [0..90]
    double       _height;   // altezza dal terreno

    double       _step;   // lunghezza di un passo
    double       _rstep;  // passo di rotazione

    double       _x0,_y0;  // previous mouse position
    bool         _walking;

};

#endif



