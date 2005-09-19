/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkContourVolumeMapperAdv.cxx,v $
  Language:  C++
  Date:      $Date: 2005-09-19 15:15:12 $
  Version:   $Revision: 1.2 $


Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include <assert.h>

#include "vtkObjectFactory.h"
#include "vtkMarchingSquaresCases.h"
#include "vtkTransform.h"

#include "vtkContourVolumeMapper.h"

using namespace vtkContourVolumeMapperNamespace;

static const vtkMarchingSquaresLineCases* marchingSquaresCases = vtkMarchingSquaresLineCases::GetCases();

void vtkContourVolumeMapper::PrepareContours(const int slice, const void *imageData, ListOfPolyline2D& polylines) {
  this->Polylines = &polylines;

  switch (this->GetDataType()) {
    case VTK_CHAR:
      this->PrepareContoursTemplate(slice, (const char*)imageData);
      break;
    case VTK_UNSIGNED_CHAR:
      this->PrepareContoursTemplate(slice, (const unsigned char*)imageData);
      break;
    case VTK_SHORT:
      this->PrepareContoursTemplate(slice, (const short*)imageData);
      break;
    case VTK_UNSIGNED_SHORT:
      this->PrepareContoursTemplate(slice, (const unsigned short*)imageData);
      break;
    }
  }

//------------------------------------------------------------------------------
template<typename DataType> void vtkContourVolumeMapper::PrepareContoursTemplate(const int slice, const DataType *imageData) {
  const DataType (* const BlockMinMax)[2] = (DataType (*)[2])((DataType *)this->BlockMinMax + 2 * (slice >> VoxelBlockSizeLog) * this->NumBlocks[0] * this->NumBlocks[1]);
  const DataType ContourValue = (DataType)this->ContourValue;
  const int lastXBlock = this->NumBlocks[0] - 1, lastYBlock = this->NumBlocks[1] - 1;
  const int lastXBlockSize = this->DataDimensions[0] - 1 - (lastXBlock << VoxelBlockSizeLog), lastYBlockSize = this->DataDimensions[1] - 1 - (lastYBlock << VoxelBlockSizeLog);

  ListOfPolyline2D& polylines = *this->Polylines;
  polylines.clear();
  
  int statCounter = 0;

  for (int byi = 0, yblock = 0; byi < this->NumBlocks[1]; byi++, yblock += this->NumBlocks[0]) {
    const int yblockSize = (byi < lastYBlock) ? VoxelBlockSize : lastYBlockSize;
    for (int bxi = 0, block = yblock; bxi < this->NumBlocks[0]; bxi++, block++) {
      if (ContourValue > BlockMinMax[block][1] || ContourValue < BlockMinMax[block][0])
        continue; // skip the block
      const int xblockSize = (bxi < lastXBlock) ? VoxelBlockSize : lastXBlockSize;

      const int xBlock = (bxi << VoxelBlockSizeLog);
      const int yBlock = (byi << VoxelBlockSizeLog);
      const DataType *imageBlock = imageData + yBlock * this->DataDimensions[0] + xBlock;

      // build the contours
      for (int yi = 0; yi < yblockSize; yi++, imageBlock += this->DataDimensions[0]) {
        for (int xi = 0; xi < xblockSize; xi++) {
          const DataType * const voxelPtr = imageBlock + xi;

          const DataType voxelVals[4] = {voxelPtr[0], voxelPtr[1], voxelPtr[this->DataDimensions[0] + 1], voxelPtr[this->DataDimensions[0]]};
          int caseIndex = voxelVals[0] > ContourValue; 
          caseIndex |= (voxelVals[1] > ContourValue) << 1;
          caseIndex |= (voxelVals[2] > ContourValue) << 2;
          caseIndex |= (voxelVals[3] > ContourValue) << 3;
          const EDGE_LIST * edge = marchingSquaresCases[caseIndex].edges;
          if (*edge < 0)
            continue;

          const int x = (xBlock + xi) << 1;
          const int y = (yBlock + yi) << 1;

          Polyline2D::Point line[2];

          while(*edge >= 0) {
            static const int edgeToOffset[4][2] = {{1, 0}, {2, 1}, {1, 2}, {0, 1}};
            // point 1
            line[0].xy[0] = x + edgeToOffset[*edge][0];
            line[0].xy[1] = y + edgeToOffset[*edge][1];
            edge++;
            // point 2
            line[1].xy[0] = x + edgeToOffset[*edge][0];
            line[1].xy[1] = y + edgeToOffset[*edge][1];
            edge++;

            // try to add line to polylines
            const int numOfPolylines = polylines.size();
	    int pi;
            for (pi = numOfPolylines - 1; pi >= 0; pi--) {
              if (polylines[pi]->AddNextLine(line))
                break;
              }
            // try to merge polylines
            if (pi >= 0) {
              for (int pj = numOfPolylines - 1; pj >= 0; pj--) {
                if (pi != pj && polylines[pi]->Merge(*polylines[pj])) {
                  delete polylines[pj];
                  polylines[pj] = polylines[numOfPolylines - 1];
                  polylines.pop_back();
                  break;
                  }
                }
              continue;
              }
            polylines.push_back(new Polyline2D(line));
            } // edge loop
            
          } // for (xi)
        } // for (yi)

      }  // for (bxi)
    } // for (byi)

  for (int pi = polylines.size() - 1; pi >= 0 ; pi--) {
    if (polylines[pi]->Length() < 30) {
      delete polylines[pi];
      polylines[pi] = polylines[polylines.size() - 1];
      polylines.pop_back();
      statCounter++;
      }
    else {
      polylines[pi]->Close();
      }
    }

  // eliminate inside polygons
#if 0
  for (pi = 0; pi < polylines.size(); pi++) {
    for (int pj = polylines.size() - 1; pj >= 0; pj--) {
      if (pi != pj && polylines[pi]->IsInsideOf(polylines[pj])) {
        delete polylines[pi];
        polylines[pi] = polylines[polylines.size() - 1];
        polylines.pop_back();
        pi--;
        break;
        }
      }
    }
#endif
  }

