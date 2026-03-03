#include "MouseHandler.h"
#include "player/input/ITurnInput.h"

#ifdef RPI
#include <SDL/SDL.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

MouseHandler::MouseHandler( ITurnInput* turnInput )
:	_turnInput(turnInput)
{}

MouseHandler::MouseHandler()
:	_turnInput(0)
{}

MouseHandler::~MouseHandler() {
}

void MouseHandler::setTurnInput( ITurnInput* turnInput ) {
	_turnInput = turnInput;
}

void MouseHandler::grab(bool hideCursor) {
	xd = 0;
	yd = 0;

#if defined(RPI)
	//LOGI("Grabbing input!\n");
	SDL_WM_GrabInput(SDL_GRAB_ON);
	if (hideCursor) {
		SDL_ShowCursor(0);
	}
#elif defined(WIN32)
	// Get the current window
	HWND hWnd = FindWindow(NULL, "Minecraft PE");
	if (!hWnd) {
		// If not found by title, try to get the foreground window
		hWnd = GetForegroundWindow();
	}
	if (hWnd) {
		// Hide cursor if requested
		if (hideCursor) {
			while (ShowCursor(FALSE) >= 0);
		}
		// Clip cursor to window
		RECT rect;
		if (GetWindowRect(hWnd, &rect)) {
			ClipCursor(&rect);
		}
	}
#endif
}

void MouseHandler::release() {
#if defined(RPI)
	//LOGI("Releasing input!\n");
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);
#elif defined(WIN32)
	// Show cursor
	while (ShowCursor(TRUE) < 0);
	// Release cursor clipping
	ClipCursor(NULL);
#endif
}

void MouseHandler::poll() {
	if (_turnInput != 0) {
		TurnDelta td = _turnInput->getTurnDelta();
		xd = td.x;
		yd = td.y;
	}
}
