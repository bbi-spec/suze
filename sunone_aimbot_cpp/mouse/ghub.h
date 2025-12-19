#ifndef GHUB_H
#define GHUB_H

#include <windows.h>

class GhubMouse
{
public:
    GhubMouse();
    ~GhubMouse();

    // Core movement/click functions
    bool mouse_xy(int x, int y);
    bool mouse_down(int key = 1);
    bool mouse_up(int key = 1);
    bool mouse_close();

private:
    // We only need this flag to track if the socket is ready
    bool gmok;
};

#endif // GHUB_H
