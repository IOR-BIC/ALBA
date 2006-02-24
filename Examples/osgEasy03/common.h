// ===================================================
// Esempi di uso di OSG :
// ===================================================
#ifndef __common_h__
#define __common_h__

#include <osgProducer/Viewer>
#include <osg/Node>

// il viewer
extern osgProducer::Viewer viewer;

// da chiamare appena parte il programma
void InitViewer();

// collega la scena al viewer
void SetSceneGraph(osg::Node *root);

// inizia il rendering loop
void Loop();

#endif 