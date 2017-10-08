#ifndef WORLDRENDERTHREAD_H
#define WORLDRENDERTHREAD_H

#include <QThread>

class WorldFrame;
class QSize;

class WorldRenderThread : public QThread
{
    Q_OBJECT
public:
    explicit WorldRenderThread(WorldFrame* worldFrame);
    
    void resizeViewport(void);

    void stop(void);

protected:
    void run(void);

private:
    bool mDoRendering, mDoResize;

    WorldFrame* mWorldFrame;
};

#endif // WORLDRENDERTHREAD_H
