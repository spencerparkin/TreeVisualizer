#pragma once

#include <wx/app.h>

class Frame;

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

private:
	Frame* frame;
};

wxDECLARE_APP(App);