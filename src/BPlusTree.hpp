/****************************************************************************
*																			*
*  @file     BPlusTree.h													*
*  @brief    Define the B+Tree and its node									*
*  @author   ������															*
*																			*
*****************************************************************************/
#ifndef MINISQL_BPLUSTREE_H
#define MINISQL_BPLUSTREE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "BufferManager.h"
#include "RecordPosition.h"
#include "Attribute.h"

using namespace std;

template<typename keyType> class BplusTree;
/*
* @brief The class define the node of the B+Tree 
*/
template<typename keyType>
class BplusTreeNode {
private:
#pragma region NodeHeaderDefine

	// byte0: Store if the node is a leaf node.
	const uint32_t IS_LEAF_OFFSET = 0;
	const uint32_t IS_LEAF_SIZE = sizeof(uint8_t);
	// byte1-4: Store the length of the index key
	const uint32_t KEY_LENGTH_OFFSET = IS_LEAF_OFFSET + IS_LEAF_SIZE;
	const uint32_t KEY_LENGTH_SIZE = sizeof(uint32_t);
	// byte5-8: Store the amount of the data 
	const uint32_t DATA_COUNT_OFFSET = KEY_LENGTH_OFFSET + KEY_LENGTH_SIZE;
	const uint32_t DATA_COUNT_SIZE = sizeof(uint32_t);
	// byte9-12: Store the page number of the node's parent
	const uint32_t PARENT_POINTER_OFFSET = DATA_COUNT_OFFSET + DATA_COUNT_SIZE;
	const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);

	// the total amount of the header
	const uint8_t NODE_HEADER_SIZE = IS_LEAF_SIZE + KEY_LENGTH_SIZE + DATA_COUNT_SIZE + PARENT_POINTER_SIZE;

#pragma endregion

	bool isLeafNode;
	uint32_t keyLength;
	uint32_t dataCount;
	uint32_t parent;
	uint32_t pageNum;

	vector<keyType> keyVector;
	vector<uint32_t> pageVector;
	vector<RecordPosition> recordVector;
public:
	BplusTreeNode(Block* _block, uint32_t _pageNum);
	BplusTreeNode(bool _isLeaf, uint32_t _keyLength, uint32_t _parent, uint32_t _pageNum);
	~BplusTreeNode();

	bool isRoot();
	bool isLeaf();
	bool isOverLoad();
	bool isHungry();

	uint32_t getParent();
	void setParent(uint32_t _parent);
	uint32_t getLastPointer();
	void setLastPointer(uint32_t _lastPointer);
	void removeLastPointer();
	uint32_t getDataCount();
	uint32_t getDegree();

	bool splitNode(BplusTree<keyType>* tree);
	bool mergeNode(BplusTree<keyType>* tree);

	int getPointerIndexWithKey(const keyType& key);
	keyType getKeyWithIndex(int index);
	uint32_t getPageWithIndex(int index);
	RecordPosition getRecordWithIndex(int index);

	bool insertValueWithKey(const keyType & _key, RecordPosition _value);
	bool insertValueWithKey(const keyType& _key, uint32_t _value);
	bool insertValueWithKey(const keyType& _key, uint32_t value1, uint32_t value2);
	bool modifyValueWithKey(const keyType & _key, RecordPosition _value);
	bool deleteValueWithKey(const keyType & _key);
	bool findValueWithKey(const keyType & _key, vector<RecordPosition> &result , int type);
	bool keyExistInNode(const keyType& _key);
	bool deleteChildWithKey(const keyType& _key);
	uint32_t findChildWithKey(const keyType& _key);

	void writeToDisk(Block* block);
	void printNode();
};

/*
* @brief The class define the B+Tree
*/
template<typename keyType>
class BplusTree {
private:
#pragma region TreeHeaderDefine
	// byte0-3: Store the length of the index key
	const uint32_t KEY_LENGTH_OFFSET = 0;
	const uint32_t KEY_LENGTH_SIZE = sizeof(uint32_t);
	// byte4-7: Store the amount of the tree nodes
	const uint32_t NODE_COUNT_OFFSET = KEY_LENGTH_OFFSET + KEY_LENGTH_SIZE;
	const uint32_t NODE_COUNT_SIZE = sizeof(uint32_t);
	// byte8-11: Store the page number of the root node
	const uint32_t ROOT_POINTER_OFFSET = NODE_COUNT_OFFSET + NODE_COUNT_SIZE;
	const uint32_t ROOT_POINTER_SIZE = sizeof(uint32_t);
	// byte12-15: Store the amount of free pages
	const uint32_t FREE_PAGE_COUNT_OFFSET = ROOT_POINTER_OFFSET + ROOT_POINTER_SIZE;
	const uint32_t FREE_PAGE_COUNT_SIZE = sizeof(uint32_t);

	// the total amount of the header
	const uint32_t TREE_HEADER_SIZE = KEY_LENGTH_SIZE + NODE_COUNT_SIZE + ROOT_POINTER_SIZE + FREE_PAGE_COUNT_SIZE;
#pragma endregion
	string indexFileName;
	uint32_t keyLength;
	uint32_t nodeCount;
	uint32_t root;
	uint32_t freePageCount;
	vector<uint32_t> freePages;
public:
	BplusTree(string _tableName, string _attributeName);
	BplusTree(string _tableName, string _attributeName, uint32_t _keyLength);
	~BplusTree();

	BplusTreeNode<keyType>* getNode(uint32_t pageNumber);
	uint32_t getNewPage();
	void setRoot(uint32_t _root);
	void makePageFree(uint32_t pageNumber);
	void writeNodeToDisk(BplusTreeNode<keyType>* node, uint32_t pageNumber);

	bool insertValueWithKey(const keyType& key, RecordPosition value);
	bool modifyValueWithKey(const keyType& key, RecordPosition value);
	bool deleteValueWithKey(const keyType& key);
	bool findValueWithKey(const keyType& key,vector<RecordPosition> &result, int type);
	void writeToDisk();
	void printTree();
	void printLeaves();
};

/****************************************************************************
*																			*
*  @file     BPlusTree.cpp													*
*  @brief    ʵ��B+������ڵ�													*
*  @author   ������															*
*																			*
*****************************************************************************/

template<typename Base, typename T>
inline bool instanceof(const T*) {
	return is_base_of<Base, T>::value;
}
//**************The implementation of the class BPlusTreeNode***************

/*
* @brief: Constructor of the B+Tree node use to create a new node
*
* @params:
*	_isLeaf: The type of the node.
*	_keyLength: The length of the key, depends on the type of key.
*	_parent: The page number of the node's parent, if the node is a root, input -1.
*	_pageNum: The number of the page which contains the node.
*/
template<typename keyType>
BplusTreeNode<keyType>::BplusTreeNode(bool _isLeaf, uint32_t _keyLength, uint32_t _parent, uint32_t _pageNum)
{
	this->isLeafNode = _isLeaf;
	this->keyLength = _keyLength;
	this->parent = _parent;
	this->pageNum = _pageNum;
	this->dataCount = 0;
}

