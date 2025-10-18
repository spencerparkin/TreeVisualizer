#pragma once

#include <vector>
#include <memory>
#include "HappyMath/Rectangle.h"

/**
 * This is the base class for any kind of tree we'd like to visualize.
 */
class Tree
{
public:
	Tree();
	virtual ~Tree();

	virtual void Layout();
	virtual void Render();

	/**
	 * This is the base class for any kind of node in the tree.
	 */
	class Node
	{
	public:
		Node();
		virtual ~Node();

		virtual void Layout();
		virtual void Render();
		virtual void RenderSubtree();
		virtual void Translate(const HappyMath::Vector2& translation);
		virtual void CalcBoundingRect();

	protected:
		HappyMath::Rectangle boundingRect;
		HappyMath::Rectangle subTreeBoundingRect;
		std::vector<std::shared_ptr<Node>> childNodeArray;
		std::weak_ptr<Node> parentNode;
	};

protected:
	std::shared_ptr<Node> rootNode;
};