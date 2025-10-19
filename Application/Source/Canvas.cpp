
#include "Canvas.h"
#include "App.h"
#include "Frame.h"
#include "Tree.h"
#include <gl/GLU.h>
#include <GL/glut.h>
#include <math.h>

static int attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

Canvas::Canvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &Canvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &Canvas::OnSize, this);

	this->worldRect.minCorner.SetComponents(-10.0, -15.0);
	this->worldRect.maxCorner.SetComponents(10.0, 5.0);
}

/*virtual*/ Canvas::~Canvas()
{
	delete this->renderContext;
}

void Canvas::OnPaint(wxPaintEvent& event)
{
	this->SetCurrent(*this->renderContext);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float aspectRatio = float(viewport[2]) / float(viewport[3]);

	HappyMath::Rectangle expandedWorldRect(this->worldRect);
	expandedWorldRect.ExpandToMatchAspectRatio(aspectRatio);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(expandedWorldRect.minCorner.x, expandedWorldRect.maxCorner.x, expandedWorldRect.minCorner.y, expandedWorldRect.maxCorner.y);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Tree* tree = wxGetApp().GetTree();
	if (tree)
	{
		tree->Layout();
		tree->Render();
	}

	glFlush();

	this->SwapBuffers();
}

void Canvas::OnSize(wxSizeEvent& event)
{
	this->SetCurrent(*this->renderContext);

	wxSize size = event.GetSize();
	glViewport(0, 0, size.GetWidth(), size.GetHeight());

	this->Refresh();
}