/*
* @brief: Constructor of the B+Tree node use to read a node from file
*
* @params:
*	_block: The block stores the node.
*	_degree: The degree of the tree.
*/
template<typename keyType>
BplusTreeNode<keyType>::BplusTreeNode(Block* _block, uint32_t _pageNum)
{
	this->pageNum = _pageNum;

	_block->SetPin(true);

	memcpy(&isLeafNode, _block->data + IS_LEAF_OFFSET, IS_LEAF_SIZE);
	memcpy(&keyLength, _block->data + KEY_LENGTH_OFFSET, KEY_LENGTH_SIZE);
	memcpy(&dataCount, _block->data + DATA_COUNT_OFFSET, DATA_COUNT_SIZE);
	memcpy(&parent, _block->data + PARENT_POINTER_OFFSET, PARENT_POINTER_SIZE);

	int valueOffset = NODE_HEADER_SIZE;
	int valueSize = isLeaf() ? sizeof(RecordPosition) : sizeof(uint32_t);
	int keyOffset = valueOffset + valueSize;
	int step = valueSize + keyLength;

	if (isLeaf()) {
		for (uint32_t i = 0; i < getDataCount(); i++) {
			RecordPosition _record;
			keyType _key;
			memcpy(&_record, _block->data + valueOffset, valueSize);
			if (instanceof<string>(&_key)) {
				char* temp = new char[keyLength];
				memcpy(temp, _block->data + keyOffset, keyLength);
				string tempString = temp;
				_key = *(keyType*)&tempString;
				delete[] temp;
			}
			else {
				memcpy(&_key, _block->data + keyOffset, keyLength);
			}

			this->recordVector.push_back(_record);
			this->keyVector.push_back(_key);

			valueOffset += step;
			keyOffset += step;
		}
	}
	else
	{
		for (uint32_t i = 0; i < getDataCount(); i++) {
			uint32_t _value;
			keyType _key;
			memcpy(&_value, _block->data + valueOffset, sizeof(uint32_t));
			if (instanceof<string>(&_key)) {
				char* temp = new char[keyLength];
				memcpy(temp, _block->data + keyOffset, keyLength);
				string tempString = temp;
				_key = *(keyType*)&tempString;
				delete[] temp;
			}
			else {
				memcpy(&_key, _block->data + keyOffset, keyLength);
			}

			this->pageVector.push_back(_value);
			this->keyVector.push_back(_key);

			valueOffset += step;
			keyOffset += step;
		}
	}

	uint32_t _value;
	memcpy(&_value, _block->data + valueOffset, sizeof(uint32_t));
	this->pageVector.push_back(_value);

	_block->SetPin(false);
}

template <typename T>
inline void clear_vector(vector<T>& vt)
{
	std::vector<T> vector_arr;
	vector_arr.swap(vt);
}

/*
* @brief: Destructor of the B+Tree node
*/
template<typename keyType>
BplusTreeNode<keyType>::~BplusTreeNode()
{
	clear_vector(pageVector);
	clear_vector(recordVector);
	clear_vector(keyVector);
}

