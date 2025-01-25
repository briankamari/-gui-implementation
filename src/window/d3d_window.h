#pragma once

#include "appdefs.h"

struct d3d_window {

    d3d_window():m_hwnd(NULL){}

    HWND m_hwnd;

    bool init();
    bool update();

    static d3d_window* __window;

    static LRESULT CALLBACK winproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

};
