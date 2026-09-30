/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: vtkALBAVolumeTextureMapper2D.cxx,v $ 
Language: C++ 
Date: $Date: 2010-07-13 12:08:02 $ 
Version: $Revision: 1.1.2.2 $ 
Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/

#include "vtkALBAVolumeTextureMapper2D.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkMatrix4x4.h"
#include "vtkTimerLog.h"
#include "vtkPlaneCollection.h"
#include "vtkPlane.h"
#include "vtkImageData.h"
#include "vtkVolumeProperty.h"
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <gl/GL.h>
#else
    #include <GL/gl.h>
#endif

#include <vtkOpenGLHelper.h>

vtkStandardNewMacro(vtkALBAVolumeTextureMapper2D);

#include "albaMemDbg.h"

//#define __PROFILING__
//#include <atlbase.h>
//#include <BSGenLib.h>
//
//__PROFILING_DECLARE_DEFAULT_PROFILER(false);


//------------------------------------------------------------------------
/*virtual*/ void vtkALBAVolumeTextureMapper2D::Render( vtkRenderer *ren, vtkVolume *vol )
//------------------------------------------------------------------------
{  
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // Initialize rendering for this volume.
  // N.B. Code mostly adopted from vtkVolumeRayCastMapper::Render
  // See the description of class in .H file for details

  //Luigi Calori (CINECA) disabled blending for opaque objects in 
  //vtkOpenGLProperty.cxx (line 83), which unfortunately corrupts 
  //the correct visualization with MIP if some opaque model (e.g., 
  //BoundingBox) is displayed together with the volume in one view.
  //This is the patch that overcomes that problem.

  GLboolean bBlend = glIsEnabled(GL_BLEND);
  glEnable(GL_BLEND);

#pragma region VTK Code (Initialization)
  vtkMatrix4x4       *matrix = vtkMatrix4x4::New();
  vtkPlaneCollection *clipPlanes;
  vtkPlane           *plane;
  int                i, numClipPlanes = 0;
  double             planeEquation[4];

  this->Timer->StartTimer();

  // Let the superclass take care of some initialization
  this->PreRender(ren, vol, NULL, NULL,0,0);

  // build transformation 
  vol->GetMatrix(matrix);
  matrix->Transpose();

  // Use the OpenGL clip planes
  clipPlanes = this->ClippingPlanes;
  if ( clipPlanes )
  {
    numClipPlanes = clipPlanes->GetNumberOfItems();
    if (numClipPlanes > 6)
    {
      vtkErrorMacro(<< "OpenGL guarantees only 6 additional clipping planes");
    }

    for (i = 0; i < numClipPlanes; i++)
    {
      glEnable((GLenum)(GL_CLIP_PLANE0+i));

      plane = (vtkPlane *)clipPlanes->GetItemAsObject(i);

      planeEquation[0] = plane->GetNormal()[0]; 
      planeEquation[1] = plane->GetNormal()[1]; 
      planeEquation[2] = plane->GetNormal()[2];
      planeEquation[3] = -(planeEquation[0]*plane->GetOrigin()[0]+
        planeEquation[1]*plane->GetOrigin()[1]+
        planeEquation[2]*plane->GetOrigin()[2]);
      glClipPlane((GLenum)(GL_CLIP_PLANE0+i),planeEquation);
    }
  }


  // insert model transformation 
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glMultMatrixd(matrix->Element[0]);

  // Turn lighting off - the polygon textures already have illumination
  glDisable( GL_LIGHTING );

  // Turn texturing on so that we can draw the textured polygons
  glEnable( GL_TEXTURE_2D );

#ifdef GL_VERSION_1_1
  GLuint tempIndex;
  glGenTextures(1, &tempIndex);
  glBindTexture(GL_TEXTURE_2D, tempIndex);
#endif

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glColor3f( 1.0, 1.0, 1.0 );
#pragma endregion
  
  //BES: GenerateTexturesAndRenderQuads checks Shade to decide whether to create a texture or not
  //and after that it calls once again initialization (which sets Shade back to original value)
  //so the rendering is not affected by this hack

  bool oldShade = vol->GetProperty()->GetShade();
  vol->GetProperty()->SetShade(false);

  this->GPURender(ren, vol);

  vol->GetProperty()->SetShade(oldShade);

#pragma region VTK Code(rendering)  
  // pop transformation matrix
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();

  matrix->Delete();

  glDisable( GL_TEXTURE_2D );

#ifdef GL_VERSION_1_1
  glFlush();
  glDeleteTextures(1, &tempIndex);
#endif

  // Turn lighting back on
  glEnable( GL_LIGHTING );

  if ( clipPlanes )
  {
    for (i = 0; i < numClipPlanes; i++)
    {
      glDisable((GLenum)(GL_CLIP_PLANE0+i));
    }
  }

  this->Timer->StopTimer();      

  this->TimeToDraw = (float)this->Timer->GetElapsedTime();

  // If the timer is not accurate enough, set it to a small
  // time so that it is not zero
  if ( this->TimeToDraw == 0.0 )
  {
    this->TimeToDraw = 0.0001;
  }  
#pragma endregion

  if (!bBlend)
    glDisable(GL_BLEND);
}
