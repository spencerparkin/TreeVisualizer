#include "Tree.h"
#include <wx/glcanvas.h>
#include <format>

using namespace HappyMath;

//---------------------------------- Tree ----------------------------------

Tree::Tree()
{
	this->numKeys = 0;
}

/*virtual*/ Tree::~Tree()
{
}

void Tree::Layout()
{
	if (this->rootNode.get())
		this->rootNode->Layout();
}

void Tree::Render()
{
	if (this->rootNode.get())
	{
		glBegin(GL_LINES);
		glColor3d(0.0, 0.0, 0.0);
		this->rootNode->RenderBranches();
		glEnd();

		this->rootNode->RenderSubtree();
	}
}

/*virtual*/ bool Tree::SanityCheck()
{
	if (this->rootNode.get())
		return this->rootNode->SanityCheck();

	return true;
}

//---------------------------------- Tree::Node ----------------------------------

Tree::Node::Node()
{
}

/*virtual*/ Tree::Node::~Node()
{
}

/*virtual*/ void Tree::Node::Layout()
{
	this->CalcBoundingRect();

	this->subTreeBoundingRect = this->boundingRect;

	std::vector<Node*> childNodeArray;
	this->GetChildren(childNodeArray);

	if (childNodeArray.size() == 0)
		return;
	
	for (Node* childNode : childNodeArray)
		childNode->Layout();

	double totalWidth = 0.0;
	for (Node* childNode : childNodeArray)
		totalWidth += childNode->subTreeBoundingRect.GetWidth();

	Vector2 center = this->boundingRect.GetCenter();

	Vector2 location;
	location.x = center.x - totalWidth / 2.0;
	location.y = center.y - this->boundingRect.GetHeight() / 2.0;

	for (Node* childNode : childNodeArray)
	{
		Vector2 childLocation = location;
		childLocation.y -= childNode->subTreeBoundingRect.GetHeight() / 2.0;
		childLocation.x += childNode->subTreeBoundingRect.GetWidth() / 2.0;

		Vector2 translation = childLocation - childNode->subTreeBoundingRect.GetCenter();
		childNode->TranslateSubtree(translation);

		location.x += childNode->subTreeBoundingRect.GetWidth();
	}

	for (Node* childNode : childNodeArray)
		this->subTreeBoundingRect.ExpandToIncludeRect(childNode->subTreeBoundingRect);
}

/*virtual*/ void Tree::Node::RenderBranches()
{
	Vector2 parentCenter = this->boundingRect.GetCenter();

	std::vector<Node*> childNodeArray;
	this->GetChildren(childNodeArray);

	for (Node* childNode : childNodeArray)
	{
		Vector2 childCenter = childNode->boundingRect.GetCenter();

		glVertex2d(parentCenter.x, parentCenter.y);
		glVertex2d(childCenter.x, childCenter.y);

		childNode->RenderBranches();
	}
}

/*virtual*/ void Tree::Node::RenderSubtree()
{
	this->Render();

	std::vector<Node*> childNodeArray;
	this->GetChildren(childNodeArray);

	for (Node* childNode : childNodeArray)
		childNode->RenderSubtree();
}

/*virtual*/ void Tree::Node::Translate(const HappyMath::Vector2& translation)
{
	this->boundingRect += translation;
	this->subTreeBoundingRect += translation;
}

/*virtual*/ void Tree::Node::TranslateSubtree(const HappyMath::Vector2& translation)
{
	this->Translate(translation);

	std::vector<Node*> childNodeArray;
	this->GetChildren(childNodeArray);

	for (Node* childNode : childNodeArray)
		childNode->TranslateSubtree(translation);
}

/*virtual*/ bool Tree::Node::SanityCheck()
{
	std::vector<Node*> childNodeArray;
	this->GetChildren(childNodeArray);

	for (Node* childNode : childNodeArray)
		if (!childNode->SanityCheck())
			return false;

	return true;
}

//---------------------------------- Tree::Key ----------------------------------

Tree::Key::Key()
{
}

/*virtual*/ Tree::Key::~Key()
{
}

//---------------------------------- Tree::NumberKey ----------------------------------

Tree::NumberKey::NumberKey(int number)
{
	this->number = number;
}

/*virtual*/ Tree::NumberKey::~NumberKey()
{
}

/*virtual*/ bool Tree::NumberKey::IsLessThan(const Key* key) const
{
	return this->number < static_cast<const NumberKey*>(key)->number;
}

/*virtual*/ bool Tree::NumberKey::IsGreaterThan(const Key* key) const
{
	return this->number > static_cast<const NumberKey*>(key)->number;
}

/*virtual*/ bool Tree::NumberKey::IsEqualTo(const Key* key) const
{
	return this->number == static_cast<const NumberKey*>(key)->number;
}

/*virtual*/ bool Tree::NumberKey::ToString(std::string& givenString) const
{
	givenString = std::format("{}", this->number);
	return true;
}

/*virtual*/ bool Tree::NumberKey::FromString(const std::string& givenString)
{
	// STPTODO: Write this.
	return false;
}