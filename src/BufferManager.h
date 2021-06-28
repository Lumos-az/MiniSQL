#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

#define BUFFER_SIZE 4096
#define BLOCK_SIZE 4096
#define HEAD_SIZE 4
#define TABLE_INFO_PATH "../data/TableInfo.txt"


class Buffer;
class BufferManager;

extern BufferManager bufferManager;

class Block {
    public:
        //|--4 byte--|---4092 byte---|
        //   size          data
        // size = 已用空间
        char data[BLOCK_SIZE];

        Block(Buffer* _buffer);

        int GetSize();
        void SetSize(int size);

        void SetPin(bool flag);
        void SetDirty(bool flag);

        void Display();
        //Return [n, k) records from block, the number of bytes of a record is h.  
        vector<string>* GetRecords(int n, int k, int h);
    private:
        Buffer* buffer;  
};

class FreeBuffer{
    public:
        FreeBuffer(int _bufferIdx, FreeBuffer* _next);
        ~FreeBuffer();

        int bufferIdx;
        FreeBuffer* next;
};

class Buffer{
    public:
        Buffer();
        ~Buffer();

        string fileName;
        int idx;
        bool used;
        bool dirty;
        bool pinned;
        //If the block in the buffer is not read from disk but created. 
        bool created;
        clock_t lastUsedTime;
        Block* block;
};


class BufferManager {
    public:
        //idx: the index of blocks in file
        //bufferIdx: the index of buffers in bufferPool

        //Register a existed file in BufferManager
        int LoadFile(const string& fileName);
        //Get the number of blocks in file 
        int GetFileSize(const string& fileName);
        //Get the idx block of file, must be called after LoadFile or CreateFile. If the block is modified, call SetDirty.
        Block* GetFileBlock(const string& fileName, int idx);
        //Create a new file, occupy a buffer for the new created file   
        bool CreateFile(const string& fileName);
        //Append the file with a block in its end
        void AppendFile(const string& fileName);
        //Append the file with n blocks in its end
        void AppendFile(const string& fileName, int n);
        //Delete the last block of file
        void DeleteFileLastBlock(const string& fileName);
        //Write all dirty blocks of the file in bufferPool to disk and commit AppendFile or DeleteFileLastBlock. Release all blocks no matter the block is pinned or not.
        void SaveFile(const string& fileName); 
         //The file will be reset with only one empty block resevered.
        void ResetFile(const string& fileName);
        //Release all blocks no matter the block is pinned or not.
        void ClearFileBlock(const string& fileName);
        //Delete file. If the file has no block in bufferPool, just delete it. If there are any blocks of the file in bufferPool, free them.  
        bool DeleteFile(const string& fileName); 
        //Display buffer state
        void DisplayBuffers();
        //Check if file is registered in the bufferpool.  
        bool IsFileRegistered(const string& fileName);
        //Check if the file is in the disk.
        bool IsFileExist(const string& fileName);
        //Display loaded file state
        //void DisplayFiles();
        //觉得有功能不够的话可以让我加


        BufferManager();
        ~BufferManager();
    
    private:
        Buffer** bufferPool;
        FreeBuffer* freeBufferList;
        map<string, map<int, int>> fileBuffer;
        
        map<string, int> fileSize;

        //return the index of the least recently used block, excluding the pinned block 
        int LRU();
        //
        int FetchBlockFromDisk(ifstream &is, int idx);
        //
        void WriteBlockToDisk(fstream &os, int idx, const char* block);
        //
        void WriteNewBlockToDisk(ofstream &os, int idx, const char* blockData);
        //
        void FreeBlock(int bufferIdx);
        //
        int GetFreeBufferIdx();
};



#endif