/*
* @brief: Check weather the node is a root node.
*
* @return:
*	true: That means the node is a root node.
*	false: That means the node is a root node.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::isRoot()
{
	return (parent == 0);
}


/*
* @brief: Check weather the node is a leaf node.
*
* @return:
*	true: That means the node is a leaf node.
*	false: That means the node is a internal node.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::isLeaf()
{
	return isLeafNode;
}

/*
* @brief: Check weather the node is overload.
*
* @return:
*	true: That means the node is overload.
*	false: That means the node is not overload.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::isOverLoad()
{
	return keyVector.size() > getDegree() - 1 || pageVector.size() > getDegree();
}

/*
* @brief: Check weather the node is hungry.
*
* @return:
*	true: That means the node is hungry.
*	false: That means the node is not hungry.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::isHungry()
{
	if (isLeaf()) {
		return keyVector.size() < getDegree() / 2;
	}
	else {
		return pageVector.size() < 2;
	}
}

/*
* @brief: Split the node when it is overload.
*
* @return: If the split success.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::splitNode(BplusTree<keyType>* tree)
{
	// the node is a leaf node
	if (isLeaf()) {
		// get the pointer to next 
		uint32_t nextLeafPage = this->getLastPointer();
		this->removeLastPointer();

		// the node is both root and leaf
		if (isRoot()) {
			// create a new root node
			uint32_t newRootPage = tree->getNewPage();
			BplusTreeNode<keyType>* newRoot = new BplusTreeNode<keyType>(false, keyLength, 0, newRootPage);
			tree->setRoot(newRootPage);
			this->setParent(newRootPage);

			// create a new leaf node
			uint32_t newLeafPage = tree->getNewPage();
			BplusTreeNode<keyType>* newLeaf = new BplusTreeNode<keyType>(true, keyLength, newRootPage, newLeafPage);

			int half = getDegree() / 2;
			// set childern of the new root
			newRoot->insertValueWithKey(keyVector[half], pageNum, newLeafPage);

			// move half data to the new leaf
			for (int i = 0; i < half; i++) {
				newLeaf->insertValueWithKey(keyVector[half], recordVector[half]);
				keyVector.erase(keyVector.begin() + half);
				recordVector.erase(recordVector.begin() + half);
				dataCount--;
			}

			// set the last pointer to next leaf
			this->setLastPointer(newLeafPage);
			newLeaf->setLastPointer(nextLeafPage);

			// write the node to disk
			tree->writeNodeToDisk(newRoot, newRootPage);
			tree->writeNodeToDisk(newLeaf, newLeafPage);
			tree->writeNodeToDisk(this, pageNum);
			tree->writeToDisk();

			delete newRoot;
			delete newLeaf;

			return true;
		}
		// the node is just a leaf node
		else {
			// create a new leaf node
			uint32_t newLeafPage = tree->getNewPage();
			BplusTreeNode<keyType>* newLeaf = new BplusTreeNode<keyType>(true, keyLength, getParent(), newLeafPage);

			// get parent node
			BplusTreeNode<keyType>* parent = tree->getNode(getParent());

			int half = getDegree() / 2;
			// set childern of the new root
			parent->insertValueWithKey(keyVector[half], pageNum, newLeafPage);

			// move half data to the new leaf
			for (int i = 0; i < half; i++) {
				newLeaf->insertValueWithKey(keyVector[half], recordVector[half]);
				keyVector.erase(keyVector.begin() + half);
				recordVector.erase(recordVector.begin() + half);
				dataCount--;
			}

			// set the last pointer to next leaf
			this->setLastPointer(newLeafPage);
			newLeaf->setLastPointer(nextLeafPage);
			BplusTreeNode<keyType>* nextLeaf = tree->getNode(nextLeafPage);
			if (nextLeaf != nullptr)
				nextLeaf->setParent(newLeafPage);

			// write the node to disk
			tree->writeNodeToDisk(newLeaf, newLeafPage);
			tree->writeNodeToDisk(this, pageNum);

			// if the parent need to split, the parent will write itself
			if (parent->isOverLoad())
				parent->splitNode(tree);
			// if the parent needn't to split write the parent
			else
				tree->writeNodeToDisk(parent, getParent());

			delete newLeaf;
			delete parent;

			return true;
		}
	}
	// the node is a internal node
	else {
		// the node is a root
		if (isRoot()) {
			// get the pointer to the last child
			uint32_t lastChildPage = this->getLastPointer();
			this->removeLastPointer();

			// create a new root node
			uint32_t newRootPage = tree->getNewPage();
			BplusTreeNode<keyType>* newRoot = new BplusTreeNode<keyType>(false, keyLength, 0, newRootPage);
			tree->setRoot(newRootPage);
			this->setParent(newRootPage);

			// create a new internal node
			uint32_t newInternalPage = tree->getNewPage();
			BplusTreeNode<keyType>* newInternal = new BplusTreeNode<keyType>(false, keyLength, newRootPage, newInternalPage);

			int half = getDegree() / 2;
			// set childern of the new root
			newRoot->insertValueWithKey(keyVector[half], pageNum, newInternalPage);
			keyVector.erase(keyVector.begin() + half);
			dataCount--;

			// move half data to the new internal
			for (int i = 0; i < half - 1; i++) {
				newInternal->insertValueWithKey(keyVector[half], pageVector[half + 1]);
				BplusTreeNode<keyType>* child = tree->getNode(pageVector[half + 1]);
				child->setParent(newInternalPage);
				tree->writeNodeToDisk(child, pageVector[half + 1]);
				delete child;
				keyVector.erase(keyVector.begin() + half);
				pageVector.erase(pageVector.begin() + half + 1);
				dataCount--;
			}

			BplusTreeNode<keyType>* lastChild = tree->getNode(lastChildPage);
			lastChild->setParent(newInternalPage);
			tree->writeNodeToDisk(lastChild, lastChildPage);
			delete lastChild;
			newInternal->setLastPointer(lastChildPage);

			// write the node to disk
			tree->writeNodeToDisk(newRoot, newRootPage);
			tree->writeNodeToDisk(newInternal, newInternalPage);
			tree->writeNodeToDisk(this, pageNum);
			tree->writeToDisk();

			delete newRoot;
			delete newInternal;

			return true;
		}
		// the node is just a internal node
		else
		{
			uint32_t lastChildPage = this->getLastPointer();
			this->removeLastPointer();

			// create a new internal node
			uint32_t newInternalPage = tree->getNewPage();
			BplusTreeNode<keyType>* newInternal = new BplusTreeNode<keyType>(false, keyLength, getParent(), newInternalPage);

			// get parent node
			BplusTreeNode<keyType>* parent = tree->getNode(getParent());

			int half = getDegree() / 2;
			// set childern of the new root
			parent->insertValueWithKey(keyVector[half], pageNum, newInternalPage);
			keyVector.erase(keyVector.begin() + half);
			dataCount--;

			// move half data to the new internal
			for (int i = 0; i < half - 1; i++) {
				newInternal->insertValueWithKey(keyVector[half], pageVector[half + 1]);
				BplusTreeNode<keyType>* child = tree->getNode(pageVector[half + 1]);
				child->setParent(newInternalPage);
				tree->writeNodeToDisk(child, pageVector[half + 1]);
				delete child;
				keyVector.erase(keyVector.begin() + half);
				pageVector.erase(pageVector.begin() + half + 1);
				dataCount--;
			}

			BplusTreeNode<keyType>* lastChild = tree->getNode(lastChildPage);
			lastChild->setParent(newInternalPage);
			tree->writeNodeToDisk(lastChild, lastChildPage);
			delete lastChild;
			newInternal->setLastPointer(lastChildPage);

			// write the node to disk
			tree->writeNodeToDisk(newInternal, newInternalPage);
			tree->writeNodeToDisk(this, pageNum);

			// if the parent need to split, the parent will write itself
			if (parent->isOverLoad())
				parent->splitNode(tree);
			// if the parent needn't to split write the parent
			else
				tree->writeNodeToDisk(parent, getParent());

			delete parent;
			delete newInternal;

			return true;
		}
	}
}

/*
* @brief: Delete the node when it is hungry.
*
* @return: If the delete success.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::mergeNode(BplusTree<keyType>* tree)
{
	tree->printTree();
	if (isRoot()) {
		if (!isLeaf() && this->getDataCount() == 0) {
			uint32_t newRootPage = getPageWithIndex(0);
			BplusTreeNode<keyType>* newRoot = tree->getNode(newRootPage);
			tree->setRoot(newRootPage);

			newRoot->setParent(0);
			this->removeLastPointer();
			tree->writeNodeToDisk(this, pageNum);
			tree->makePageFree(pageNum);
			tree->writeNodeToDisk(newRoot, newRootPage);

			delete newRoot;
		}

		return true;
	}

	// get the node's parent
	BplusTreeNode<keyType>* parent = tree->getNode(getParent());

	// the node is a leaf node
	if (isLeaf()) {
		keyType firstKey = getKeyWithIndex(0);
		int parentPointIndex = parent->getPointerIndexWithKey(firstKey);
		// try to merge or replace the left sibling
		if (parentPointIndex != 0) {
			uint32_t siblingPage = parent->getPageWithIndex(parentPointIndex - 1);
			BplusTreeNode<keyType>* sibling = tree->getNode(siblingPage);
			// can merge to its left sibling
			if (sibling->getDataCount() + this->getDataCount() < getDegree()) {
				parent->deleteChildWithKey(firstKey);
				uint32_t nextLeaf = this->getLastPointer();
				sibling->removeLastPointer();
				int count = getDataCount();

				for (uint32_t i = 0; i < count; i++) {
					sibling->insertValueWithKey(keyVector[0], recordVector[0]);
					this->deleteValueWithKey(keyVector[0]);
				}

				sibling->setLastPointer(nextLeaf);
				this->removeLastPointer();

				tree->writeNodeToDisk(this, pageNum);
				tree->makePageFree(pageNum);
				tree->writeNodeToDisk(sibling, siblingPage);

				if (parent->isHungry()) return parent->mergeNode(tree);
				else tree->writeNodeToDisk(parent, getParent());

				delete sibling;
				delete parent;

				return true;

			}
			// borrow the most right key from left sibling
			else {
				parent->deleteChildWithKey(firstKey);

				keyType _key = sibling->getKeyWithIndex(sibling->getDataCount() - 1);
				RecordPosition _value = sibling->getRecordWithIndex(sibling->getDataCount() - 1);

				this->insertValueWithKey(_key, _value);
				sibling->deleteValueWithKey(_key);

				parent->insertValueWithKey(_key, siblingPage, pageNum);

				tree->writeNodeToDisk(this, pageNum);
				tree->writeNodeToDisk(parent, getParent());
				tree->writeNodeToDisk(sibling, siblingPage);

				delete parent;
				delete sibling;
				return true;
			}
		}
		// try to merge or replace the right sibling
		else {
			uint32_t siblingPage = parent->getPageWithIndex(parentPointIndex + 1);
			BplusTreeNode<keyType>* sibling = tree->getNode(siblingPage);
			// can merge to its right sibling
			if (sibling->getDataCount() + this->getDataCount() < getDegree()) {
				parent->deleteChildWithKey(sibling->getKeyWithIndex(0));
				uint32_t nextLeaf = sibling->getLastPointer();
				this->removeLastPointer();

				int count = sibling->getDataCount();

				for (uint32_t i = 0; i < count; i++) {
					keyType _key = sibling->getKeyWithIndex(0);
					RecordPosition _value = sibling->getRecordWithIndex(0);
					this->insertValueWithKey(_key, _value);
					sibling->deleteValueWithKey(_key);
				}

				this->setLastPointer(nextLeaf);
				sibling->removeLastPointer();

				tree->writeNodeToDisk(sibling, siblingPage);
				tree->makePageFree(siblingPage);
				tree->writeNodeToDisk(this, pageNum);

				if (parent->isHungry()) return parent->mergeNode(tree);
				else tree->writeNodeToDisk(parent, getParent());

				delete parent;
				delete sibling;
				return true;
			}
			// borrow the most left key from right sibling
			else {
				keyType _key = sibling->getKeyWithIndex(0);
				parent->deleteChildWithKey(_key);

				uint32_t _value = sibling->getPageWithIndex(0);
				this->insertValueWithKey(_key, _value);
				sibling->deleteValueWithKey(_key);

				parent->insertValueWithKey(sibling->getKeyWithIndex(0), pageNum, siblingPage);

				tree->writeNodeToDisk(this, pageNum);
				tree->writeNodeToDisk(parent, getParent());
				tree->writeNodeToDisk(sibling, siblingPage);

				delete sibling;
				delete parent;
				return true;
			}
		}
	}
	// the node is a internal node
	else {
		uint32_t childPage = getPageWithIndex(0);
		BplusTreeNode<keyType>* child = tree->getNode(childPage);
		keyType firstKey = child->getKeyWithIndex(0);
		int parentPointIndex = parent->getPointerIndexWithKey(firstKey);

		// try to merge or replace the left sibling
		if (parentPointIndex != 0) {
			uint32_t siblingPage = parent->getPageWithIndex(parentPointIndex - 1);
			BplusTreeNode<keyType>* sibling = tree->getNode(siblingPage);

			parent->deleteChildWithKey(firstKey);

			keyType _key = parent->getKeyWithIndex(parentPointIndex - 1);
			uint32_t _value = sibling->getLastPointer();

			// can merge to the left node
			if (sibling->getDataCount() + this->getDataCount() < getDegree() - 1) {
				sibling->insertValueWithKey(_key, _value, childPage);
				this->removeLastPointer();

				tree->writeNodeToDisk(this, pageNum);
				tree->makePageFree(pageNum);
				tree->writeNodeToDisk(sibling, siblingPage);

				if (parent->isHungry()) return parent->mergeNode(tree);
				else tree->writeNodeToDisk(parent, getParent());

				delete sibling;
				delete parent;

				return true;
			}
			// borrow the most right key from the left node
			else {
				BplusTreeNode<keyType>* siblingChild = tree->getNode(_value);
				keyType _childKey = siblingChild->getKeyWithIndex(0);

				sibling->deleteChildWithKey(_childKey);
				this->insertValueWithKey(_key, _value);
				parent->insertValueWithKey(_childKey, siblingPage, pageNum);

				tree->writeNodeToDisk(this, pageNum);
				tree->makePageFree(pageNum);
				tree->writeNodeToDisk(sibling, siblingPage);

				delete sibling;
				delete parent;

				return true;
			}
		}
		// try to merge or replace the right sibling
		else
		{
			uint32_t siblingPage = parent->getPageWithIndex(parentPointIndex + 1);
			BplusTreeNode<keyType>* sibling = tree->getNode(siblingPage);

			keyType _key = parent->getKeyWithIndex(parentPointIndex);
			uint32_t _value = sibling->getPageWithIndex(0);

			// can merge to the right node
			if (sibling->getDataCount() + this->getDataCount() < getDegree() - 1) {
				sibling->insertValueWithKey(_key, childPage, _value);
				this->removeLastPointer();

				parent->deleteValueWithKey(_key);

				tree->writeNodeToDisk(this, pageNum);
				tree->makePageFree(pageNum);
				tree->writeNodeToDisk(sibling, siblingPage);

				if (parent->isHungry()) return parent->mergeNode(tree);
				else tree->writeNodeToDisk(parent, getParent());

				delete sibling;
				delete parent;

				return true;
			}
			// borrow the most left key from the right node
			else {
				BplusTreeNode<keyType>* siblingChild = tree->getNode(_value);
				keyType _childKey = siblingChild->getKeyWithIndex(1);

				sibling->deleteValueWithKey(_childKey);
				this->insertValueWithKey(_key, childPage, _value);
				parent->insertValueWithKey(_childKey, pageNum, siblingPage);

				tree->writeNodeToDisk(this, pageNum);
				tree->makePageFree(pageNum);
				tree->writeNodeToDisk(sibling, siblingPage);

				delete sibling;
				delete parent;

				return true;
			}
		}

	}
}
/*
* @brief: Get the parent of the node.
*
* @return: The page number of the parent.
*/
template<typename keyType>
uint32_t BplusTreeNode<keyType>::getParent()
{
	return parent;
}

