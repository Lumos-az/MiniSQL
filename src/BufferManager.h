#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include <map>
#include <set>
#include <string>

using namespace std;

#define BUFFER_SIZE 64
#define BLOCK_SIZE 4096

class Block {
    public:
        char data[BUFFER_SIZE];
};

class FreeBuffer{
    public:
        FreeBuffer(int _bufferIdx, FreeBuffer* _next);
    
        int bufferIdx;
        FreeBuffer* next;
};

class Buffer{
    public:
        Buffer();


        string fileName;
        int idx;
        bool used;
        bool dirty;
        bool pinned;
        time_t lastUsedTime;
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
        //Get the idx block of file, must be called after LoadFile or CreateFile. Assume you will modify the block.
        Block* GetFileBlock(const string& fileName, int idx);
        //Get the idx block of file, must be called after LoadFile or CreateFile. Read only.
        const Block* ReadFileBlock(const string& fileName, int idx);
        //Create a new file, occupy a buffer for the new created file   
        void CreateFile(const string& fileName);
        //Append the file with a block in its end
        void AppendFile(const string& fileName);
        //Delete the last block of file
        void DeleteFileLastBlock(const string& fileName);
        //Write all blocks of the file in bufferPool to disk and commit AppendFile or DeleteFileLastBlock
        void SaveFile(const string& fileName); 
        //Delete file. If the file has no block in bufferPool, just delete it. If there are any blocks of the file in bufferPool, free them.  
        void DeleteFile(const string& fileName); 
        
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
        bool WriteBlockToDisk(int bufferIdx);
        //
        void FreeBlock(int bufferIdx);
        //
        int GetFreeBufferIdx();
};


#endif