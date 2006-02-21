/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgTerrainManipulator.h,v $
Language:  C++
Date:      $Date: 2006-02-21 16:14:10 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafOsgTerrainManipulator__
#define __mafOsgTerrainManipulator__ 1

#include <osgGA/MatrixManipulator>
#include <osg/Quat>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/ShapeDrawable>
//---------------------------------------------------
//forward refs
//---------------------------------------------------
class mafOsgTerrainManipulatorGui;
using namespace osgGA;
//---------------------------------------------------
class mafOsgTerrainManipulator : public MatrixManipulator
//---------------------------------------------------
{
    public:
        //--------------------------------------------------------------------
        struct Camera
        //--------------------------------------------------------------------
        {
          osg::Vec3d   _center;
          osg::Quat    _rotation;
          double       _yaw;
          double       _pitch;
          double       _distance;
          osg::Matrixd _matrix;
          osg::Matrixd _inversematrix;

          //--------------------------------------------------------------------
          Camera()
          //--------------------------------------------------------------------
          {
            _center = osg::Vec3d(0,0,0);
            _yaw = 0;
            _pitch = 0;
            _distance = 1;
            UpdateRotation();
          }
          //--------------------------------------------------------------------
          void Interpolate( Camera c1, Camera c2, double t )
          //--------------------------------------------------------------------
          {
            _center   = c1._center   *(1-t) + c2._center   * t;
            _distance = c1._distance *(1-t) + c2._distance * t;
            _yaw      = c1._yaw      *(1-t) + c2._yaw      * t;
            _pitch    = c1._pitch    *(1-t) + c2._pitch    * t;
            UpdateRotation();
          };
          //--------------------------------------------------------------------
          void InterpolateUsingQuaternion( Camera c1, Camera c2, double t )
          //--------------------------------------------------------------------
          {
            _center   = c1._center   *(1-t) + c2._center   * t;
            _distance = c1._distance *(1-t) + c2._distance * t;
            _rotation = c1._rotation *(1-t) + c2._rotation * t;
            RotToYawPitch(osg::Matrixd::rotate(_rotation), _yaw, _pitch );
            UpdateMatrix();
          };
          //--------------------------------------------------------------------
          void UpdateRotation()
          //--------------------------------------------------------------------
          {
            YawPitchToRot(_yaw,_pitch,_rotation);
            UpdateMatrix();
          };
          //--------------------------------------------------------------------
          void UpdateMatrix()
          //--------------------------------------------------------------------
          {
             _matrix =        osg::Matrixd::translate(0.0,0.0,_distance) 
                             *osg::Matrixd::rotate(_rotation)
                             *osg::Matrix::translate(_center); 

             _inversematrix = osg::Matrix::translate(-_center)
                             *osg::Matrixd::rotate(_rotation.inverse())
                             *osg::Matrixd::translate(0.0,0.0,-_distance);
          };
          //--------------------------------------------------------------------
          static void YawPitchToRot(double yaw, double pitch, osg::Quat& r  )
          //--------------------------------------------------------------------
          {
            static double ToRad = osg::PI / 180.0; 
            r = 
              osg::Quat( pitch*ToRad, osg::Vec3(0.1,0.0,0.0)) 
              * 
              osg::Quat( yaw*ToRad,   osg::Vec3(0.0,0.0,1.0));
          };
          //--------------------------------------------------------------------
          static void RotToYawPitch(osg::Matrixd m, double& yaw, double& pitch  )
          //--------------------------------------------------------------------
          {
            //osg::Vec3d v = -getUpVector(m);  // commentata - e' una funzione di MatrixManipulator
            osg::Vec3d v(m(2,0),m(2,1),m(2,2));// questo sarebbe il LookVector
            osg::Vec2d y(v[0], v[1]);
            osg::Vec2d p( y.length(), v[2] );

            double ToDegree = 180/osg::PI;
            yaw   = atan2(  y[1], y[0] ) *ToDegree -90;  // +  e - 90 trovati per tentativi, per matchare YawPitchToRot
            pitch = atan2(  p[1], p[0] ) *ToDegree +90;

            if( yaw<0 ) yaw += 360;
            if( pitch < 2)  pitch =2;
            if( pitch > 80) pitch =80;
          };
        };
      
        mafOsgTerrainManipulator();
        virtual const char* className() const { return "Terrain"; }

        virtual void setByMatrix(const osg::Matrixd& matrix);
        virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }
        virtual osg::Matrixd getMatrix() const;
        virtual osg::Matrixd getInverseMatrix() const;

        // nodo usato per impostare la HomePosition ed anche per fare l'Intersect
        virtual void setNode(osg::Node*); 
        virtual const osg::Node* getNode() const {return _terrain.get();};
        virtual osg::Node* getNode() {return _terrain.get();};
        virtual void home(const GUIEventAdapter& ea,GUIActionAdapter& us);
        //virtual void computeHomePosition();


        //virtual void setExcludeNode(osg::Node* en) {_exclude_terrain = en;}; 
        //virtual const osg::Node* getExcludeNode() const {return _exclude_terrain.get();};
       
        virtual void init(const GUIEventAdapter& ea,GUIActionAdapter& us);
        virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& us);
        

        void FlyToXY(double x, double y);
        void FlyToXY_YPD(double x,double y,double yaw,double pitch, double dist);

        void   SetFlyingDuration( double flying_duration) {_flying_duration = flying_duration; };
        double GetFlyingDuration()                        {return _flying_duration; };

        void       SetCenter(osg::Vec3d pos)            {_camera._center=pos; _needIntersect = true;};
        void       SetCenter(double pos[3])             {_camera._center[0]=pos[0]; _camera._center[1]=pos[1]; _camera._center[2]=pos[2]; _needIntersect = true;};
        void       GetCenter(double pos[3])             {pos[0]=_camera._center[0]; pos[1]=_camera._center[1]; pos[2]=_camera._center[2];};
        osg::Vec3d GetCenter()                          {return _camera._center;};

        void       SetYaw(double yaw)                   {_camera._yaw = yaw; _camera.UpdateRotation(); };
        double     GetYaw()                             {return _camera._yaw;};

        void       SetPitch(double pitch)               {_camera._pitch = pitch; _camera.UpdateRotation(); };
        double     GetPitch()                           {return _camera._pitch;};

        void       SetDistance(double distance)         {_camera._distance = distance; _camera.UpdateMatrix(); };
        double     GetDistance()                        {return _camera._distance;};

        void       SetMinDistance(double minDistance)   {_minimumDistance = minDistance;};
        double     GetMinDistance()                     {return _minimumDistance;};

    protected:
        virtual ~mafOsgTerrainManipulator();
        void computePosition(const osg::Vec3d& eye,const osg::Vec3d& lv,const osg::Vec3d& up);

        void flushMouseEventStack();
        void addMouseEvent(const GUIEventAdapter& ea);
        bool calcMovement();
        void rotate(double dx, double dy);
        void pan(double dx, double dy);
        void zoom(double dx, double dy);
        bool isMouseMoving(); // true if the speed at which the mouse is moving is over a threshold 

        void IntersectTerrain();
        bool Intersect( osg::Vec3d p1, osg::Vec3d p2, osg::Vec3d& result);

        // Internal event stack comprising last three mouse events.
        osg::ref_ptr<const GUIEventAdapter> _ga_t1;
        osg::ref_ptr<const GUIEventAdapter> _ga_t0;

        osg::ref_ptr<osg::Node> _terrain;
        osg::ref_ptr<osg::Node> _exclude_terrain;

        Camera      _camera; // current camera position
        bool        _thrown;
        bool        _needIntersect;
        bool        _intersectionfound;

        // fly
        bool    _flying;
        bool    _request_fly;
        double  _begin_fly_time;
        double  _flying_duration;
        Camera  _f1; // fly begin position
        Camera  _f2; // fly end position


public:
        virtual void setGizmoParent(osg::Group *gizmoParent); // nodo dove si attaccano hud e Gizmo
        virtual void showHud(bool show);   
        virtual void showGizmo(bool show); 
        virtual bool isHudShown()  {return _gizmoParent.get() && _hud->getNumParents();  };
        virtual bool isGizmoShown(){return _gizmoParent.get() && _gizmo->getNumParents();};

protected:
        void createHud();
        void createGizmo();
        osg::ref_ptr<osg::MatrixTransform> _gizmo;
        osg::ref_ptr<osg::Sphere>          _gizmoShape;
        osg::ref_ptr<osg::Geode>           _gizmoGeode; //when intersecting terrain avoid this 
        osg::ref_ptr<osg::Group>           _gizmoParent;
        osg::ref_ptr<osgText::Text>        _text; //hud text
        osg::ref_ptr<osg::Group>           _hud;
};

#endif


