/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBACellLocator
 Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*//*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: vtkALBACellLocator.cxx,v $ 
Language: C++ 
Date: $Date: 2011-02-16 21:15:02 $ 
Version: $Revision: 1.1.2.1 $ 
Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/

#include "vtkALBACellLocator.h"

#include "vtkCellArray.h"
#include "vtkGenericCell.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkBox.h"

#include <math.h>

vtkStandardNewMacro(vtkALBACellLocator);


//#define __PROFILING__
//#include <atlbase.h>
//#include <BSGenLib.h>
//
//__PROFILING_DECLARE_DEFAULT_PROFILER(false);

//----------------------------------------------------------------------------
void vtkALBACellLocator::FindCellsAlongLine(double p1[3], double p2[3], double vtkNotUsed(tol),
                                        vtkIdList *cells)
{
  cells->Reset();

  double origin[3];
  double direction1[3];
  double direction2[3];
  double direction3[3];
  double hitPosition[3];
  double hitCellBoundsPosition[3], cellBounds[6];
  int hitCellBounds;
  double result;
  double bounds2[6];
  int i, leafStart, prod, loop;
  vtkIdType cellId, cId;
  int idx;
  double tMax, dist[3];
  int npos[3];
  int pos[3];
  int bestDir;
  double stopDist, currDist;
  double length, maxLength=0.0;
  
  // convert the line into i,j,k coordinates
  tMax = 0.0;
  for (i=0; i < 3; i++) 
    {
    direction1[i] = p2[i] - p1[i];
    length = this->Bounds[2*i+1] - this->Bounds[2*i];
    if ( length > maxLength )
      {
      maxLength = length;
      }
    origin[i] = (p1[i] - this->Bounds[2*i]) / length;
    direction2[i] = direction1[i]/length;
    
    bounds2[2*i]   = 0.0;
    bounds2[2*i+1] = 1.0;
    tMax += direction2[i]*direction2[i];
    }
  
  // create a parametric range around the tolerance
  stopDist = tMax*this->NumberOfDivisions;
  for (i = 0; i < 3; i++) 
    {
    direction3[i] = direction2[i]/tMax;
    }
  
  if (vtkBox::IntersectBox(bounds2, origin, direction2, hitPosition, result))
    {
    // start walking through the octants
    prod = this->NumberOfDivisions*this->NumberOfDivisions;
    leafStart = this->NumberOfOctants - this->NumberOfDivisions*prod;
    
    // Clear the array that indicates whether we have visited this cell.
    // The array is only cleared when the query number rolls over.  This
    // saves a number of calls to memset.
    this->QueryNumber++;
    if (this->QueryNumber == 0)
      {
      this->ClearCellHasBeenVisited();
      this->QueryNumber++;    // can't use 0 as a marker
      }
    
    // set up curr and stop dist
    currDist = 0;
    for (i = 0; i < 3; i++)
      {
      currDist += (hitPosition[i] - origin[i])*(hitPosition[i] - origin[i]);
      }
    currDist = sqrt(currDist)*this->NumberOfDivisions;
    
    // add one offset due to the problems around zero
    for (loop = 0; loop <3; loop++)
      {
      hitPosition[loop] = hitPosition[loop]*this->NumberOfDivisions + 1.0;
      pos[loop] = static_cast<int>(hitPosition[loop]);
      // Adjust right boundary condition: if we intersect from the top, right,
      // or back; then pos must be adjusted to a valid octant index 
      if (pos[loop] > this->NumberOfDivisions)
        {
        pos[loop] = this->NumberOfDivisions;
        }
      }
    
    idx = leafStart + pos[0] - 1 + (pos[1] - 1)*this->NumberOfDivisions 
      + (pos[2] - 1)*prod;
    
    while ( (pos[0] > 0) && (pos[1] > 0) && (pos[2] > 0) &&
      (pos[0] <= this->NumberOfDivisions) &&
      (pos[1] <= this->NumberOfDivisions) &&
      (pos[2] <= this->NumberOfDivisions) &&
      (currDist < stopDist))
      {
      if (this->Tree[idx])
        {
        this->ComputeOctantBounds(pos[0]-1,pos[1]-1,pos[2]-1);
        for (tMax = VTK_DOUBLE_MAX, cellId=0; 
        cellId < this->Tree[idx]->GetNumberOfIds(); cellId++) 
          {
          cId = this->Tree[idx]->GetId(cellId);
          if (this->CellHasBeenVisited[cId] != this->QueryNumber)
            {
            this->CellHasBeenVisited[cId] = this->QueryNumber;
            hitCellBounds = 0;
            
            // check whether we intersect the cell bounds
            if (this->CacheCellBounds)
              {
              hitCellBounds = vtkBox::IntersectBox(this->CellBounds[cId],
                                                   p1, direction1,
                                                   hitCellBoundsPosition, result);
              }
            else 
              {
              this->DataSet->GetCellBounds(cId, cellBounds);
              hitCellBounds = vtkBox::IntersectBox(cellBounds,
                                                   p1, direction1,
                                                   hitCellBoundsPosition, result);
              }

            if (hitCellBounds)
              {
              cells->InsertUniqueId(cId);
              } // if (hitCellBounds)
            } // if (!this->CellHasBeenVisited[cId])
          }
        }
      
      // move to the next octant
      tMax = VTK_DOUBLE_MAX;
      bestDir = 0;
      for (loop = 0; loop < 3; loop++)
        {
        if (direction3[loop] > 0)
          {
          npos[loop] = pos[loop] + 1;
          dist[loop] = (1.0 - hitPosition[loop] + pos[loop])/direction3[loop];
          if (dist[loop] == 0)
            {
            dist[loop] = 1.0/direction3[loop];
            }
          if (dist[loop] < 0)
            {
            dist[loop] = 0;
            }
          if (dist[loop] < tMax)
            {
            bestDir = loop;
            tMax = dist[loop];
            }
          }
        if (direction3[loop] < 0)
          {
          npos[loop] = pos[loop] - 1;
          dist[loop] = (pos[loop] - hitPosition[loop])/direction3[loop];
          if (dist[loop] == 0)
            {
            dist[loop] = -0.01/direction3[loop];
            }
          if (dist[loop] < 0)
            {
            dist[loop] = 0;
            }
          if (dist[loop] < tMax)
            {
            bestDir = loop;
            tMax = dist[loop];
            }
          }
        }
      // update our position
      for (loop = 0; loop < 3; loop++)
        {
        hitPosition[loop] += dist[bestDir]*direction3[loop];
        }
      currDist += dist[bestDir];
      // now make the move, find the smallest distance
      // only cross one boundry at a time
      pos[bestDir] = npos[bestDir];
      
      idx = leafStart + pos[0] - 1 + (pos[1]-1)*this->NumberOfDivisions + 
        (pos[2]-1)*prod;
      }
    }
}

