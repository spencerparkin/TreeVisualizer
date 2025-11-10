#include "BTree.h"
#include <wx/glcanvas.h>
#include <assert.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <format>

//------------------------------------- BTree -------------------------------------

BTree::BTree(int minDegree)
{
	this->minDegree = minDegree;
}

/*virtual*/ BTree::~BTree()
{
}

/*virtual*/ bool BTree::InsertKey(std::shared_ptr<Key> givenKey)
{
	if (!this->rootNode.get())
	{
		std::shared_ptr<BTreeNode> newNode = std::make_shared<BTreeNode>(this->shared_from_this());
		newNode->keyArray.push_back(givenKey);
		this->rootNode = newNode;
		this->numKeys++;
		return true;
	}

	BTreeNode* node = static_cast<BTreeNode*>(this->rootNode.get());

	while (true)
	{
		if (node->IsFull())
		{
			bool nodeSplit = node->Split();
			assert(nodeSplit);
			node = node->parentNodeWeakPtr.lock().get();
			assert(node != nullptr);
		}

		int i = 0;
		if (node->FindKeyIndex(givenKey.get(), i))
			return false;

		bool indexFound = node->FindChildOrKeyInsertionIndex(givenKey.get(), i);
		assert(indexFound);

		if (node->IsLeaf())
		{
			node->keyArray.insert(node->keyArray.begin() + i, givenKey);
			break;
		}

		node = node->childNodeArray[i].get();
		assert(node != nullptr);
	}

	this->numKeys++;
	return true;
}

