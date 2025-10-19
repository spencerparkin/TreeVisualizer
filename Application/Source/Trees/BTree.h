#pragma once

#include "../Tree.h"

/**
 * 
 */
class BTree : public Tree, public std::enable_shared_from_this<BTree>
{
	friend class BTreeNode;

public:
	BTree(int minDegree);
	virtual ~BTree();

	virtual bool InsertKey(std::shared_ptr<Key> givenKey) override;
	virtual bool RemoveKey(std::shared_ptr<Key> givenKey) override;
	virtual bool FindKey(std::shared_ptr<Key> givenKey, std::shared_ptr<Key>& foundKey) const override;

	int GetMinDegree() const;
	int GetMaxDegree() const;

private:
	int minDegree;
};

/**
 * 
 */
class BTreeNode : public Tree::Node, public std::enable_shared_from_this<BTreeNode>
{
	friend class BTree;

public:
	BTreeNode(std::shared_ptr<BTree> tree);
	virtual ~BTreeNode();

	virtual void Render() override;
	virtual void Translate(const HappyMath::Vector2& translation) override;
	virtual void CalcBoundingRect() override;
	virtual void GetChildren(std::vector<Tree::Node*>& givenChildNodeArray) override;
	virtual void GetKeys(std::vector<Tree::Key*>& givenKeyArray) override;

	bool IsLeaf() const;
	bool IsInternal() const;
	bool IsRoot() const;
	bool IsFull() const;
	bool FindKey(std::shared_ptr<Tree::Key> givenKey, std::shared_ptr<BTreeNode>& foundKey, BTreeNode** foundNode = nullptr);
	bool FindKeyIndex(Tree::Key* givenKey, int& i);
	bool FindChildIndex(BTreeNode* givenNode, int& i);
	bool FindChildOrKeyInsertionIndex(Tree::Key* givenKey, int& i);
	bool Split();

private:
	std::string MakeLabel();

	HappyMath::Rectangle keyRect;
	double renderScale;
	std::vector<std::shared_ptr<BTreeNode>> childNodeArray;
	std::vector<std::shared_ptr<Tree::Key>> keyArray;
	std::weak_ptr<BTreeNode> parentNodeWeakPtr;
	std::weak_ptr<BTree> treeWeakPtr;
};