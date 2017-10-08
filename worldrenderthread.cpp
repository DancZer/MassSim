#include <QSize>
#include <cmath>

#include "worldframe.h"
#include "worldrenderthread.h"

WorldRenderThread::WorldRenderThread(WorldFrame* worldFrame) :
    QThread(),
    mWorldFrame(worldFrame)
{
    mDoRendering = true;
    mDoResize = false;
}

void WorldRenderThread::resizeViewport(void)
{
    mDoResize = true;
}

void WorldRenderThread::stop()
{
    mDoRendering = false;
}

void WorldRenderThread::run()
{
    mWorldFrame->makeCurrent();
    mWorldFrame->glInit();

    while (mDoRendering){
        if (mDoResize){
            mWorldFrame->glResize();
            mDoResize = false;
        }
        mWorldFrame->glPaint();
        mWorldFrame->swapBuffers();

        for (int i = 0, updateCount = mWorldFrame->getSimUpdateCountPerFrame(); i < updateCount; ++i) {
            mWorldFrame->updateWorld();
        }
        mWorldFrame->printWorldInfo();

        msleep(16); // wait 16ms => about 60 FPS
    }
}
