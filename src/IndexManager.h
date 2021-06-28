/****************************************************************************
*																			*
*  @file     IndexManager.h													*
*  @brief    Define the indexManger											*
*  @author   麻钰茗															*
*																			*
*  @interfaces:																*
*	---------------------------------------------------------------------	*
*	bool createIndex(string _tableName, Attribute _attribute)				*
*	@brief: 在目标表的独特属性上创建索引										*
*	@params:																*
*		string _tableName: 需要创建索引的表名									*
*		Attribute _attribute: 需要创建索引的属性								*
*	@return:																*
*		true: 创建索引成功													*
*		false: 创建索引失败													*
*	---------------------------------------------------------------------	*
*	bool insertIndex(string _tableName, Attribute _attribute,				*
*						char* _key, int _value)								*
*	@brief: 在目标表的独特属性上插入索引										*
*	@params:																*
*		string _tableName: 需要创建索引的表名									*
*		Attribute _attribute: 需要创建索引的属性								*
*		const void* _key: 插入的索引键键值（需要用&符号）						*
*		int _value：被索引的行数据所在的block号								*
*	@return:																*
*		true: 插入索引成功													*
*		false: 插入索引失败													*
*	---------------------------------------------------------------------	*
*	bool modifyIndex(string _tableName, Attribute _attribute,				*
*						char* _key, int _value)								*
*	@brief: 在目标表的独特属性上的索引进行修改									*
*	@params:																*
*		string _tableName: 需要修改索引的表名									*
*		Attribute _attribute: 需要修改索引的属性								*
*		const void* key: 修改的索引键键值（需要用&符号）						*
*		int value：被索引的行数据所在的block号									*
*	@return:																*
*		true: 修改索引成功													*
*		false: 修改索引失败													*
*	---------------------------------------------------------------------	*
*	bool deleteIndex(string _tableName, Attribute _attribute,				*
*						char* _key)											*
*	@brief: 对目标表的独特属性上的某个索引进行删除								*
*	@params:																*
*		string _tableName: 需要删除索引的表名									*
*		Attribute _attribute: 需要删除索引的属性								*
*		const void* key: 删除的索引键键值（需要用&符号）						*
*	@return:																*
*		true: 删除索引成功													*
*		false: 删除索引失败													*
*	---------------------------------------------------------------------	*
*	int findIndexBlockNumber(string _tableName, Attribute _attribute,		*
*							char* _key)										*
*	@brief: 在创建好索引的属性上获取block号									*
*	@params:																*
*		string _tableName: 需要查询的表名										*
*		Attribute _attribute: 需要查询的属性									*
*		const void* key: 查询的索引键键值（需要用&符号）						*
*	@return:																*
*		-1：查询中出错														*
*		others: 查询到的块号													*
*	---------------------------------------------------------------------	*
*	bool dropIndex(string _tableName, Attribute _attribute)					*
*	@brief: 将某个索引完全删除												*
*	@params:																*
*		string _tableName: 需要删除的索引的表名								*
*		Attribute _attribute: 需要删除的索引的属性							*
*	@return:																*
*		true: 删除索引成功													*
*		false: 删除索引失败													*
*																			*
*****************************************************************************/
#ifndef MINISQL_INDEXMANAGER_H
#define MINISQL_INDEXMANAGER_H
#include "BPlusTree.hpp"

class IndexManager {
private:
	const uint8_t NODE_HEADER_SIZE = sizeof(uint32_t) * 3 + sizeof(uint8_t);
	uint32_t getKeyLength(Attribute _attribute);
public:
	IndexManager();
	~IndexManager();

	bool createIndex(string _tableName, Attribute _attribute);
	bool insertIndex(string _tableName, Attribute _attribute, char* _key, RecordPosition _value);
	bool modifyIndex(string _tableName, Attribute _attribute, char* _key, RecordPosition _value);
	bool deleteIndex(string _tableName, Attribute _attribute, char* _key);
	vector<RecordPosition> findIndexBlockNumber(string _tableName, Attribute _attribute, char* _key, int type);
	bool dropIndex(string _tableName, Attribute _attribute);
	bool saveIndex(string _tableName, Attribute _attribute);
};

#endif // !MINISQL_INDEXMANAGER_H