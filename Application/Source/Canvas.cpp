
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
	this->Bind(wxEVT_KEY_UP, &Canvas::OnKeyUp, this);

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
		tree->Render();

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

void Canvas::OnKeyUp(wxKeyEvent& event)
{
	Tree* tree = wxGetApp().GetTree();
	if (!tree)
		return;

	switch (event.GetKeyCode())
	{
		case wxKeyCode::WXK_SPACE:
		{
			if (event.ShiftDown())
			{
				if (this->keyArray.size() > 0)
				{
					double t = double(::rand()) / double(RAND_MAX);
					int i = int(t * double(this->keyArray.size() - 1));
					
					if (i < 0)
						i = 0;
					if (i >= (int)this->keyArray.size())
						i = (int)this->keyArray.size() - 1;

					int j = this->keyArray[i];
					std::shared_ptr<Tree::Key> key = std::make_shared<Tree::NumberKey>(j);
					
					bool removed = tree->RemoveKey(key);
					wxASSERT(removed);
					if (!removed)
					{
						int b = 0;
						b++;
					}

					this->keyArray.erase(this->keyArray.begin() + i);
				}
			}
			else
			{
				if (this->keyArray.size() < 100)
				{
					std::shared_ptr<Tree::Key> key, foundKey;
					int i = 0;

					do
					{
						double t = double(::rand()) / double(RAND_MAX);
						i = int(t * 100.0);
						key = std::make_shared<Tree::NumberKey>(i);
					} while (tree->FindKey(key, foundKey));

					bool inserted = tree->InsertKey(key);
					wxASSERT(inserted);

					this->keyArray.push_back(i);
				}
			}

			break;
		}
	}

	tree->Layout();

	this->Refresh();
}