#include "BufferManager.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <cstring>
//#include <unistd.h>

using namespace std;

BufferManager bufferManager;

//*****Block*****
Block::Block(Buffer* _buffer): buffer(_buffer){
    SetSize(HEAD_SIZE);
}

int Block::GetSize(){
    int size;
    //buffer->lastUsedTime = clock();
    memcpy(&size, data, HEAD_SIZE);
    return size;
}

void Block::SetSize(int size){
    //buffer->lastUsedTime = clock();
    memcpy(data, &size, HEAD_SIZE);
}

void Block::SetPin(bool flag){
    //buffer->lastUsedTime = clock();
    buffer->pinned = flag;
}

void Block::SetDirty(bool flag){
    //buffer->lastUsedTime = clock();
    buffer->dirty = flag;
}

void Block::Display(){
    //buffer->lastUsedTime = clock();
    for (int i = 0; i < 4096; i++) {
        cout << hex << (int)data[i]<<" ";
    }
    cout << endl;
}

vector<string>* Block::GetRecords(int n, int k, int h){
    int size, i;
    vector<string>* resultSet = NULL;
    size = GetSize();

    if((k-n)*h <= size){
        resultSet = new vector<string>();
        
        for(i = n; i < k; i++){
            string record(data + HEAD_SIZE + i * h, h);
            resultSet->push_back(record);
        }
    }

    return resultSet;
}

//*****FreeBuffer*****

FreeBuffer::FreeBuffer(int _bufferIdx, FreeBuffer* _next):bufferIdx(_bufferIdx), next(_next){} 

FreeBuffer::~FreeBuffer(){
    delete next;
}


Buffer::Buffer():used(false), dirty(false), pinned(false), fileName(""), idx(-1), lastUsedTime(0), created(false){
    block = new Block(this);
}

Buffer::~Buffer(){
    delete block;
}

//*****BufferManager*****


BufferManager::BufferManager(){
    bufferPool = new Buffer*[BUFFER_SIZE];

    freeBufferList = NULL;
    for(int i = 0; i < BUFFER_SIZE; i++){
        bufferPool[i] = new Buffer();

        FreeBuffer* freeBuffer = new FreeBuffer(i, freeBufferList);
        freeBufferList = freeBuffer;        
    }

//    LoadFile(TABLE_INFO_PATH);

    fstream os;
    int fileSize;
    Block *block;

    os.open(TABLE_INFO_PATH);
    if(os.is_open()){
        os.close();
        LoadFile(TABLE_INFO_PATH);
    }
    else{
        os.close();
        CreateFile(TABLE_INFO_PATH);
    }


    fileSize = GetFileSize(TABLE_INFO_PATH);
    for(int i = 0; i < fileSize; i++){
        block = GetFileBlock(TABLE_INFO_PATH, i);
        block->SetPin(true);
    }
}

BufferManager::~BufferManager(){
    SaveFile(TABLE_INFO_PATH);
//    cout << "..." << endl;
    for(int i = 0; i < BUFFER_SIZE; i++){
        if(bufferPool[i]->dirty){
            fstream os ;
            os.open(bufferPool[i]->fileName);
            WriteBlockToDisk(os, bufferPool[i]->idx, bufferPool[i]->block->data);
            os.close();
        }
        delete bufferPool[i];
    }

    delete[]  bufferPool;
    delete freeBufferList;
}

