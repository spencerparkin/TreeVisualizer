#include "App.h"
#include "Frame.h"
#include "Tree.h"

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
	
	auto nodeA = std::make_shared<Tree::Node>();

	auto nodeB = std::make_shared<Tree::Node>();
	auto nodeC = std::make_shared<Tree::Node>();

	auto nodeD = std::make_shared<Tree::Node>();
	auto nodeE = std::make_shared<Tree::Node>();

	auto nodeF = std::make_shared<Tree::Node>();
	auto nodeG = std::make_shared<Tree::Node>();

	nodeA->childNodeArray.push_back(nodeB);
	nodeA->childNodeArray.push_back(nodeC);

	nodeB->childNodeArray.push_back(nodeD);
	nodeB->childNodeArray.push_back(nodeE);

	nodeC->childNodeArray.push_back(nodeF);
	nodeC->childNodeArray.push_back(nodeG);

	this->tree = std::make_shared<Tree>();
	this->tree->rootNode = nodeA;

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