#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:
    virtual void init() = 0;
    virtual void destroy() = 0;
    virtual void update(float) = 0;
    virtual void render() = 0;
    virtual void resize(int, int) = 0;
};

#endif // SCENE_H