/*virtual*/ bool BTree::RemoveKey(std::shared_ptr<Key> givenKey)
{
	BTreeNode* node = static_cast<BTreeNode*>(this->rootNode.get());

	while (true)
	{
		int i = -1;
		if (node->FindKeyIndex(givenKey.get(), i))
		{
			if (node->IsLeaf())
			{
				node->keyArray.erase(node->keyArray.begin() + i);
				return true;
			}

			std::shared_ptr<BTreeNode> nodeA = node->childNodeArray[i];
			std::shared_ptr<BTreeNode> nodeB = node->childNodeArray[i + 1];

			if (nodeA->keyArray.size() == this->minDegree - 1 && nodeB->keyArray.size() == this->minDegree - 1)
			{
				node->keyArray.erase(node->keyArray.begin() + i);
				node->childNodeArray.erase(node->childNodeArray.begin() + (i + 1));

				for (int j = 0; j < (int)nodeB->keyArray.size(); j++)
					nodeA->keyArray.push_back(nodeB->keyArray[j]);

				for (int j = 0; j < (int)nodeB->childNodeArray.size(); j++)
					nodeA->childNodeArray.push_back(nodeB->childNodeArray[j]);

				return true;
			}
			else
			{
				// At this point, I'm not sure if it matters if we go delete
				// the predecessor or the successor.  Either one would be fine,
				// I would think.

				if (nodeA->keyArray.size() > this->minDegree - 1)
				{
					std::shared_ptr<BTreeNode> leafNode = nodeA;
					while (!leafNode->IsLeaf())
						leafNode = leafNode->childNodeArray[leafNode->childNodeArray.size() - 1];

					std::shared_ptr<Key> predecessorKey = leafNode->keyArray[leafNode->keyArray.size() - 1];
					if (!this->RemoveKey(predecessorKey))
						return false;

					node->keyArray[i] = predecessorKey;
					return true;
				}
				else if (nodeB->keyArray.size() > this->minDegree - 1)
				{
					std::shared_ptr<BTreeNode> leafNode = nodeB;
					while (!leafNode->IsLeaf())
						leafNode = leafNode->childNodeArray[0];

					std::shared_ptr<Key> successorKey = leafNode->keyArray[0];
					if (!this->RemoveKey(successorKey))
						return false;

					node->keyArray[i] = successorKey;
					return true;
				}
				else
				{
					assert(false);
					return false;
				}
			}
		}
		else
		{
			// The caller tried to remove a key that is not present in the tree.
			if (node->IsLeaf())
				return false;

			node->FindChildOrKeyInsertionIndex(givenKey.get(), i);

			std::shared_ptr<BTreeNode> childNode = node->childNodeArray[i];
			if (childNode->keyArray.size() < this->minDegree)
			{
				if (i - 1 >= 0 && node->childNodeArray[i - 1]->keyArray.size() >= this->minDegree)
				{
					BTreeNode* siblingNode = node->childNodeArray[i - 1].get();

					std::shared_ptr<Key> keyA = node->keyArray[i];
					std::shared_ptr<Key> keyB = siblingNode->keyArray.back();

					siblingNode->keyArray.pop_back();
					node->keyArray.insert(node->keyArray.begin() + i, keyB);
					node->keyArray.erase(node->keyArray.begin() + (i + 1));
					childNode->keyArray.insert(childNode->keyArray.begin(), keyA);

					std::shared_ptr<BTreeNode> grandChild = siblingNode->childNodeArray.back();
					siblingNode->childNodeArray.pop_back();
					childNode->childNodeArray.insert(childNode->childNodeArray.begin(), grandChild);
				}
				else if (i + 1 < (int)node->childNodeArray.size() && node->childNodeArray[i + 1]->keyArray.size() >= this->minDegree)
				{
					BTreeNode* siblingNode = node->childNodeArray[i + 1].get();

					std::shared_ptr<Key> keyA = node->keyArray[i];
					std::shared_ptr<Key> keyB = siblingNode->keyArray.front();

					siblingNode->keyArray.erase(siblingNode->keyArray.begin());
					node->keyArray.insert(node->keyArray.begin() + i, keyB);
					node->keyArray.erase(node->keyArray.begin() + (i + 1));
					childNode->keyArray.push_back(keyA);

					std::shared_ptr<BTreeNode> grandChild = siblingNode->childNodeArray.front();
					siblingNode->childNodeArray.erase(siblingNode->childNodeArray.begin());
					childNode->childNodeArray.push_back(grandChild);
				}
				else if (i - 1 >= 0)
				{
					std::shared_ptr<Key> key = node->keyArray[i];
					node->keyArray.erase(node->keyArray.begin() + i);

					std::shared_ptr<BTreeNode> siblingNode = node->childNodeArray[i - 1];
					node->childNodeArray.erase(node->childNodeArray.begin() + i);

					siblingNode->keyArray.push_back(key);
					for (int j = 0; j < (int)childNode->keyArray.size(); j++)
						siblingNode->keyArray.push_back(childNode->keyArray[j]);

					for (int j = 0; j < (int)childNode->childNodeArray.size(); j++)
						siblingNode->childNodeArray.push_back(childNode->childNodeArray[j]);

					childNode = siblingNode;
				}
				else if (i + 1 < (int)node->childNodeArray.size())
				{
					std::shared_ptr<Key> key = node->keyArray[i + 1];
					node->keyArray.erase(node->keyArray.begin() + (i + 1));

					std::shared_ptr<BTreeNode> siblingNode = node->childNodeArray[i + 1];
					node->childNodeArray.erase(node->childNodeArray.begin() + (i + 1));

					childNode->keyArray.push_back(key);
					for (int j = 0; j < (int)siblingNode->keyArray.size(); j++)
						childNode->keyArray.push_back(siblingNode->keyArray[j]);

					for (int j = 0; j < (int)siblingNode->childNodeArray.size(); j++)
						childNode->childNodeArray.push_back(siblingNode->childNodeArray[j]);
				}
			}

			node = childNode.get();
		}
	}

	// ...if the root gots zero keys, shrink height of tree...

	return false;
}

/*virtual*/ bool BTree::FindKey(std::shared_ptr<Key> givenKey, std::shared_ptr<Key>& foundKey) const
{
	return false;
}

int BTree::GetMinDegree() const
{
	return this->minDegree;
}