/*
* @brief: Set the parent of the node.
*
* @param: The page number of the parent.
*/
template<typename keyType>
void BplusTreeNode<keyType>::setParent(uint32_t _parent)
{
	this->parent = _parent;
}

/*
* @brief: Get the last value of the node.
*
* @return: The page number of last node.
*/
template<typename keyType>
uint32_t BplusTreeNode<keyType>::getLastPointer()
{
	return pageVector.back();
}

/*
* @brief: Set the last value of the node.
*
* @param: The page number of last node.
*/
template<typename keyType>
void BplusTreeNode<keyType>::setLastPointer(uint32_t _lastPointer)
{
	pageVector.push_back(_lastPointer);
}

/*
* @brief: Remove the last value of the node.
*/
template<typename keyType>
void BplusTreeNode<keyType>::removeLastPointer()
{
	pageVector.pop_back();
}

/*
* @brief: Get the amount of data in the node.
*
* @return: The amount of data in the node.
*/
template<typename keyType>
uint32_t BplusTreeNode<keyType>::getDataCount()
{
	return dataCount;
}

/*
* @brief: Get the amount of data in the node.
*
* @return: The amount of data in the node.
*/
template<typename keyType>
uint32_t BplusTreeNode<keyType>::getDegree()
{
	int canUseSize = BLOCK_SIZE - NODE_HEADER_SIZE - sizeof(uint32_t);
	int degree;

	if (isLeaf())
		degree = canUseSize / (keyLength + sizeof(RecordPosition));
	else
		degree = canUseSize / (keyLength + sizeof(uint32_t));

	degree = degree % 2 == 0 ? degree : degree - 1;
	return degree;
}

