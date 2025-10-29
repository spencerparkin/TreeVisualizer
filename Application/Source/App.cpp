#include "App.h"
#include "Frame.h"
#include "Tree.h"
#include "Trees/BTree.h"
#include <GL/glut.h>

wxIMPLEMENT_APP(App);

App::App()
{
	this->frame = nullptr;
}

/*virtual*/ App::~App()
{
}

/*virtual*/ bool App::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	int fakeArgc = 1;
	char* fakeArgv[] = { (char*)"App" };
	glutInit(&fakeArgc, fakeArgv);

	this->tree = std::make_shared<BTree>(3);

	this->frame = new Frame(wxDefaultPosition, wxSize(2400, 1600));
	this->frame->Show();

	return true;
}

/*virtual*/ int App::OnExit()
{
	return 0;
}

Frame* App::GetFrame()
{
	return this->frame;
}

Tree* App::GetTree()
{
	return this->tree.get();
}