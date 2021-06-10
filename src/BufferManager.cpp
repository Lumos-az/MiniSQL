#include "BufferManager.h"
#include <fstream>
#include <ctime>
using namespace std;

FreeBuffer::FreeBuffer(int _bufferIdx, FreeBuffer* _next):bufferIdx(_bufferIdx), next(_next){} 

Buffer::Buffer():used(false), dirty(false), pinned(false), fileName(NULL), idx(-1){
    block = new Block();
}

BufferManager::BufferManager(){
    bufferPool = new Buffer*[BUFFER_SIZE];

    freeBufferList = NULL;
    for(int i = 0; i < BUFFER_SIZE; i++){
        bufferPool[i] = new Buffer();

        FreeBuffer* freeBuffer = new FreeBuffer(i, freeBufferList);
        freeBufferList = freeBuffer;        
    }
}

int BufferManager::LoadFile(const string& fileName){
    int size, bufferIdx;
    ifstream is;
    is.open(fileName, ios::in);

    //calculate the number of blocks in file
    is.seekg(0, ios_base::end);
    size = ((int)is.tellg() + BLOCK_SIZE - 1) / BLOCK_SIZE;
    //occupy the first block of file in bufferPool, get the bufferIdx 
    bufferIdx = FetchBlockFromDisk(is, 0);
    
    is.close();

    //update state
    bufferPool[bufferIdx]->fileName = fileName;
    fileBuffer[fileName][0] = bufferIdx;
    fileSize[fileName] = size;
}

int BufferManager::GetFileSize(const string& fileName){
    if(fileBuffer.find(fileName) != fileBuffer.end()){
        //file is registered
        return fileSize[fileName];
    }
    else{
        return -1;
    }
}   

Block* BufferManager::GetFileBlock(const string& fileName, int idx){
    int bufferIdx;
    ifstream is;
    if(fileBuffer.find(fileName) != fileBuffer.end() && idx >= 0 && idx < fileSize[fileName]){
        //file is registered and idx is reasonable
        if(fileBuffer[fileName].find(idx) != fileBuffer[fileName].end()){
            //block already in the bufferPool
            bufferIdx = fileBuffer[fileName][idx];
        }
        else{
            //block not in the bufferPool, fetch block from disk
            is.open(fileName, ios::in);
            bufferIdx = FetchBlockFromDisk(is, idx);
            is.close();

            //update state
            bufferPool[bufferIdx]->fileName = fileName;
            fileBuffer[fileName][idx] = bufferIdx;
        }
        bufferPool[bufferIdx]->lastUsedTime = time(NULL);
        //Assume the user will modified the block
        bufferPool[bufferIdx]->dirty = true;
        
        bufferPool[bufferIdx]->pinned = true;

        return bufferPool[bufferIdx]->block;
    }
    else{
        //illeagl condition
        return NULL;
    }

}

const Block* ReadFileBlock(const string& fileName, int idx){
    //TBD
    return NULL;
}

void BufferManager::CreateFile(const string& fileName){
    int bufferIdx = GetFreeBufferIdx();

    bufferPool[bufferIdx]->used = true;   
    bufferPool[bufferIdx]->idx = 0;
    bufferPool[bufferIdx]->fileName = fileName;
    bufferPool[bufferIdx]->lastUsedTime = time(NULL);

    
    fileBuffer[fileName][0] = bufferIdx;
    fileSize[fileName] = 1;

}

void BufferManager::AppendFile(const string& fileName){
    int bufferIdx = GetFreeBufferIdx();

    bufferPool[bufferIdx]->used = true;   
    bufferPool[bufferIdx]->idx = 0;
    bufferPool[bufferIdx]->fileName = fileName;
    bufferPool[bufferIdx]->lastUsedTime = time(NULL);

    fileBuffer[fileName][fileSize[fileName]] = bufferIdx;
    fileSize[fileName] += 1;

}

void BufferManager::DeleteFileLastBlock(const string& fileName){
    //TBD
}

int BufferManager::LRU(){
    int ret = -1;
    time_t lruTime = time(NULL); 

    for(int i = 0; i < BUFFER_SIZE; i++){
        if(bufferPool[i]->used && !bufferPool[i]->pinned){
            if(bufferPool[i]->lastUsedTime <= lruTime){
                ret = i;
                lruTime = bufferPool[i]->lastUsedTime;
            }
        }
    }
    return ret;
}

int BufferManager::FetchBlockFromDisk(ifstream &is, int idx){
    int bufferIdx;
    is.seekg(idx*BLOCK_SIZE, is.beg);
        
    bufferIdx = GetFreeBufferIdx();
    is.read((char*)(bufferPool[bufferIdx]->block), BLOCK_SIZE);
    bufferPool[bufferIdx]->used = true;   
    bufferPool[bufferIdx]->idx = idx;
    bufferPool[bufferIdx]->lastUsedTime = time(NULL);
    
    return bufferIdx;  
}

void BufferManager::FreeBlock(int bufferIdx){
    bufferPool[bufferIdx]->used = false;
    bufferPool[bufferIdx]->dirty = false;
    bufferPool[bufferIdx]->pinned = false;
    bufferPool[bufferIdx]->fileName = "";
    bufferPool[bufferIdx]->idx = -1;

    FreeBuffer* freeBuffer = new FreeBuffer(bufferIdx, freeBufferList);
    freeBufferList = freeBuffer;

}

int BufferManager::GetFreeBufferIdx(){
    int bufferIdx, ret;
    if(freeBufferList == NULL){
        bufferIdx = LRU();
        if(bufferPool[bufferIdx]->dirty){
            WriteBlockToDisk(bufferIdx);
        }
        FreeBlock(bufferIdx);
    }

    ret = freeBufferList->bufferIdx;
    freeBufferList = freeBufferList->next;

    return ret;
}