///////////////////////////////////////////////////////////////////////////////////
//                             class Polyline2D

//---------------------------------------------------------------------------------
void Polyline2D::Reallocate() {
  const int newsize = 2 * (size + 1);
  Point *buffer = new Point[newsize];
  const int newstart = int(0.5f * size + 1);
  const int newend   = newstart + this->end - this->start;
  memcpy(buffer + newstart, this->vertices + this->start, sizeof(Point) * (this->end - this->start + 1));
  if (this->vertices != this->verticesBuffer)
    delete [] vertices;
  this->vertices = buffer;
  this->start = newstart;
  this->end   = newend;
  this->size = newsize;
  assert(this->start > 1 && (this->end + 2) < newsize);
  }


//---------------------------------------------------------------------------------
void Polyline2D::Allocate(int newsize) {
  if (this->vertices != NULL && (this->end - this->start) > newsize)
    return;
  if (this->vertices != this->verticesBuffer)
    delete [] vertices;
  this->vertices = new Point[newsize];
  this->start = -1;
  this->end   = -1;
  this->size  = newsize;
  this->closestPolyline[0] = this->closestPolyline[1] = -1;
  this->minDistance[0] = this->minDistance[1] = 0;
  }


//---------------------------------------------------------------------------------
Polyline2D::Polyline2D(const Point *line) {
  assert(sizeof(Point) == (2 * sizeof(short)));
  this->size = VERTICES_BUFFER_SIZE;
  this->start = this->end = VERTICES_BUFFER_SIZE >> 1;
  this->end++;
  this->vertices = this->verticesBuffer;
  this->vertices[this->start] = line[0];
  this->vertices[this->end]   = line[1];
  this->closestPolyline[0] = this->closestPolyline[1] = -1;
  this->minDistance[0] = this->minDistance[1] = 0;
  this->updateBoundingBox = true;
  }


//---------------------------------------------------------------------------------
bool Polyline2D::AddNextLine(const Point *newLine) {
  if (newLine[0] == this->vertices[this->start]) {
    if (this->start == 0)
      Reallocate();
    this->vertices[--this->start] = newLine[1];
    }
  else if (newLine[0] == this->vertices[this->end]) {
    if ((this->end + 1) == size)
      Reallocate();
    this->vertices[++this->end] = newLine[1];
    }
  else if (newLine[1] == this->vertices[this->start]) {
    if (this->start == 0)
      Reallocate();
    this->vertices[--this->start] = newLine[0];
    }
  else if (newLine[1] == this->vertices[this->end]) {
    if ((this->end + 1) == size)
      Reallocate();
    this->vertices[++this->end] = newLine[0];
    }
  else {
    return false;
    }
  assert(this->end < this->size && this->start >= 0);
  this->updateBoundingBox = true;
  return true;
  }


