#pragma once

#include <wx/glcanvas.h>
#include "HappyMath/Rectangle.h"

class Canvas : public wxGLCanvas
{
public:
	Canvas(wxWindow* parent);
	virtual ~Canvas();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);

private:

	HappyMath::Rectangle worldRect;
	wxGLContext* renderContext;
};