/****************************************************************************
*																			*
*  @file     IndexManager.h													*
*  @brief    Define the indexManger											*
*  @author   ������															*
*																			*
*  @interfaces:																*
*	---------------------------------------------------------------------	*
*	bool createIndex(string _tableName, Attribute _attribute)				*
*	@brief: ��Ŀ���Ķ��������ϴ�������										*
*	@params:																*
*		string _tableName: ��Ҫ���������ı���									*
*		Attribute _attribute: ��Ҫ��������������								*
*	@return:																*
*		true: ���������ɹ�													*
*		false: ��������ʧ��													*
*	---------------------------------------------------------------------	*
*	bool insertIndex(string _tableName, Attribute _attribute,				*
*						char* _key, int _value)								*
*	@brief: ��Ŀ���Ķ��������ϲ�������										*
*	@params:																*
*		string _tableName: ��Ҫ���������ı���									*
*		Attribute _attribute: ��Ҫ��������������								*
*		const void* _key: �������������ֵ����Ҫ��&���ţ�						*
*		int _value�������������������ڵ�block��								*
*	@return:																*
*		true: ���������ɹ�													*
*		false: ��������ʧ��													*
*	---------------------------------------------------------------------	*
*	bool modifyIndex(string _tableName, Attribute _attribute,				*
*						char* _key, int _value)								*
*	@brief: ��Ŀ���Ķ��������ϵ����������޸�									*
*	@params:																*
*		string _tableName: ��Ҫ�޸������ı���									*
*		Attribute _attribute: ��Ҫ�޸�����������								*
*		const void* key: �޸ĵ���������ֵ����Ҫ��&���ţ�						*
*		int value�������������������ڵ�block��									*
*	@return:																*
*		true: �޸������ɹ�													*
*		false: �޸�����ʧ��													*
*	---------------------------------------------------------------------	*
*	bool deleteIndex(string _tableName, Attribute _attribute,				*
*						char* _key)											*
*	@brief: ��Ŀ���Ķ��������ϵ�ĳ����������ɾ��								*
*	@params:																*
*		string _tableName: ��Ҫɾ�������ı���									*
*		Attribute _attribute: ��Ҫɾ������������								*
*		const void* key: ɾ������������ֵ����Ҫ��&���ţ�						*
*	@return:																*
*		true: ɾ�������ɹ�													*
*		false: ɾ������ʧ��													*
*	---------------------------------------------------------------------	*
*	int findIndexBlockNumber(string _tableName, Attribute _attribute,		*
*							char* _key)										*
*	@brief: �ڴ����������������ϻ�ȡblock��									*
*	@params:																*
*		string _tableName: ��Ҫ��ѯ�ı���										*
*		Attribute _attribute: ��Ҫ��ѯ������									*
*		const void* key: ��ѯ����������ֵ����Ҫ��&���ţ�						*
*	@return:																*
*		-1����ѯ�г���														*
*		others: ��ѯ���Ŀ��													*
*	---------------------------------------------------------------------	*
*	bool dropIndex(string _tableName, Attribute _attribute)					*
*	@brief: ��ĳ��������ȫɾ��												*
*	@params:																*
*		string _tableName: ��Ҫɾ���������ı���								*
*		Attribute _attribute: ��Ҫɾ��������������							*
*	@return:																*
*		true: ɾ�������ɹ�													*
*		false: ɾ������ʧ��													*
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