/*
* @brief: Get the index which equals to the key.
*
* @return:
*	others: The index of the key's pointer.
*/
template<typename keyType>
int BplusTreeNode<keyType>::getPointerIndexWithKey(const keyType& key)
{
	auto it = lower_bound(keyVector.begin(), keyVector.end(), key);
	if (*it == key) {
		return it - keyVector.begin() + 1;
	}
	else {
		return it - keyVector.begin();
	}
}

/*
* @brief: Get the key with index.
*
* @return: The key in vector with index.
*/
template<typename keyType>
keyType BplusTreeNode<keyType>::getKeyWithIndex(int index)
{
	return keyVector[index];
}

/*
* @brief: Get the value with index.
*
* @return: The value in vector with index.
*/
template<typename keyType>
uint32_t BplusTreeNode<keyType>::getPageWithIndex(int index)
{
	return pageVector[index];
}

/*
* @brief: Get the value with index.
*
* @return: The value in vector with index.
*/
template<typename keyType>
RecordPosition BplusTreeNode<keyType>::getRecordWithIndex(int index)
{
	return recordVector[index];
}

/*
* @brief: Insert the value with key, the node should be a leaf
*
* @params:
*	key: The key of the value to insert
*	value: The value to be inserted to
*
* @return:
*	true: That means insert successfully.
*	false: That means insert failed.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::insertValueWithKey(const keyType& _key, RecordPosition _value)
{
	// If the node is not a leaf node
	if (!isLeaf()) {
		cout << "Try to insert value in a internal node" << endl;
		return false;
	}
	//insert data into a leaf node
	int sub = lower_bound(keyVector.begin(), keyVector.end(), _key) - keyVector.begin();
	keyVector.insert(keyVector.begin() + sub, _key);
	recordVector.insert(recordVector.begin() + sub, _value);

	dataCount++;
	return true;
}

template<typename keyType>
bool BplusTreeNode<keyType>::insertValueWithKey(const keyType& _key, uint32_t _value)
{
	// If the node is not a internal node
	if (isLeaf()) {
		cout << "Try to insert pointer in a leaf node" << endl;
		return false;
	}
	//insert data into a internal node
	int sub = lower_bound(keyVector.begin(), keyVector.end(), _key) - keyVector.begin();
	keyVector.insert(keyVector.begin() + sub, _key);
	pageVector.insert(pageVector.begin() + sub, _value);
	dataCount++;
	return true;
}

/*
* @brief: Insert the value with key, the node should be a leaf
*
* @params:
*	key: The key of the value to insert
*	value: The value to be inserted to
*
* @return:
*	true: That means insert successfully.
*	false: That means insert failed.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::insertValueWithKey(const keyType& _key, uint32_t _value1, uint32_t _value2)
{
	// If the node is not a internal node
	if (isLeaf()) {
		cout << "Try to insert node in a leaf node" << endl;
		return false;
	}
	//insert data into a internal node
	if (getDataCount() == 0) {
		keyVector.insert(keyVector.begin(), _key);
		pageVector.insert(pageVector.begin(), _value2);
		pageVector.insert(pageVector.begin(), _value1);
	}
	else {
		int sub = lower_bound(keyVector.begin(), keyVector.end(), _key) - keyVector.begin();
		keyVector.insert(keyVector.begin() + sub, _key);
		pageVector.insert(pageVector.begin() + sub, _value1);
		pageVector[sub + 1] = _value2;
	}
	dataCount++;
	return true;
}

/*
* @brief: Modify the value with Key
*
* @params:
*	key: The key of the value to modify
*	value: The value to be modified to
*
* @return:
*	true: That means modification succeeded.
*	false: means modification failed.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::modifyValueWithKey(const keyType& _key, RecordPosition _value)
{
	// If the node is not a leaf node
	if (!isLeaf()) {
		cout << "Try to modify value in a internal node" << endl;
		return false;
	}
	// Modify data in a leaf node
	if (keyVector.empty()) {
		cout << "The node is empty" << endl;
		return false;
	}

	auto it = lower_bound(keyVector.begin(), keyVector.end(), _key);

	if (it != keyVector.end() && *it == _key) {
		recordVector[it - keyVector.begin()] = _value;
		return true;
	}
	else {
		cout << "That key is not in the tree" << endl;
		return false;
	}
}

/*
* @brief: Delete the value with key in a leaf node
*
* @param: The key of the value to delete
*
* @return:
*	true: That means deletion succeeded
*	false: That means deletion failed
*/
template<typename keyType>
bool BplusTreeNode<keyType>::deleteValueWithKey(const keyType& key)
{
	// If the node is not a leaf node
	if (!isLeaf()) {
		cout << "Try to delete value in a internal node" << endl;
		return false;
	}
	// Delete data in a leaf node
	if (keyVector.empty()) {
		cout << "The node is empty" << endl;
		return false;
	}

	int sub = lower_bound(keyVector.begin(), keyVector.end(), key) - keyVector.begin();
	if (sub + keyVector.begin() != keyVector.end() && keyVector[sub] == key) {
		keyVector.erase(sub + keyVector.begin());
		recordVector.erase(sub + recordVector.begin());
		dataCount--;
		return true;
	}
	else {
		cout << "That key is not in the tree" << endl;
		return false;
	}
}