//---------------------------------------------------------------------------------
bool Polyline2D::Merge(Polyline2D &polyline) {
  if (polyline.vertices[polyline.start] == this->vertices[start]) {
    while (this->start <= (polyline.end - polyline.start))
      Reallocate();
    for (int i = polyline.start + 1; i <= polyline.end; i++)
      this->vertices[--this->start] = polyline.vertices[i];
    }
  else if (polyline.vertices[polyline.start] == this->vertices[end]) {
    while ((this->size - this->end) <= (polyline.end - polyline.start))
      Reallocate();
    memcpy(this->vertices + end + 1, polyline.vertices + polyline.start + 1, sizeof(Point) * (polyline.end - polyline.start));
    this->end += polyline.end - polyline.start;
    }
  else if (polyline.vertices[polyline.end] == this->vertices[start]) {
    while (this->start <= (polyline.end - polyline.start))
      Reallocate();
    memcpy(this->vertices + start - (polyline.end - polyline.start), 
      polyline.vertices + polyline.start, sizeof (Point) * (polyline.end - polyline.start));
    this->start -= polyline.end - polyline.start;
    }
  else if (polyline.vertices[polyline.end] == this->vertices[end]) {
    while ((this->size - this->end) <= (polyline.end - polyline.start))
      Reallocate();
    for (int i = polyline.end - 1; i >= polyline.start; i--)
      this->vertices[++this->end] = polyline.vertices[i];
    }
  else {
    return false;
    }
  
  this->updateBoundingBox = true;
  assert(this->end < this->size && this->start >= 0);
  return true;
  }


//---------------------------------------------------------------------------------
void Polyline2D::Close() {
  if (this->vertices[this->start] == this->vertices[this->end]) {
    if ((this->end + 1) == this->size)
      this->Reallocate();
    this->vertices[++this->end] = this->vertices[this->start];
    }
  }

//---------------------------------------------------------------------------------
void Polyline2D::UpdateBoundingBox() const {
  this->bbox[0] = this->bbox[2] = VTK_SHORT_MAX;
  this->bbox[1] = this->bbox[3] = VTK_SHORT_MIN;
  for (int i = this->start; i <= this->end; i++) {
    const short * const xy = this->vertices[i].xy;
    if (xy[0] > this->bbox[1])
      this->bbox[1] = xy[0];
    if (xy[0] < this->bbox[0])
      this->bbox[0] = xy[0];
    if (xy[1] > this->bbox[3])
      this->bbox[3] = xy[1];
    if (xy[1] < this->bbox[2])
      this->bbox[2] = xy[1];
    }
  this->updateBoundingBox = false;
  }


