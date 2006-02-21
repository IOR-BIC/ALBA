/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgTerrainManipulator.h,v $
Language:  C++
Date:      $Date: 2006-02-21 16:12:39 $
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
        mafOsgTerrainManipulator();
        virtual const char* className() const { return "Terrain"; }

        virtual void setByMatrix(const osg::Matrixd& matrix);
        virtual void setByInverseMatrix(const osg::Matrixd& matrix) { setByMatrix(osg::Matrixd::inverse(matrix)); }
        virtual osg::Matrixd getMatrix() const;
        virtual osg::Matrixd getInverseMatrix() const;

        virtual void setGizmoParent(osg::Group *gizmoParent); // nodo dove si attaccano hud e Gizmo
        virtual void showHud(bool show);   // crea un nodo che si attacca sotto gizmoParent
        virtual bool isHudShown() {return _hud.get() != NULL  && _gizmoParent.get() != NULL; };
        virtual void showGizmo(bool show); // crea un nodo che si attacca sotto gizmoParent
        virtual bool isGizmoShown() {return _gizmo.get() != NULL  && _gizmoParent.get() != NULL; };

        virtual void setNode(osg::Node*); 
        virtual const osg::Node* getNode() const {return _NODE.get();};
        virtual osg::Node* getNode() {return _NODE.get();};

        virtual void setExcludeNode(osg::Node* en) {_exclude_node = en;}; 
        virtual const osg::Node* getExcludeNode() const {return _exclude_node.get();};

        /** Move the camera to the default position.*/
        virtual void home(const GUIEventAdapter& ea,GUIActionAdapter& us);
        //virtual void computeHomePosition();
        
        virtual void init(const GUIEventAdapter& ea,GUIActionAdapter& us);
        virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& us);
        
        void FlyToXY(double x, double y);
        void FlyToXY_YPD(double x,double y,double yaw,double pitch, double dist);
        
        void SetFlyingDuration( double flying_duration) {_flying_duration = flying_duration; };
        void SetCenter(osg::Vec3d pos)           {_center=pos; _needIntersect = true; _needIntersect = true;};
        void SetCenter(double pos[3])            {_center[0]=pos[0]; _center[1]=pos[1]; _center[2]=pos[2]; _needIntersect = true;};
        void SetYaw(double yaw)                  {_yaw = yaw;     YawPitchToRot(_yaw,_pitch,_rotation);};
        void SetPitch(double pitch)              {_pitch = pitch; YawPitchToRot(_yaw,_pitch,_rotation);};
        void SetDistance(double distance)        {_distance = distance;};
        void SetMinDistance(double minDistance)  {_minimumDistance = minDistance;};

        double     GetFlyingDuration()      {return _flying_duration; };
        void       GetCenter(double pos[3]) {pos[0]=_center[0]; pos[1]=_center[1]; pos[2]=_center[2];};
        osg::Vec3d GetCenter()              {return _center;};
        double     GetYaw()                 {return _yaw;};
        double     GetPitch()               {return _pitch;};
        double     GetDistance()            {return _distance;};
        double     GetMinDistance()         {return _minimumDistance;};

    protected:
        virtual ~mafOsgTerrainManipulator();
        void computePosition(const osg::Vec3d& eye,const osg::Vec3d& lv,const osg::Vec3d& up);

        void flushMouseEventStack();
        void addMouseEvent(const GUIEventAdapter& ea);
        bool calcMovement();
        void rotate(double dx, double dy);
        void pan(double dx, double dy);
        void zoom(double dx, double dy);
        bool isMouseMoving(); /* true if the speed at which the mouse is moving is over a threshold */

        void RotToYawPitch(osg::Matrixd m, double& yaw, double& pitch  );
        void YawPitchToRot(double yaw, double pitch, osg::Quat& r  );
        void IntersectTerrain();
        bool Intersect( osg::Vec3d p1, osg::Vec3d p2, osg::Vec3d& result);

        // Internal event stack comprising last three mouse events.
        osg::ref_ptr<const GUIEventAdapter> _ga_t1;
        osg::ref_ptr<const GUIEventAdapter> _ga_t0;

        osg::ref_ptr<osg::Node> _NODE;
        osg::ref_ptr<osg::Node> _exclude_node;
        bool                    _thrown;
        osg::Vec3d              _center;
        osg::Quat               _rotation;
        float                   _distance;
        osg::Vec3d              _previousUp;

        struct FlyPose
        {
          osg::Vec3d  center;
          osg::Quat   rotation;
          double      yaw;
          double      pitch;
          double      distance;
          bool        valid;
        };
        FlyPose _f1; // begin Fly position
        FlyPose _f2; // end Fly position
     
        // fly
        bool                    _flying;
        bool                    _request_fly;
        double                  _begin_fly_time;
        double                  _flying_duration;

        double       _yaw;
        double       _pitch;
        bool        _needIntersect;
        bool        _hitfound;

        void moveGizmo();
        void createHud();
        osg::ref_ptr<osg::MatrixTransform> _gizmo;
        osg::ref_ptr<osg::Group>           _gizmoParent;

        osg::ref_ptr<osgText::Text>        _text; //hud text
        osg::ref_ptr<osg::Group>           _hud;
};

#endif


