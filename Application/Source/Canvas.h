#pragma once

#include <wx/glcanvas.h>
#include <vector>
#include "HappyMath/Rectangle.h"

class Canvas : public wxGLCanvas
{
public:
	Canvas(wxWindow* parent);
	virtual ~Canvas();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnKeyUp(wxKeyEvent& event);

private:

	std::vector<int> keyArray;
	HappyMath::Rectangle worldRect;
	wxGLContext* renderContext;
};