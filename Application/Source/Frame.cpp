#include "Frame.h"
#include "Canvas.h"
#include "App.h"
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/aboutdlg.h>

Frame::Frame(const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, wxT("Clue"), pos, size)
{
	wxMenu* programMenu = new wxMenu();
	programMenu->Append(new wxMenuItem(programMenu, ID_Exit, "Exit", "Close this program."));

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(new wxMenuItem(helpMenu, ID_About, "About", "Show the about box."));

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(programMenu, "Program");
	menuBar->Append(helpMenu, "Help");
	this->SetMenuBar(menuBar);

	this->Bind(wxEVT_MENU, &Frame::OnExit, this, ID_Exit);
	this->Bind(wxEVT_MENU, &Frame::OnAbout, this, ID_About);

	this->SetStatusBar(new wxStatusBar(this));

	this->canvas = new Canvas(this);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(this->canvas, 1, wxALL | wxGROW, 0);
	this->SetSizer(mainSizer);
}

/*virtual*/ Frame::~Frame()
{
}

Canvas* Frame::GetCanvas()
{
	return this->canvas;
}

void Frame::OnExit(wxCommandEvent& event)
{
	this->Close(true);
}

void Frame::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo aboutDialogInfo;

	aboutDialogInfo.SetName("Tree Visualizer");
	aboutDialogInfo.SetDescription("Visualize and animate tree-based data-structures.");

	wxAboutBox(aboutDialogInfo);
}

void Frame::OnUpdateUI(wxUpdateUIEvent& event)
{
}