/*
//------------------------------------------------------------------------------------------------
void Polyline2D::FindClosestPolyline(int index, int numOfPolylines, Polyline2D* polylines) {
  assert(index == 0 || index == 1);
  
  this->minDistance[index] = VTK_FLOAT_MAX;
  this->closestPolyline[index] = -1;
  
  const int numPoints = (this->end - this->start + 1);
  
  //const float distanceThreshold = 4.f * (this->dataSpacing[0] + this->dataSpacing[1]);
  
  for (int pi = 0; pi < numOfPolylines; pi++) {
    // compare two polylines
    float distance = 0;
    
    if (this->bbox[0] > polylines[pi].bbox[1] || this->bbox[1] < polylines[pi].bbox[0] ||
      this->bbox[2] > polylines[pi].bbox[3] || this->bbox[3] < polylines[pi].bbox[2])
      continue; // bounding boxes do not intersect
    
    for (int i = this->start; i <= this->end; i++) {
      float *iPoint = this->pointArray->GetPoint(this->vertices[i]);
      
      // find the best match
      float localMindistance = VTK_FLOAT_MAX;
      for (int j = polylines[pi].start; j <= polylines[pi].end; j++) {
        float *jPoint = polylines[pi].pointArray->GetPoint(polylines[pi].vertices[j]);
        float locdistance = fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
        if (locdistance < localMindistance)
          localMindistance = locdistance;
        }
      distance += localMindistance;
      }
    distance *= (fabs(numPoints - polylines[pi].end + polylines[pi].start - 1) / numPoints + 1) / numPoints;
    
    if (distance < this->minDistance[index]) {
      this->minDistance[index] = distance;
      this->closestPolyline[index] = pi;
      }
    }
  if (this->closestPolyline[index] == -1)
    this->minDistance[index] = 0;
  }


//----------------------------------------------------------------------------------------------------
int Polyline2D::FindSubPolyline(int numOfPolylines, Polyline2D* polylines, float &minDistance) {
  const int numPoints = (this->end - this->start + 1);
  minDistance = VTK_FLOAT_MAX;
  int bestMatch = -1;
  
  for (int pi = 0; pi < numOfPolylines; pi++) {
    // compare two polylines
    float distance = 0;
    
    if (this->bbox[0] > polylines[pi].bbox[1] || this->bbox[1] < polylines[pi].bbox[0] ||
      this->bbox[2] > polylines[pi].bbox[3] || this->bbox[3] < polylines[pi].bbox[2])
      continue; // bounding boxes do not intersect
    if ((polylines[pi].end - polylines[pi].start) > numPoints)
      continue;
    
    for (int j = polylines[pi].start; j <= polylines[pi].end; j++) {
      float *jPoint = polylines[pi].pointArray->GetPoint(polylines[pi].vertices[j]);
      
      // find the best match
      float localMindistance = VTK_FLOAT_MAX;
      for (int i = this->start; i <= this->end; i++) {  
        float *iPoint = this->pointArray->GetPoint(this->vertices[i]);
        float locdistance = fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
        if (locdistance < localMindistance)
          localMindistance = locdistance;
        }
      distance += localMindistance;
      }
    distance /= (polylines[pi].end - polylines[pi].start);
    if (distance < minDistance) {
      bestMatch = pi;
      minDistance = distance;
      }
    }
  return bestMatch;
  }




//---------------------------------------------------------------------------------
bool Polyline2D::SplitPolyline(Polyline2D& subpoly, Polyline2D& newpoly) {
  const int polyLength = this->end - this->start + 1;
  const int subpolyLength = subpoly.end - subpoly.start + 1;
  
  float avgLineLength = 0;
  for (int si = subpoly.start; si < subpoly.end; si++) {
    float *iPoint = subpoly.pointArray->GetPoint(subpoly.vertices[si]);
    float *jPoint = subpoly.pointArray->GetPoint(subpoly.vertices[si + 1]);
    avgLineLength += fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
    }
  avgLineLength /= (subpoly.end - subpoly.start);
  const float pointDistanceThreshold = 30.f * avgLineLength;
  
  // find the best match between two polylines
  static char *hitBuffer = NULL;
  static int hitBufferLength = 0;
  if (hitBuffer == NULL || hitBufferLength <= (polyLength + 1)) {
    delete [] hitBuffer;
    hitBuffer = new char [polyLength + 1];
    hitBufferLength = polyLength + 1;
    }
  memset(hitBuffer, 0, polyLength + 1);
  
  bool anyHit = false;
  for (si = subpoly.start; si < subpoly.end; si++) {
    float *iPoint = subpoly.pointArray->GetPoint(subpoly.vertices[si + 1]);
    
    // fin best match between points
    float bestDistance = VTK_FLOAT_MAX;
    int bestIndex;
    for (int j = this->start; j <= this->end; j++) {
      float *jPoint = this->pointArray->GetPoint(this->vertices[j]);
      float distance = fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
      if (distance < bestDistance) {
        bestDistance = distance;
        bestIndex = j;
        }
      }
    if (bestDistance < pointDistanceThreshold) {
      hitBuffer[bestIndex - this->start] = 1;
      anyHit = true;
      }
    } // for (i)
  
  if (!anyHit)
    return false;
  
  // analyze the hits
  int bestIndex = -1;
  int bestLength = 0;
  for (int i = 0; i < polyLength; i++) {
    const int startI = i;
    while (hitBuffer[i] == 0) {
      i++;
      if (i >= polyLength)
        i = 0;
      }
    const int length = (i >= startI) ? (i - startI) : (i + polyLength - startI);
    if (length > bestLength) {
      bestLength = length;
      bestIndex = startI;
      }
    if (i < startI)
      break;
    }
  
  // avoid problems with border cases
  if (bestIndex == 0)
    bestIndex++;
  if ((bestIndex + bestLength) == polyLength)
    bestLength--;
  if (bestLength < 40)
    return false; // the subpolyline is too small
  
  // identify borders of polylines
  bool newPolyLineOverlap = ((bestIndex + bestLength) >= polyLength);
  int  newPolyStart = bestIndex;
  int  newPolyEnd =  (bestIndex + bestLength) % polyLength;
  
  // find the narrowest point
  float bestDistance = VTK_FLOAT_MAX;
  int   bestS, bestE;
  for (si = (newPolyStart > 15) ? (newPolyStart - 15) : 0; si < (polyLength - 1) && si < (newPolyStart + 15); si++) {
    float *sPoint = this->pointArray->GetPoint(this->vertices[si + this->start]);
    for (int ei = (newPolyEnd > 15) ? (newPolyEnd - 15) : 0; ei < (polyLength - 1) && ei < (newPolyEnd + 15); ei++) {
      float *ePoint = this->pointArray->GetPoint(this->vertices[ei + this->start]);
      float distance = fabs(sPoint[0] - ePoint[0]) + fabs(sPoint[1] - ePoint[1]);
      if (distance < bestDistance) {
        bestS = si;
        bestE = ei;
        bestDistance = distance;
        }
      }
    }
  newPolyStart = bestS;
  newPolyEnd   = bestE;
  const int newPolyLength = (newPolyEnd >= newPolyStart) ? (newPolyEnd - newPolyStart) : (newPolyEnd + polyLength - newPolyStart);
  
  // test if the polylines are OK
  if (newPolyLength > 1.2 * (polyLength - subpolyLength) ||
    newPolyLength < 0.8 * (polyLength - subpolyLength))
    return false; // deviation is too big
  // check the distance
  const float *sPoint = this->pointArray->GetPoint(this->vertices[this->start + newPolyStart]);
  const float *ePoint = this->pointArray->GetPoint(this->vertices[this->start + newPolyEnd]);
  if ((fabs(sPoint[0] - ePoint[0]) + fabs(sPoint[1] - ePoint[1])) > pointDistanceThreshold)
    return false;
  
  // create new polyline
  newpoly.Allocate(newPolyLength + 3);
  if (newPolyLineOverlap) {
    memcpy(newpoly.vertices, this->vertices + this->start + newPolyStart, sizeof (vtkIdType) * (polyLength - newPolyStart));
    memcpy(newpoly.vertices + polyLength - newPolyStart, this->vertices + this->start, sizeof (vtkIdType) * (newPolyEnd + 1));
    }
  else {
    memcpy(newpoly.vertices, this->vertices + this->start + newPolyStart, sizeof (vtkIdType) * (newPolyLength + 1));
    }
  newpoly.start = 1;
  newpoly.end   = newPolyLength;
  newpoly.vertices[newpoly.end] = newpoly.vertices[newpoly.start];
  newpoly.SetPointArray(this->pointArray);
  newpoly.UpdateBoundingBox();
  
  // modify the original
  if (newPolyLineOverlap) {
    this->end    = this->start + newPolyStart - 1;
    this->start += newPolyEnd + 1;
    this->vertices[this->end] = this->vertices[this->start];
    }
  else {
    memcpy(this->vertices + this->start + newPolyStart - 1, this->vertices + this->start + newPolyEnd, sizeof (vtkIdType) * (polyLength - newPolyEnd));
    this->end -= (newPolyLength + 1);
    }
  
  this->UpdateBoundingBox();
  
  return true;
  }
*/
  