int BufferManager::LoadFile(const string& fileName){
    int size = -1, bufferIdx;
    ifstream is;
    is.open(fileName, ios::in);

    if(is.is_open()){
        //calculate the number of blocks in file
        is.seekg(0, ios_base::end);
        size = ((int)is.tellg() + BLOCK_SIZE - 1) / BLOCK_SIZE;
        //occupy the first block of file in bufferPool, get the bufferIdx 
        bufferIdx = FetchBlockFromDisk(is, 0);

        is.close();
        if(bufferIdx != -1){
            //update state
            bufferPool[bufferIdx]->fileName = fileName;
            fileBuffer[fileName][0] = bufferIdx;
            fileSize[fileName] = size;

            //return how many blocks in the file
            return size;
        }
        else{
            //Bufferpool is full.
            cout<<"[WARN]: Load fail. Bufferpool is full. Unpin some buffer."<<endl;
            return -1;
        }
        
    }
    else{
        //No such file, create one.
        //cout<<"[WARN]: No such file.\""<<fileName<<"\" will be created."<<endl;
        bool ret = CreateFile(fileName);
        if(ret){
            return 1;
        }
        else{
            return -1;
        }
    }
    
}

void BufferManager::ResetFile(const string& fileName){
    vector<int> bufferIdxVec;
    int bufferIdx, firstBufferIdx;
    ifstream is;
    ofstream os;
    //Block* block;
    
    if(fileBuffer.find(fileName) != fileBuffer.end()){
        //block = GetFileBlock(fileName, 0);
        
        if(fileBuffer[fileName].find(0) == fileBuffer[fileName].end()){
            
            is.open(fileName);
            firstBufferIdx = FetchBlockFromDisk(is, 0);
            is.close();
            bufferPool[firstBufferIdx]->fileName = fileName;
            fileBuffer[fileName][0] = firstBufferIdx;
        }
        else{
            firstBufferIdx = fileBuffer[fileName][0];
        }

        bufferPool[firstBufferIdx]->pinned = true;

        //Clear the block
        for (auto iter = fileBuffer[fileName].begin(); iter != fileBuffer[fileName].end(); ++iter){
            if(iter->first != 0){
                bufferIdxVec.push_back(iter->second);
            }
        }
        for (auto iter = bufferIdxVec.begin(); iter != bufferIdxVec.end(); ++iter) {
            bufferIdx = *iter;
            FreeBlock(bufferIdx);
        }

        //Make the first block empty
        bufferPool[firstBufferIdx]->block->SetSize(4);

        os.open(fileName);
        os.seekp(0, os.beg);
        os.write(bufferPool[firstBufferIdx]->block->data, BLOCK_SIZE);
        os.close();

        
        fileSize[fileName] = 1;
        bufferPool[firstBufferIdx]->lastUsedTime = clock();
        bufferPool[firstBufferIdx]->dirty = false;
        bufferPool[firstBufferIdx]->pinned = false;
    }
}


bool BufferManager::IsFileRegistered(const string& fileName){
    if(fileBuffer.find(fileName) != fileBuffer.end()){
        //file is registered
        return true;
    }
    else{
        return false;
    }
}

bool BufferManager::IsFileExist(const string& fileName){
    ifstream is;
    bool ret;
    
    is.open(fileName);
    ret = is.is_open();
    is.close();

    return ret;
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
        bufferPool[bufferIdx]->lastUsedTime = clock();
        
        //bufferPool[bufferIdx]->dirty = true;
        //bufferPool[bufferIdx]->pinned = true;

        return bufferPool[bufferIdx]->block;
    }
    else{
        //illeagl condition
        return NULL;
    }

}

bool BufferManager::CreateFile(const string& fileName){
    int bufferIdx = GetFreeBufferIdx();
    if(bufferIdx != -1){
        bufferPool[bufferIdx]->used = true;
        bufferPool[bufferIdx]->dirty = true;
        bufferPool[bufferIdx]->created = true;   
        bufferPool[bufferIdx]->idx = 0;
        bufferPool[bufferIdx]->fileName = fileName;
        bufferPool[bufferIdx]->lastUsedTime = clock();

        
        fileBuffer[fileName][0] = bufferIdx;
        fileSize[fileName] = 1;
        return true;
    }
    else{
        return false;
    }

}

