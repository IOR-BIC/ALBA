/*=========================================================================

 Program: MAF2
 Module: vtkMAFLineStripSourceTest
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "mafDefines.h"
#include "vtkMAFLineStripSource.h"
#include "vtkMAFLineStripSourceTest.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"

//------------------------------------------------------------
void vtkMAFLineStripSourceTest::TestDynamicAllocation()
{
	vtkMAFLineStripSource *to = vtkMAFLineStripSource::New();
  to->Delete();
}