int BTree::GetMaxDegree() const
{
	return 2 * this->minDegree;
}

//------------------------------------- BTreeNode -------------------------------------

BTreeNode::BTreeNode(std::shared_ptr<BTree> tree)
{
	this->treeWeakPtr = tree;
	this->renderScale = 0.005;
}

/*virtual*/ BTreeNode::~BTreeNode()
{
}

/*virtual*/ void BTreeNode::Render()
{
	glBegin(GL_QUADS);
	glColor3d(0.9, 0.9, 0.9);

	glVertex2d(this->keyRect.minCorner.x, this->keyRect.minCorner.y);
	glVertex2d(this->keyRect.maxCorner.x, this->keyRect.minCorner.y);
	glVertex2d(this->keyRect.maxCorner.x, this->keyRect.maxCorner.y);
	glVertex2d(this->keyRect.minCorner.x, this->keyRect.maxCorner.y);

	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3d(0.0, 0.0, 0.0);

	glVertex2d(this->keyRect.minCorner.x, this->keyRect.minCorner.y);
	glVertex2d(this->keyRect.maxCorner.x, this->keyRect.minCorner.y);
	glVertex2d(this->keyRect.maxCorner.x, this->keyRect.maxCorner.y);
	glVertex2d(this->keyRect.minCorner.x, this->keyRect.maxCorner.y);

	glEnd();

	HappyMath::Vector2 translation = this->keyRect.minCorner;
	translation.y += 0.1;

	glColor3d(1.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(translation.x, translation.y, 0.0);
	glScalef(this->renderScale, this->renderScale, this->renderScale);
	std::string label = this->MakeLabel();
	for (int i = 0; label.c_str()[i] != '\0'; i++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, label.c_str()[i]);
	glPopMatrix();
}

/*virtual*/ void BTreeNode::Translate(const HappyMath::Vector2& translation)
{
	Node::Translate(translation);

	this->keyRect += translation;
}

/*virtual*/ void BTreeNode::CalcBoundingRect()
{
	std::string label = this->MakeLabel();

	double width = this->renderScale * glutStrokeLengthf(GLUT_STROKE_MONO_ROMAN, (const unsigned char*)label.c_str());
	double height = this->renderScale * glutStrokeHeight(GLUT_STROKE_MONO_ROMAN);

	this->keyRect.minCorner.x = -width / 2.0;
	this->keyRect.maxCorner.x = width / 2.0;
	this->keyRect.minCorner.y = -height / 2.0;
	this->keyRect.maxCorner.y = height / 2.0;

	this->boundingRect = this->keyRect;
	this->boundingRect.AddHorizontalMargin(0.05);
	this->boundingRect.AddVerticalMargin(0.5);
}

std::string BTreeNode::MakeLabel()
{
	std::string label;

	for (int i = 0; i < (int)this->keyArray.size(); i++)
	{
		std::string keyStr;
		this->keyArray[i]->ToString(keyStr);

		if (i != 0)
			label += ",";

		label += keyStr;
	}

	return label;
}

/*virtual*/ void BTreeNode::GetChildren(std::vector<Tree::Node*>& givenChildNodeArray)
{
	givenChildNodeArray.clear();
	for (std::shared_ptr<BTreeNode>& childNode : this->childNodeArray)
		givenChildNodeArray.push_back(childNode.get());
}

/*virtual*/ void BTreeNode::GetKeys(std::vector<Tree::Key*>& givenKeyArray)
{
	givenKeyArray.clear();
	for (std::shared_ptr<Tree::Key>& key : this->keyArray)
		givenKeyArray.push_back(key.get());
}

bool BTreeNode::IsLeaf() const
{
	return this->childNodeArray.size() == 0;
}

bool BTreeNode::IsInternal() const
{
	return !this->IsLeaf();
}

bool BTreeNode::IsRoot() const
{
	return this->parentNodeWeakPtr.lock().get() == nullptr;
}

bool BTreeNode::IsFull() const
{
	return this->keyArray.size() == this->treeWeakPtr.lock()->GetMaxDegree() - 1;
}

bool BTreeNode::FindKey(std::shared_ptr<Tree::Key> givenKey, std::shared_ptr<BTreeNode>& foundKey, BTreeNode** foundNode /*= nullptr*/)
{
	int i = 0;

	if (this->FindKeyIndex(givenKey.get(), i))
	{
		if (foundNode)
			*foundNode = this;

		foundKey = this->shared_from_this();
		return true;
	}

	if (this->IsLeaf())
		return false;

	if (this->FindChildOrKeyInsertionIndex(givenKey.get(), i))
		return this->childNodeArray[i]->FindKey(givenKey, foundKey, foundNode);

	return false;
}

bool BTreeNode::FindKeyIndex(Tree::Key* givenKey, int& i)
{
	for (i = 0; i < (int)this->keyArray.size(); i++)
		if (this->keyArray[i]->IsEqualTo(givenKey))
			return true;

	return false;
}

bool BTreeNode::FindChildIndex(BTreeNode* givenNode, int& i)
{
	for (i = 0; i < (int)this->childNodeArray.size(); i++)
		if (this->childNodeArray[i].get() == givenNode)
			return true;

	return false;
}

bool BTreeNode::FindChildOrKeyInsertionIndex(Tree::Key* givenKey, int& i)
{
	if (givenKey->IsLessThan(this->keyArray[0].get()))
		i = 0;
	else if (givenKey->IsGreaterThan(this->keyArray[this->keyArray.size() - 1].get()))
		i = (int)this->keyArray.size();
	else
	{
		for (i = 0; i + 1 < (int)this->keyArray.size(); i++)
		{
			Tree::Key* keyA = this->keyArray[i].get();
			Tree::Key* keyB = this->keyArray[i + 1].get();

			if (givenKey->IsGreaterThan(keyA) && givenKey->IsLessThan(keyB))
			{
				i++;
				break;
			}
		}
	}

	return true;
}

bool BTreeNode::Split()
{
	std::shared_ptr<BTree> tree = this->treeWeakPtr.lock();
	if (!tree.get())
		return false;

	if (!this->IsFull())
		return false;

	std::shared_ptr<BTreeNode> parentNode = this->parentNodeWeakPtr.lock();
	if (parentNode.get() && parentNode->IsFull())
		return false;

	int minDegree = tree->GetMinDegree();

	auto liftedKey = this->keyArray[minDegree - 1];

	auto newNode = std::make_shared<BTreeNode>(tree);
	newNode->parentNodeWeakPtr = parentNode;

	for (int j = 0; j < minDegree - 1; j++)
	{
		auto movedKey = this->keyArray[minDegree + j];
		newNode->keyArray.push_back(movedKey);
	}

	this->keyArray.resize(minDegree - 1);

	if (!this->IsLeaf())
	{
		for (unsigned int j = 0; j < minDegree; j++)
		{
			auto movedChild = this->childNodeArray[minDegree + j];
			newNode->childNodeArray.push_back(movedChild);
			movedChild->parentNodeWeakPtr = newNode;
		}

		this->childNodeArray.resize(minDegree);
	}

	if (parentNode.get())
	{
		int i = -1;
		parentNode->FindChildIndex(this, i);
		parentNode->childNodeArray.insert(parentNode->childNodeArray.begin() + i + 1, newNode);
		parentNode->keyArray.insert(parentNode->keyArray.begin() + i, liftedKey);
	}
	else
	{
		auto newRoot = std::make_shared<BTreeNode>(tree);
		std::shared_ptr<Tree::Node> oldRoot = tree->rootNode;
		tree->rootNode = newRoot;
		newRoot->childNodeArray.push_back(this->shared_from_this());
		newRoot->childNodeArray.push_back(newNode);
		newRoot->keyArray.push_back(liftedKey);
		this->parentNodeWeakPtr = newRoot;
		newNode->parentNodeWeakPtr = newRoot;
	}

	return true;
}