void Polyline2D::Move(Polyline2D &polyline) {
  if (this->vertices != this->verticesBuffer)
    delete [] vertices;
  *this = polyline; // copy all members
  
  this->updateBoundingBox = true;
  
  // reset the source
  polyline.vertices = NULL;
  }


//------------------------------------------------------------
// check if one polyline is inside another       
bool Polyline2D::IsInsideOf(const Polyline2D *outerPolyline) const {
  if (this->updateBoundingBox)
    this->UpdateBoundingBox();
  if (outerPolyline->updateBoundingBox)
    outerPolyline->UpdateBoundingBox();

  // check bounding boxes
  if (outerPolyline->bbox[0] > this->bbox[1] ||
      outerPolyline->bbox[2] > this->bbox[3] ||
      outerPolyline->bbox[1] < this->bbox[0] ||
      outerPolyline->bbox[3] < this->bbox[2] || outerPolyline->Length() < 16)
    return false;

  // get sample point
  int intersection = 0;
  const short sx = this->vertices[this->start].xy[0];
  const short sy = this->vertices[this->start].xy[1];

  for (int p = outerPolyline->start + 1; p < outerPolyline->end; p++) {
    const short pointX   = outerPolyline->vertices[p].xy[0];
    const short pointY   = outerPolyline->vertices[p].xy[1];
    if (pointY != sy || pointX <= sx)
      continue;
    const short prevPointY   = outerPolyline->vertices[p - 1].xy[1];
    p++;
    while ((p < outerPolyline->end) && (outerPolyline->vertices[p].xy[1] == pointY))
      p++; // skip parallel lines
    if (p < outerPolyline->end && outerPolyline->vertices[p].xy[1] != prevPointY)
      intersection++;
    }

  // check
  if ((intersection % 2) == 1 && this->Length() > 32) {
    intersection = 0;
    
    for (int p = outerPolyline->start + 1; p < outerPolyline->end; p++) {
      const short pointX   = outerPolyline->vertices[p].xy[0];
      const short pointY   = outerPolyline->vertices[p].xy[1];
      if (pointY != sy || pointX >= sx)
        continue;
      const short prevPointY   = outerPolyline->vertices[p - 1].xy[1];
      p++;
      while ((p < outerPolyline->end) && (outerPolyline->vertices[p].xy[1] == pointY))
        p++; // skip parallel lines
      if (p < outerPolyline->end && outerPolyline->vertices[p].xy[1] != prevPointY)
        intersection++;
      }
    }
    
  return ((intersection % 2) == 1);
  }