/*
* @brief: Find the value with key in a leaf node
*
* @param: The key of the value to find
*
* @return:
*	-1: That means the key is not in the tree, or find value in a internal node.
*	other returns: That means the block index stores the record.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::findValueWithKey(const keyType& _key, vector<RecordPosition>& result, int type)
{
	// If the node is not a leaf node
	if (!isLeaf()) {
		cout << "Try to find value in a internal node" << endl;
		return false;
	}
	// find in the leaf node
	if (keyVector.empty()) {
		cout << "The node is empty" << endl;
		return false;
	}

	// equal
	if (type == 0) {
		
		int sub = lower_bound(keyVector.begin(), keyVector.end(), _key) - keyVector.begin();

		if (sub + keyVector.begin() != keyVector.end() && keyVector[sub] == _key) {
			result.push_back(recordVector[sub]);
		}
		else {
			cout << "That key is not in the tree" << endl;
			return false;
		}

		return true;
	}
	//not equal
	else if (type == 1) {
		for (uint32_t i = 0; i < keyVector.size(); i++) {

			if (keyVector[i] != _key)
				result.push_back(recordVector[i]);
			else return false;
		}
		return true;
	}
	// litter than
	else if (type == 2) {
		for (uint32_t i = 0; i < keyVector.size(); i++) {

			if (keyVector[i] < _key)
				result.push_back(recordVector[i]);
			else return false;
		}
		return true;
	}
	// bigger than
	else if (type == 3) {
		for (uint32_t i = 0; i < keyVector.size(); i++) {
			if (keyVector[i] > _key)
				result.push_back(recordVector[i]);
		}

		return true;
	}
	// litter than or equal
	else if (type == 4) {
		for (uint32_t i = 0; i < keyVector.size(); i++) {
			if (keyVector[i] <= _key)
				result.push_back(recordVector[i]);
			else return false;
		}
		return true;
	}
	// bigger than or equal
	else if (type == 5) {
		for (uint32_t i = 0; i < keyVector.size(); i++) {
			if (keyVector[i] >= _key)
				result.push_back(recordVector[i]);
		}

		return true;
	}
	else {
		cout << "Wrong type to find with index !" << endl;
		return false;
	}
}

template<typename keyType>
bool BplusTreeNode<keyType>::keyExistInNode(const keyType& _key)
{
	int sub = lower_bound(keyVector.begin(), keyVector.end(), _key) - keyVector.begin();
	return ((sub + keyVector.begin()) != keyVector.end() && keyVector[sub] == _key);
}

/*
* @brief: Delete the child page with key in a internal node
*
* @param: The key of the value to find
*
* @return: That page number of the child.
*/
template<typename keyType>
bool BplusTreeNode<keyType>::deleteChildWithKey(const keyType& key)
{
	// If the node is not a internal node
	if (isLeaf()) {
		cout << "Try to find a child in a leaf node" << endl;
		return false;
	}
	// find the child in the internal node
	if (keyVector.empty()) {
		cout << "The node is empty" << endl;
		return false;
	}

	int sub = lower_bound(keyVector.begin(), keyVector.end(), key) - keyVector.begin();
	if (sub + keyVector.begin() != keyVector.end() && keyVector[sub] == key) {
		keyVector.erase(sub + keyVector.begin());
		pageVector.erase(sub + pageVector.begin() + 1);
		dataCount--;
		return true;
	}
	return false;
}

/*
* @brief: Find the child page with key in a internal node
*
* @param: The key of the value to find
*
* @return: That page number of the child.
*/
template<typename keyType>
uint32_t BplusTreeNode<keyType>::findChildWithKey(const keyType& _key)
{
	// If the node is not a internal node
	if (isLeaf()) {
		cout << "Try to find a child in a leaf node" << endl;
		return 0;
	}
	// find the child in the internal node
	if (keyVector.empty()) {
		cout << "The node is empty" << endl;
		return 0;
	}
	int sub = lower_bound(keyVector.begin(), keyVector.end(), _key) - keyVector.begin();
	// if the key on the node equals to the key to search

	if (sub == keyVector.size() || _key < keyVector[sub]) {
		return pageVector[sub];
	}// if the key on the node is greater than the key to search
	else {
		return pageVector[sub + 1];
	}
}

/*
* @brief: Write the node to the disk
*
* @param: The block will be written to
*/
template<typename keyType>
void BplusTreeNode<keyType>::writeToDisk(Block* _block)
{
	_block->SetPin(true);
	_block->SetDirty(true);

	memcpy(_block->data + IS_LEAF_OFFSET, &isLeafNode, IS_LEAF_SIZE);
	memcpy(_block->data + KEY_LENGTH_OFFSET, &keyLength, KEY_LENGTH_SIZE);
	memcpy(_block->data + DATA_COUNT_OFFSET, &dataCount, DATA_COUNT_SIZE);
	memcpy(_block->data + PARENT_POINTER_OFFSET, &parent, PARENT_POINTER_SIZE);

	int valueOffset = NODE_HEADER_SIZE;
	int valueSize = isLeaf() ? sizeof(RecordPosition) : sizeof(uint32_t);
	int keyOffset = valueOffset + valueSize;
	int step = valueSize + keyLength;

	if (isLeaf()) {
		for (uint32_t i = 0; i < getDataCount(); i++) {
			memcpy(_block->data + valueOffset, &recordVector[i], valueSize);

			if (instanceof<string>(&keyVector[i])) {
				string temp = *(string*)&keyVector[i];
				memcpy(_block->data + keyOffset, temp.c_str(), keyLength);
			}
			else {
				memcpy(_block->data + keyOffset, &keyVector[i], keyLength);
			}

			valueOffset += step;
			keyOffset += step;
		}
	}
	else {
		for (uint32_t i = 0; i < getDataCount(); i++) {
			memcpy(_block->data + valueOffset, &pageVector[i], valueSize);

			if (instanceof<string>(&keyVector[i])) {
				string temp = *(string*)&keyVector[i];
				memcpy(_block->data + keyOffset, temp.c_str(), keyLength);
			}
			else {
				memcpy(_block->data + keyOffset, &keyVector[i], keyLength);
			}

			valueOffset += step;
			keyOffset += step;
		}
	}

	if (!pageVector.empty())
		memcpy(_block->data + valueOffset, &pageVector.back(), sizeof(uint32_t));

	_block->SetPin(false);
}


template<typename keyType>
void BplusTreeNode<keyType>::printNode()
{
	cout << "+-----------------------------------------------------------------------+" << endl;
	cout << "�˽ڵ��ҳ��Ϊ��" << pageNum << " ����" << dataCount << "�����ݡ� ���ڵ�Ϊ " << parent << endl;
	if (isLeaf()) {
		cout << "key |" << " block number |" << " record number " << endl;
		for (uint32_t i = 0; i < getDataCount(); i++) {
			cout << keyVector[i] << "    " << recordVector[i].blockNum << "    " << recordVector[i].recordNum << endl;
		}
		if (!pageVector.empty())
			cout << "next leaf page " << pageVector.back() << endl;
	}
	else {
		cout << "key |" << " page number " << endl;
		for (uint32_t i = 0; i < getDataCount(); i++) {
			cout << keyVector[i] << "    " << pageVector[i] << endl;
		}
		if (!pageVector.empty())
			cout << pageVector.back() << endl;
	}
}



//**************The implementation of the class BPlusTreeNode***************




