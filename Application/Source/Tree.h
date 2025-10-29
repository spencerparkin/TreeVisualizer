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

	void Layout();
	void Render();

	class Key;

	virtual bool InsertKey(std::shared_ptr<Key> givenKey) = 0;
	virtual bool RemoveKey(std::shared_ptr<Key> givenKey) = 0;
	virtual bool FindKey(std::shared_ptr<Key> givenKey, std::shared_ptr<Key>& foundKey) const = 0;

	/**
	 * This is the base class for any kind of node in the tree.
	 */
	class Node
	{
	public:
		Node();
		virtual ~Node();

		void Layout();
		void TranslateSubtree(const HappyMath::Vector2& translation);
		void RenderBranches();
		void RenderSubtree();

		virtual void Render() = 0;
		virtual void Translate(const HappyMath::Vector2& translation);
		virtual void CalcBoundingRect() = 0;
		virtual void GetChildren(std::vector<Node*>& givenChildNodeArray) = 0;
		virtual void GetKeys(std::vector<Key*>& givenKeyArray) = 0;

	protected:
		HappyMath::Rectangle boundingRect;
		HappyMath::Rectangle subTreeBoundingRect;
	};
	
	/**
	 * This is the base class for any kind of key in the tree.  If there was a value associated
	 * with the key, then a class deriving from this one would own it.  The derived class also
	 * owns what constitutes the key (e.g., a string, number, etc.)
	 */
	class Key
	{
	public:
		Key();
		virtual ~Key();

		virtual bool IsLessThan(const Key* key) const = 0;
		virtual bool IsGreaterThan(const Key* key) const = 0;
		virtual bool IsEqualTo(const Key* key) const = 0;
		virtual bool ToString(std::string& givenString) const = 0;
		virtual bool FromString(const std::string& givenString) = 0;
	};

	/**
	 * This is a simple number-based key that any tree type can use.
	 */
	class NumberKey : public Key
	{
	public:
		NumberKey(int number);
		virtual ~NumberKey();

		virtual bool IsLessThan(const Key* key) const override;
		virtual bool IsGreaterThan(const Key* key) const override;
		virtual bool IsEqualTo(const Key* key) const override;
		virtual bool ToString(std::string& givenString) const override;
		virtual bool FromString(const std::string& givenString) override;

	private:
		int number;
	};

protected:
	std::shared_ptr<Node> rootNode;
	int numKeys;
};