#pragma once

#include <wx/app.h>

class Frame;
class Tree;

/**
 * 
 */
class App : public wxApp
{
public:
	App();
	virtual ~App();

	virtual bool OnInit() override;
	virtual int OnExit() override;

	Frame* GetFrame();
	Tree* GetTree();

private:
	Frame* frame;

	std::shared_ptr<Tree> tree;
};

wxDECLARE_APP(App);