void BufferManager::AppendFile(const string& fileName){
    int bufferIdx = GetFreeBufferIdx();

    bufferPool[bufferIdx]->used = true;
    bufferPool[bufferIdx]->dirty = true;
    bufferPool[bufferIdx]->created = true;   
    bufferPool[bufferIdx]->idx = fileSize[fileName];
    bufferPool[bufferIdx]->fileName = fileName;
    bufferPool[bufferIdx]->lastUsedTime = clock();

    fileBuffer[fileName][fileSize[fileName]] = bufferIdx;
    
    fileSize[fileName] = fileSize[fileName] + 1;
    //if(fileAppendCount.find(fileName) != fileAppendCount.end()){
    //   fileAppendCount[fileName] += 1;
    //}
    //else{
    //    fileAppendCount[fileName] += 1;
    //}
}

void BufferManager::AppendFile(const string& fileName, int n){
    for (int i = 0; i < n; i++){
        AppendFile(fileName);
    }
}

void BufferManager::DeleteFileLastBlock(const string& fileName){
    int size, bufferIdx;
    if(fileBuffer.find(fileName) != fileBuffer.end()){
        size = fileSize[fileName];
        //Check if the last block of the file is in fileBuffer 
        if(fileBuffer[fileName].find(size-1) != fileBuffer[fileName].end()){
            bufferIdx = fileBuffer[fileName][size-1];
            FreeBlock(bufferIdx);
        }
        fileSize[fileName] = size - 1;
    }
}

void BufferManager::SaveFile(const string& fileName){
    
    ofstream of;
    fstream os;
    int idx, bufferIdx, size;
    vector<int> idxVec;
    
    os.open(fileName);
    if(os.is_open()){
        //the file exists
        //check if some block of the file is deleted
        os.seekp(0, ios_base::end);
        //size = ((int)os.tellp() + BLOCK_SIZE - 1) / BLOCK_SIZE;
        
        size = ((int)os.tellp() + BLOCK_SIZE - 1) / BLOCK_SIZE;
        
        os.close();
        //truncate the file
        if(size > fileSize[fileName]){
            //truncate(fileName.c_str(), fileSize[fileName]*BLOCK_SIZE);
        }
        os.open(fileName);
        if(fileBuffer.find(fileName) != fileBuffer.end()){
            
            for (auto iter = fileBuffer[fileName].begin(); iter != fileBuffer[fileName].end(); ++iter){
                idxVec.push_back(iter->first);
            }
            for (auto iter = idxVec.begin(); iter != idxVec.end(); ++iter) {
                idx = *iter;
                bufferIdx = fileBuffer[fileName][idx];   
                if(bufferPool[bufferIdx]->dirty){
                    WriteBlockToDisk(os, idx, bufferPool[bufferIdx]->block->data);
                }
                bufferPool[bufferIdx]->dirty = false;
                //FreeBlock(bufferIdx);
            }
        }

        os.close();
    }
    else{
        //the file does not exist
        of.open(fileName);
            for (auto iter = fileBuffer[fileName].begin(); iter != fileBuffer[fileName].end(); ++iter){
                idxVec.push_back(iter->first);
            }
            for (auto iter = idxVec.begin(); iter != idxVec.end(); ++iter) {
                idx = *iter;
                bufferIdx = fileBuffer[fileName][idx];   
                if(bufferPool[bufferIdx]->dirty){
                    WriteNewBlockToDisk(of, idx, bufferPool[bufferIdx]->block->data);
                }
                bufferPool[bufferIdx]->dirty = false;
                //FreeBlock(bufferIdx);
            }
        of.close();
    }
}

void BufferManager::ClearFileBlock(const string& fileName){
    vector<int> idxVec;
    int idx;
    if(fileBuffer.find(fileName) != fileBuffer.end()){
        for (auto iter = fileBuffer[fileName].begin(); iter != fileBuffer[fileName].end(); ++iter){
            idxVec.push_back(iter->second);
        }
        for (auto iter = idxVec.begin(); iter != idxVec.end(); ++iter) {
            idx = *iter;
            FreeBlock(idx);
        }
    }
}

