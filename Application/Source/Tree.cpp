#include "Tree.h"
#include <wx/glcanvas.h>

using namespace HappyMath;

//---------------------------------- Tree ----------------------------------

Tree::Tree()
{
}

/*virtual*/ Tree::~Tree()
{
}

/*virtual*/ void Tree::Layout()
{
	if (this->rootNode.get())
		this->rootNode->Layout();
}

/*virtual*/ void Tree::Render()
{
	if (this->rootNode.get())
		this->rootNode->Render();
}

//---------------------------------- Tree::Node ----------------------------------

Tree::Node::Node()
{
}

/*virtual*/ Tree::Node::~Node()
{
}

/*virtual*/ void Tree::Node::CalcBoundingRect()
{
	this->boundingRect.minCorner.SetComponents(-0.5, -0.5);
	this->boundingRect.maxCorner.SetComponents(0.5, 0.5);
}

/*virtual*/ void Tree::Node::Layout()
{
	this->CalcBoundingRect();

	if (this->childNodeArray.size() == 0)
	{
		this->subTreeBoundingRect = this->boundingRect;
		return;
	}
	
	for (std::shared_ptr<Node>& childNode : this->childNodeArray)
		childNode->Layout();

	double totalWidth = 0.0;
	for (std::shared_ptr<Node>& childNode : this->childNodeArray)
		totalWidth += childNode->subTreeBoundingRect.GetWidth();

	Vector2 center = this->boundingRect.GetCenter();

	Vector2 location;
	location.x = center.x - totalWidth / 2.0;
	location.y = center.y + this->boundingRect.GetHeight() / 2.0;

	for (std::shared_ptr<Node>& childNode : this->childNodeArray)
	{
		Vector2 childLocation = location;
		childLocation.y += childNode->subTreeBoundingRect.GetHeight() / 2.0;
		childLocation.x += childNode->subTreeBoundingRect.GetWidth() / 2.0;

		Vector2 translation = childLocation - childNode->subTreeBoundingRect.GetCenter();
		childNode->Translate(translation);

		location.x += childNode->subTreeBoundingRect.GetWidth();
	}

	this->subTreeBoundingRect.MakeInvalid();
}

/*virtual*/ void Tree::Node::Render()
{
	glBegin(GL_LINE_LOOP);
	glColor3d(0.0, 0.0, 0.0);

	glVertex2d(this->boundingRect.minCorner.x, this->boundingRect.minCorner.y);
	glVertex2d(this->boundingRect.maxCorner.x, this->boundingRect.minCorner.y);
	glVertex2d(this->boundingRect.maxCorner.x, this->boundingRect.maxCorner.y);
	glVertex2d(this->boundingRect.minCorner.x, this->boundingRect.maxCorner.y);

	glEnd();
}

/*virtual*/ void Tree::Node::RenderSubtree()
{
	this->Render();

	for (std::shared_ptr<Node>& childNode : this->childNodeArray)
		childNode->Render();
}

/*virtual*/ void Tree::Node::Translate(const HappyMath::Vector2& translation)
{
	this->boundingRect += translation;
	this->subTreeBoundingRect += translation;
}