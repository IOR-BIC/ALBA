#include "common.h"

osgProducer::Viewer viewer;

// -----------------------------------------
// da chiamare appena parte il programma
void InitViewer()
// -----------------------------------------
{
  viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
}

// -----------------------------------------
// collega la scena al viewer
void SetSceneGraph(osg::Node *root)
// -----------------------------------------
{
  viewer.setSceneData( root );
}

// -----------------------------------------
// inizia il rendering loop
void Loop()
// -----------------------------------------
{
  // create the windows and run the threads.
  viewer.realize();

  while( !viewer.done() )
  {
    // wait for all cull and draw threads to complete.
    viewer.sync();

    // update the scene by traversing it with the the update visitor which will
    // call all node update callbacks and animations.
    viewer.update();

    // fire off the cull and draw traversals of the scene.
    viewer.frame();

  }
  // wait for all cull and draw threads to complete before exit.
  viewer.sync();
}

