/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOsgSky.h,v $
Language:  C++
Date:      $Date: 2006-02-21 16:14:10 $
Version:   $Revision: 1.1 $
Authors:   Silvano Imboden
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafOsgSky_h__
#define __mafOsgSky_h__

#include <assert.h>

#include <osg/Group>
#include <osgProducer/Viewer>
#include <osg/CameraNode>

//--------------------------------------------------------------------------------
// F.R.
//--------------------------------------------------------------------------------
class mafOsgSkyCallback;
class mafOsgTerrainManipulator;
//--------------------------------------------------------------------------------
class mafOsgSky 
//--------------------------------------------------------------------------------
{
public:
    mafOsgSky( osgProducer::Viewer* viewer, mafOsgTerrainManipulator *manip );
   ~mafOsgSky(void);

    void Setup( double radius, double pitchTable[9], wxColour colorTable[9] );
    void SetTexture( std::string filename );

    void GetPitchTable( double   pitchTable[9]) { for(int i=0; i<9; i++) pitchTable[i]=m_pitchTable[i]; };
    void GetColorTable( wxColour colorTable[9]) { for(int i=0; i<9; i++) colorTable[i]=m_colorTable[i]; };
    double GetRadius() { return m_radius;};

    void Update(); // called by the update callback
protected:
    void Create();
  
    osg::ref_ptr<osgProducer::Viewer>       m_viewer;
    osg::ref_ptr<osg::MatrixTransform>      m_sky;
    osg::ref_ptr<osg::StateSet>             m_ss;
    osg::ref_ptr<mafOsgSkyCallback>         m_cb;

    double      m_pitchTable[9];
    wxColour    m_colorTable[9];
    double      m_radius;
    bool        m_blend;  // finire
    std::string m_texfile;

};

#endif //__mafOsgSky_h__