/*
* @brief: Constructor of the B+Tree use to load a tree from the file
*
* @params:
*	_tableName: The table countains the B+tree.
*	_attributeName: The atrribute which the B+Tree is located.
*	_degree: The degree of the B+tree.
*/
template<typename keyType>
BplusTree<keyType>::BplusTree(string _tableName, string _attributeName)
{
	this->indexFileName = _tableName + "_" + _attributeName + ".index";
	Block* _block = bufferManager.GetFileBlock(indexFileName, 0);

	_block->SetPin(true);

	memcpy(&keyLength, _block->data + KEY_LENGTH_OFFSET, KEY_LENGTH_SIZE);
	memcpy(&nodeCount, _block->data + NODE_COUNT_OFFSET, NODE_COUNT_SIZE);
	memcpy(&root, _block->data + ROOT_POINTER_OFFSET, ROOT_POINTER_SIZE);
	memcpy(&freePageCount, _block->data + FREE_PAGE_COUNT_OFFSET, FREE_PAGE_COUNT_SIZE);

	int pageOffset = TREE_HEADER_SIZE;
	int step = sizeof(uint32_t);
	for (uint32_t i = 0; i < freePageCount; i++) {
		uint32_t _freePage;
		memcpy(&_freePage, _block->data + pageOffset, sizeof(uint32_t));

		freePages.push_back(_freePage);
		pageOffset += step;
	}

	_block->SetPin(false);
}

/*
* @brief: Constructor of the B+Tree use to create a new tree
*
* @params:
*	_tableName: The table countains the B+tree.
*	_attributeName: The atrribute which the B+Tree is located.
*	_keyLength: The length of the key, depends on the type of key.
*	_degree: The degree of the B+tree.
*/
template<typename keyType>
BplusTree<keyType>::BplusTree(string _tableName, string _attributeName, uint32_t _keyLength)
{
	this->indexFileName = _tableName + "_" + _attributeName + ".index";
	this->keyLength = _keyLength;
	this->nodeCount = 1;
	this->root = 1;
	this->freePageCount = 0;

	bufferManager.AppendFile(indexFileName);
	BplusTreeNode<keyType>* node = new BplusTreeNode<keyType>(true, keyLength, 0, 1);
	Block* rootBlock = bufferManager.GetFileBlock(indexFileName, 1);
	node->setLastPointer(0);
	node->writeToDisk(rootBlock);

	writeToDisk();
}


/*
* @brief: Destructor of the B+Tree
*/
template<typename keyType>
BplusTree<keyType>::~BplusTree()
{
	freePages.clear();
}

/*
* @brief: Get the node pointer from the page number
*
* @params: The page number stores the node.
*
* @return: The node stored in the page.
*/
template<typename keyType>
BplusTreeNode<keyType>* BplusTree<keyType>::getNode(uint32_t pageNumber)
{
	if (pageNumber == 0) return nullptr;

	Block* nodeBlock = bufferManager.GetFileBlock(indexFileName, pageNumber);
	BplusTreeNode<keyType>* node = new BplusTreeNode<keyType>(nodeBlock, pageNumber);
	return node;
}

template<typename keyType>
uint32_t BplusTree<keyType>::getNewPage()
{
	// if there is not a free page, return a new page
	if (freePages.empty()) {
		bufferManager.AppendFile(indexFileName);
		nodeCount++;
		return bufferManager.GetFileSize(indexFileName) - 1;
	}
	// if there is a free page, return the first free page
	else {
		uint32_t pageNumber = freePages[0];
		freePages.erase(freePages.begin());
		return pageNumber;
	}
}

/*
* @brief: Set the root of the tree.
*
* @param: The page number of the root.
*/
template<typename keyType>
void BplusTree<keyType>::setRoot(uint32_t _root)
{
	this->root = _root;
}

/*
* @brief: Make a page free.
*
* @param: The number of the page to free.
*/
template<typename keyType>
void BplusTree<keyType>::makePageFree(uint32_t pageNumber)
{
	BplusTreeNode<keyType>* freePage = getNode(pageNumber);
	freePage->setParent(0);
	writeNodeToDisk(freePage, pageNumber);
	delete freePage;
	nodeCount--;
	freePageCount++;
	freePages.push_back(pageNumber);
}

/*
* @brief: Write the node to its block.
*
* @param:
*	node: The node to write
*	pageNumber: The page number of the node to write.
*/
template<typename keyType>
void BplusTree<keyType>::writeNodeToDisk(BplusTreeNode<keyType>* node, uint32_t pageNumber)
{
	Block* nodeBlock = bufferManager.GetFileBlock(indexFileName, pageNumber);
	node->writeToDisk(nodeBlock);
}

/*
* @brief: Insert the value with Key.
*
* @params:
*	key: The key of the value to insert
*	value: The value to be inserted to
*
* @return:
*	true: That means insert successfully.
*	false: That means insert failed.
*/
template<typename keyType>
bool BplusTree<keyType>::insertValueWithKey(const keyType& key, RecordPosition value)
{
	BplusTreeNode<keyType>* node = getNode(root);
	uint32_t nodeNumber = root;

	while (node != nullptr && !node->isLeaf()) {
		nodeNumber = node->findChildWithKey(key);
		delete node;
		node = getNode(nodeNumber);
	}

	if (node == nullptr) {
		cout << "The node is null" << endl;
		return false;
	}

	if (node->insertValueWithKey(key, value)) {

		if (node->isOverLoad()) {
			node->splitNode(this);
		}
		else {
			writeNodeToDisk(node, nodeNumber);
		}

		delete node;
	}
	return true;
}


/*
* @brief: Modify the value with Key
*
* @params:
*	key: The key of the value to modify
*	value: The value to be modified to
*
* @return:
*	true: That means modification succeeded.
*	false: means modification failed.
*/
template<typename keyType>
bool BplusTree<keyType>::modifyValueWithKey(const keyType& key, RecordPosition value)
{
	BplusTreeNode<keyType>* node = getNode(root);
	uint32_t nodeNumber = root;

	while (node != nullptr && !node->isLeaf()) {
		nodeNumber = node->findChildWithKey(key);
		delete node;
		node = getNode(nodeNumber);
	}

	if (node == nullptr) {
		cout << "The node is null" << endl;
		return false;
	}

	node->modifyValueWithKey(key, value);

	writeNodeToDisk(node, nodeNumber);
	writeToDisk();
	bufferManager.SaveFile(indexFileName);

	delete node;

	return true;
}