void ListOfPolyline2D::clear() {
  for (int pj = size() - 1; pj >= 0; pj--)
    delete at(pj);
  erase(begin(), end());
  }
    
bool ListOfPolyline2D::IsInside(int x, int y, int polylineLengthThreshold) {
  const short sx = x << 1;
  const short sy = y << 1;

  for (int pi = this->size() - 1; pi >= 0; pi--) {
    const Polyline2D * const polyline = at(pi);
    if (polyline->Length() < polylineLengthThreshold)
      continue;

    if (polyline->updateBoundingBox)
      polyline->UpdateBoundingBox();
    
    // check bounding boxes
    if (polyline->bbox[0] > sx || polyline->bbox[1] < sx ||
        polyline->bbox[2] > sy || polyline->bbox[3] < sy)
      continue;

    int intersection = 0;
    for (int p = polyline->start + 1; p < polyline->end; p++) {
      const short pointX   = polyline->vertices[p].xy[0];
      const short pointY   = polyline->vertices[p].xy[1];
      if (pointY != sy || pointX <= sx)
        continue;
      const short prevPointY   = polyline->vertices[p - 1].xy[1];
      p++;
      while ((p < polyline->end) && (polyline->vertices[p].xy[1] == pointY))
        p++; // skip parallel lines
      if (p < polyline->end && polyline->vertices[p].xy[1] != prevPointY)
        intersection++;
      }
    
    // check
    if ((intersection % 2) == 1) {
      intersection = 0;
      for (int p = polyline->start + 1; p < polyline->end; p++) {
        const short pointX   = polyline->vertices[p].xy[0];
        const short pointY   = polyline->vertices[p].xy[1];
        if (pointY != sy || pointX >= sx)
          continue;
        const short prevPointY   = polyline->vertices[p - 1].xy[1];
        p++;
        while ((p < polyline->end) && (polyline->vertices[p].xy[1] == pointY))
          p++; // skip parallel lines
        if (p < polyline->end && polyline->vertices[p].xy[1] != prevPointY)
          intersection++;
        }
      }
    
    if ((intersection % 2) == 1) {
      // test that the point is inside, not on the contour
      int p;
      for (p = polyline->start + 1; p < polyline->end; p++) {
        const short pointX   = polyline->vertices[p].xy[0];
        const short pointY   = polyline->vertices[p].xy[1];

        if (abs(pointX - sx) <= 4 && abs(pointY - sy) <= 4)
          break;
        }

      if (p == polyline->end)
        return true;
      }
    }

  return false;
  }


Polyline2D *ListOfPolyline2D::FindContour(int x, int y, int polylineLengthThreshold, int distance) {
  const short sx = x << 1;
  const short sy = y << 1;
  distance = distance << 1;

  for (int pi = this->size() - 1; pi >= 0; pi--) {
    Polyline2D *polyline = at(pi);
    if (polyline->Length() < polylineLengthThreshold)
      continue;

    if (polyline->updateBoundingBox)
      polyline->UpdateBoundingBox();
    
    // check bounding boxes
    if (polyline->bbox[0] > (sx + distance) || polyline->bbox[1] < (sx - distance) ||
        polyline->bbox[2] > (sy + distance) || polyline->bbox[3] < (sy - distance))
      continue;

    for (int p = polyline->start + 1; p < polyline->end; p++) {
      const short pointX   = polyline->vertices[p].xy[0];
      const short pointY   = polyline->vertices[p].xy[1];

      if (abs(pointX - sx) <= distance && abs(pointY - sy) <= distance)
        return polyline;
      }
    }

  return NULL;
  }