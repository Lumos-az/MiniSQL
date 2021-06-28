/****************************************************************************
*																			*
*  @file     IndexManger.cpp												*
*  @brief    实现IndexManager的具体功能										*
*  @author   麻钰茗															*
*																			*
*****************************************************************************/
#include "IndexManager.h"
#define CHAR2INT(array) *(int*)(array)
#define CHAR2FLOAT(array) *(float*)(array)
#define CHAR2STRING(array) *(string*)(array)
using namespace std;
//**************The implementation of the class IndexManager***************


/*
* @brief: Constructor of the indexManager
*
* @params:
*	_bm: The pointer of buffer manager.
*/
IndexManager::IndexManager()
{

}

/*
* @brief: Destructor of the B+Tree node
*/
IndexManager::~IndexManager()
{

}

/*
* @brief: Calculating the degree of the tree
*
* @param: the attribute to create index.
* @return: the degree of the tree.
*/
uint32_t IndexManager::getKeyLength(Attribute _attribute)
{
	int keyLength = 0;
	switch (_attribute.dataType)
	{
	case 0:
		keyLength = sizeof(int);
		break;
	case 1:
		keyLength = sizeof(float);
		break;
	case 2:
		keyLength = _attribute.charSize;
		break;
	default:
		break;
	}
	if (keyLength == 0) {
		cout << "The data type or char size is wrong" << endl;
	}

	return keyLength;
}

/*
* @brief: Create a index file on an attribute.
*
* @params:
*	_tableName: the table to create index.
*	_attribute: the attribute to create index.
* @returns:
*	true: success.
*	false: failed.
*/
bool IndexManager::createIndex(string _tableName, Attribute _attribute)
{
	string indexFileName = _tableName + "_" + _attribute.name + ".index";
	bufferManager.CreateFile(indexFileName);

	switch (_attribute.dataType) {
	case 0: {
		BplusTree<int> bptree = BplusTree<int>(_tableName, _attribute.name, getKeyLength(_attribute));
	}
		break;
	case 1: {
		BplusTree<float> bptree = BplusTree<float>(_tableName, _attribute.name, getKeyLength(_attribute));
	}
		break;
	case 2: {
		BplusTree<string> bptree = BplusTree<string>(_tableName, _attribute.name, getKeyLength(_attribute));
	}
		break;
	default:
		cout << "wrong attribute type" << endl;
		return false;
		break;
	}
	return true;
}

/*
* @brief: Insert a index file on an attribute.
*
* @params:
*	_tableName: the table to create index.
*	_attribute: the attribute to create index.
*	_key: the key to find the value.
*	_value: the block number of the record.
* @returns:
*	true: success.
*	false: failed.
*/
bool IndexManager::insertIndex(string _tableName, Attribute _attribute, char* _key, RecordPosition _value)
{
	switch (_attribute.dataType) {
	case 0: {
		BplusTree<int> bptree = BplusTree<int>(_tableName, _attribute.name);
		bptree.insertValueWithKey(CHAR2INT(_key), _value); 
	}
		break;
	case 1:{
		BplusTree<float> bptree = BplusTree<float>(_tableName, _attribute.name);
		bptree.insertValueWithKey(CHAR2FLOAT(_key), _value); 
	}
		break;
	case 2: {
		BplusTree<string> bptree = BplusTree<string>(_tableName, _attribute.name);
		bptree.insertValueWithKey(CHAR2STRING(_key), _value);
	}
		break;
	default:
		cout << "wrong attribute type" << endl;
		return false;
		break;
	}
	return true;
}

/*
* @brief: Modify a index file on an attribute.
*
* @params:
*	_tableName: the table to create index.
*	_attribute: the attribute to create index.
*	_key: the key to find the value.
*	_value: the block number of the record.
* @returns:
*	true: success.
*	false: failed.
*/
bool IndexManager::modifyIndex(string _tableName, Attribute _attribute, char* _key, RecordPosition _value)
{
	switch (_attribute.dataType) {
	case 0: {
		BplusTree<int> bptree = BplusTree<int>(_tableName, _attribute.name);
		bptree.modifyValueWithKey(CHAR2INT(_key), _value);
	}
		break;
	case 1: {
		BplusTree<float> bptree = BplusTree<float>(_tableName, _attribute.name);
		bptree.modifyValueWithKey(CHAR2FLOAT(_key), _value);
	}
		break;
	case 2: {
		BplusTree<string> bptree = BplusTree<string>(_tableName, _attribute.name);
		bptree.modifyValueWithKey(CHAR2STRING(_key), _value);
	}
		break;
	default:
		cout << "wrong attribute type" << endl;
		return false;
		break;
	}


	return true;
}

/*
* @brief: delete a index file on an attribute.
*
* @params:
*	_tableName: the table to create index.
*	_attribute: the attribute to create index.
*	_key: the key to find the value.
* @returns:
*	true: success.
*	false: failed.
*/
bool IndexManager::deleteIndex(string _tableName, Attribute _attribute, char* _key)
{
	switch (_attribute.dataType) {
	case 0: {
		BplusTree<int> bptree = BplusTree<int>(_tableName, _attribute.name);
		bptree.deleteValueWithKey(CHAR2INT(_key));
	}
		break;
	case 1: {
		BplusTree<float> bptree = BplusTree<float>(_tableName, _attribute.name);
		bptree.deleteValueWithKey(CHAR2FLOAT(_key));
	}
		break;
	case 2: {
		BplusTree<string> bptree = BplusTree<string>(_tableName, _attribute.name);
		bptree.deleteValueWithKey(CHAR2STRING(_key));
	}
		break;
	default:
		cout << "wrong attribute type" << endl;
		return false;
		break;
	}


	return true;
}

/*
* @brief: Find the block number though index file.
*
* @params:
*	_tableName: the table to create index.
*	_attribute: the attribute to create index.
*	_key: the key to find the value.
* @returns:
*	-1: error.
*	others: find the block number.
*/
vector<RecordPosition> IndexManager::findIndexBlockNumber(string _tableName, Attribute _attribute, char* _key, int type)
{
	vector<RecordPosition> result;

	switch (_attribute.dataType) {
	case 0: {
		BplusTree<int> bptree = BplusTree<int>(_tableName, _attribute.name);
		bptree.findValueWithKey(CHAR2INT(_key),result,type);
	}
		break;
	case 1: {
		BplusTree<float> bptree = BplusTree<float>(_tableName, _attribute.name);
		bptree.findValueWithKey(CHAR2FLOAT(_key),result,type);
	}
		break;
	case 2: {
		BplusTree<string> bptree = BplusTree<string>(_tableName, _attribute.name);
		bptree.findValueWithKey(CHAR2STRING(_key),result,type);
	}
		break;
	default:
		cout << "wrong attribute type" << endl;
		break;
	}

	return result;
}

/*
* @brief: Drop all of the index file on the attribute.
*
* @params:
*	_tableName: the table to create index.
*	_attribute: the attribute to create index.
*	_key: the key to find the value.
* @returns:
*	-1: error.
*	others: find the block number.
*/
bool IndexManager::dropIndex(string _tableName, Attribute _attribute)
{
	string indexFileName = _tableName + "_" + _attribute.name + ".index";

	bufferManager.DeleteFile(indexFileName);
	return true;
}

bool IndexManager::saveIndex(string _tableName, Attribute _attribute)
{
	string indexFileName = _tableName + "_" + _attribute.name + ".index";

	bufferManager.SaveFile(indexFileName);
	return true;
}