/*
* @brief: Delete the value with key in the tree
*
* @param: The key of the value to delete
*
* @return:
*	true: That means deletion succeeded
*	false: That means deletion failed
*/
template<typename keyType>
bool BplusTree<keyType>::deleteValueWithKey(const keyType& key)
{
	BplusTreeNode<keyType>* node = getNode(root);
	uint32_t firstNodeNumber = root;
	uint32_t nodeNumber = root;

	while (node != nullptr && !node->isLeaf()) {
		firstNodeNumber = node->getPageWithIndex(0);
		delete node;
		node = getNode(firstNodeNumber);
	}

	delete node;
	node = getNode(root);
	while (node != nullptr && !node->isLeaf()) {
		nodeNumber = node->findChildWithKey(key);
		delete node;
		node = getNode(nodeNumber);
	}

	if (node == nullptr) {
		cout << "The node is null" << endl;
		return false;
	}

	if (!node->keyExistInNode(key)) {
		cout << "The key is not in the tree !" << endl;
		return false;
	}
	else if (node->isRoot()) {
		node->deleteValueWithKey(key);
		writeNodeToDisk(node, nodeNumber);
		return true;
	}
	// key exist in the branch node
	else if (key == node->getKeyWithIndex(0) && nodeNumber != firstNodeNumber) {
		uint32_t parentPage = node->getParent();
		BplusTreeNode<keyType>* parent = getNode(node->getParent());
		while (!parent->keyExistInNode(key)) {
			if (!parent->isRoot()) {
				parentPage = parent->getParent();
				delete parent;
				parent = getNode(parentPage);
			}
			else break;
		}
		int pointerIndex = parent->getPointerIndexWithKey(key);
		uint32_t branchPage1 = parent->getPageWithIndex(pointerIndex - 1);
		uint32_t branchPage2 = parent->getPageWithIndex(pointerIndex);
		keyType newKey = node->getKeyWithIndex(1);
		parent->deleteChildWithKey(key);
		parent->insertValueWithKey(newKey, branchPage1, branchPage2);
		writeNodeToDisk(parent, parentPage);
	}

	node->deleteValueWithKey(key);

	if (node->isHungry()) {
		node->mergeNode(this);
	}

	writeNodeToDisk(node, nodeNumber);
	writeToDisk();
	bufferManager.SaveFile(indexFileName);

	delete node;

	return true;
}




/*
* @brief: Find the value with key in the tree
*
* @param: The key of the value to find
*
* @return:
*	-1: That means the key is not in the tree.
*	other returns: That means the block index stores the record.
*/
template<typename keyType>
bool BplusTree<keyType>::findValueWithKey(const keyType& key, vector<RecordPosition>& result, int type)
{
	BplusTreeNode<keyType>* node = getNode(root);
	uint32_t nodeNumber = root;

	// equal
	if (type == 0) {
		while (node != nullptr && !node->isLeaf()) {
			nodeNumber = node->findChildWithKey(key);
			delete node;
			node = getNode(nodeNumber);
		}

		if (node == nullptr) {
			cout << "The node is null" << endl;
			return false;
		}

		node->findValueWithKey(key, result, type);

		return true;
	}
	//not equal
	/*if (type == 1) {
	
	}*/
	// litter than
	else if (type == 2) {
		while (node != nullptr && !node->isLeaf()) {
			nodeNumber = node->getPageWithIndex(0);
			delete node;
			node = getNode(nodeNumber);
		}

		if (node == nullptr) {
			cout << "The node is null" << endl;
			return false;
		}

		while (node != nullptr && node->findValueWithKey(key, result, type)) {
			nodeNumber = node->getLastPointer();
			delete node;
			node = getNode(nodeNumber);
		}

		return true;
	}
	// bigger than
	else if (type == 3) {
		while (node != nullptr && !node->isLeaf()) {
			nodeNumber = node->findChildWithKey(key);
			delete node;
			node = getNode(nodeNumber);
		}

		if (node == nullptr) {
			cout << "The node is null" << endl;
			return false;
		}

		while (node != nullptr && node->findValueWithKey(key, result, type)) {
			nodeNumber = node->getLastPointer();
			delete node;
			node = getNode(nodeNumber);
		}

		return true;
	}
	// litter than or equal to
	else if (type == 4) {
		while (node != nullptr && !node->isLeaf()) {
			nodeNumber = node->getPageWithIndex(0);
			delete node;
			node = getNode(nodeNumber);
		}

		if (node == nullptr) {
			cout << "The node is null" << endl;
			return false;
		}

		while (node != nullptr && node->findValueWithKey(key, result, type)) {
			nodeNumber = node->getLastPointer();
			delete node;
			node = getNode(nodeNumber);
		}

		return true;
	}
	else if (type == 5) {
		while (node != nullptr && !node->isLeaf()) {
			nodeNumber = node->findChildWithKey(key);
			delete node;
			node = getNode(nodeNumber);
		}

		if (node == nullptr) {
			cout << "The node is null" << endl;
			return false;
		}

		while (node != nullptr && node->findValueWithKey(key, result, type)) {
			nodeNumber = node->getLastPointer();
			delete node;
			node = getNode(nodeNumber);
		}

		return true;
	}
	else {
		cout << "Wrong type to find with index !" << endl;
		return false;
	}
}

template<typename keyType>
void BplusTree<keyType>::writeToDisk()
{
	Block* _block = bufferManager.GetFileBlock(indexFileName, 0);
	_block->SetPin(true);
	_block->SetDirty(true);

	memcpy(_block->data + KEY_LENGTH_OFFSET, &keyLength, KEY_LENGTH_SIZE);
	memcpy(_block->data + NODE_COUNT_OFFSET, &nodeCount, NODE_COUNT_SIZE);
	memcpy(_block->data + ROOT_POINTER_OFFSET, &root, ROOT_POINTER_SIZE);
	memcpy(_block->data + FREE_PAGE_COUNT_OFFSET, &freePageCount, FREE_PAGE_COUNT_SIZE);

	int pageOffset = TREE_HEADER_SIZE;
	int step = sizeof(uint32_t);
	for (uint32_t i = 0; i < freePageCount; i++) {
		memcpy(_block->data + pageOffset, &freePages[i], sizeof(uint32_t));

		pageOffset += step;
	}

	_block->SetPin(false);
	bufferManager.SaveFile(indexFileName);
}

template<typename keyType>
void BplusTree<keyType>::printTree()
{
	cout << "��ǰ����Ϊ:" << indexFileName << " ���ڵ�Ϊ " << root << endl;
	int size = bufferManager.GetFileSize(indexFileName);
	for (uint32_t i = 1; i < size; i++) {
		if (find(freePages.begin(), freePages.end(), i) == freePages.end()) {
			BplusTreeNode<keyType>* node = getNode(i);
			node->printNode();
			delete node;
		}
	}
}

template<typename keyType>
void BplusTree<keyType>::printLeaves()
{
	int size = bufferManager.GetFileSize(indexFileName);
	for (int i = 1; i < size; i++) {
		BplusTreeNode<keyType>* node = getNode(i);
		if (node->isLeaf()) node->printNode();
		delete node;
	}
}

#endif // !MINISQL_BPLUSTREE_H