bool BufferManager::DeleteFile(const string& fileName){
    int ret;
    ClearFileBlock(fileName);
    ret = remove(fileName.c_str());

    if(ret == 0){
        return true;
    }
    else{
        return false;
    }
}

 void BufferManager::DisplayBuffers(){
     for(int i = 0; i < BUFFER_SIZE; i++){
         cout<<"BufferIdx:["<<i<<"]:"<<" Used:"<<bufferPool[i]->used<<" Dirty:"<<bufferPool[i]->dirty<<" Time:"<<bufferPool[i]->lastUsedTime<<" Pinned:"<<bufferPool[i]->pinned<<" FileName:"<<bufferPool[i]->fileName<<" Idx: "<<bufferPool[i]->idx<<endl;
     }
 }

int BufferManager::LRU(){
    int ret = -1;
    clock_t lruTime = clock(); 

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
    if(bufferIdx != -1){
        is.read((char*)(bufferPool[bufferIdx]->block), BLOCK_SIZE);
        bufferPool[bufferIdx]->used = true;   
        bufferPool[bufferIdx]->created = false;
        bufferPool[bufferIdx]->idx = idx;
        bufferPool[bufferIdx]->lastUsedTime = clock();
    }
    
    return bufferIdx;  
}

void BufferManager::WriteBlockToDisk(fstream &os, int idx, const char* blockData){
    os.seekp(idx*BLOCK_SIZE, os.beg);
    os.write(blockData, BLOCK_SIZE);
}

void BufferManager::WriteNewBlockToDisk(ofstream &of, int idx, const char* blockData){
    of.seekp(idx*BLOCK_SIZE, of.beg);
    of.write(blockData, BLOCK_SIZE);
}

void BufferManager::FreeBlock(int bufferIdx){
    string fileName;
    
    bufferPool[bufferIdx]->used = false;
    bufferPool[bufferIdx]->dirty = false;
    bufferPool[bufferIdx]->pinned = false;
    bufferPool[bufferIdx]->created = false;
    bufferPool[bufferIdx]->lastUsedTime = 0;
    bufferPool[bufferIdx]->block->SetSize(HEAD_SIZE);
    fileName = bufferPool[bufferIdx]->fileName;
    fileBuffer[fileName].erase(bufferPool[bufferIdx]->idx);
    //cout<<"buffer "<<bufferIdx<<" is released"<<endl;
    
    if(fileBuffer[fileName].empty()){
        fileBuffer.erase(fileName);
        fileSize.erase(fileName);
        //cout<< "[REMIND]: There is no block of \""<< fileName << "\" remains in bufferPool. This could be caused by LRU, delete, save or clear operation."<<endl;
    }
    
    bufferPool[bufferIdx]->fileName = "";
    bufferPool[bufferIdx]->idx = -1;

    FreeBuffer* freeBuffer = new FreeBuffer(bufferIdx, freeBufferList);
    freeBufferList = freeBuffer;

}

int BufferManager::GetFreeBufferIdx(){
    int bufferIdx, ret;
    if(freeBufferList == NULL){
        bufferIdx = LRU();
        if(bufferIdx == -1){
            //No buffer can be released with LRU
            cout<<"[ERROR]: No buffer can bu released. Unpin some buffers."<<endl;
            return -1; 
        }
        if(bufferPool[bufferIdx]->dirty){
            fstream os;
            os.open(bufferPool[bufferIdx]->fileName);
            if(os.is_open()){
                WriteBlockToDisk(os, bufferPool[bufferIdx]->idx, bufferPool[bufferIdx]->block->data);
                os.close();
            }
            else{
                //file doesn't exist
                ofstream of;
                of.open(bufferPool[bufferIdx]->fileName);
                WriteNewBlockToDisk(of, bufferPool[bufferIdx]->idx, bufferPool[bufferIdx]->block->data);
                of.close();
            }
            
        }
        FreeBlock(bufferIdx);
    }

    ret = freeBufferList->bufferIdx;
    freeBufferList = freeBufferList->next;

    return ret;
}
