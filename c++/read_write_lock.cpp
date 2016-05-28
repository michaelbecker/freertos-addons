#include "read_write_lock.hpp"

CReadWriteLock::CReadWriteLock()
    : ReadCount(0)
{
    ReadLock = xSemaphoreCreateMutex();
    if (ReadLock == NULL) {
        throw CReadWriteLockCreationException();
    }

    ResourceLock = xSemaphoreCreateMutex();
    if (ResourceLock == NULL) {
        vSemaphoreDelete(ReadLock);
        throw CReadWriteLockCreationException();
    }
}


CReadWriteLock::~CReadWriteLock()
{
    vSemaphoreDelete(ReadLock);
    vSemaphoreDelete(ResourceLock);
}


void CReadWriteLockPreferReader::ReaderLock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount++;
    if (ReadCount == 1) {
        xSemaphoreTake(ResourceLock, portMAX_DELAY);
    }

    xSemaphoreGive(ReadLock);
}


void CReadWriteLockPreferReader::ReaderUnlock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount--;
    if (ReadCount == 0) {
        xSemaphoreGive(ResourceLock);
    }

    xSemaphoreGive(ReadLock);
}


void CReadWriteLockPreferReader::WriterLock()
{
    xSemaphoreTake(ResourceLock, portMAX_DELAY);
}


void CReadWriteLockPreferReader::WriterUnlock()
{
    xSemaphoreGive(ResourceLock);
}


CReadWriteLockPreferWriter::CReadWriteLockPreferWriter()
    : CReadWriteLock()
{
    WriteLock = xSemaphoreCreateMutex();
    if (WriteLock == NULL) {
        throw CReadWriteLockCreationException();
    }

    BlockReadersLock = xSemaphoreCreateMutex();
    if (BlockReadersLock == NULL) {
        vSemaphoreDelete(WriteLock);
        throw CReadWriteLockCreationException();
    }
}


CReadWriteLockPreferWriter::~CReadWriteLockPreferWriter()
{
    vSemaphoreDelete(WriteLock);
    vSemaphoreDelete(BlockReadersLock);
}


void CReadWriteLockPreferWriter::ReaderLock()
{
    xSemaphoreTake(BlockReadersLock, portMAX_DELAY);
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount++;
    if (ReadCount == 1) {
        xSemaphoreTake(ResourceLock, portMAX_DELAY);
    }

    xSemaphoreGive(ReadLock);
    xSemaphoreGive(BlockReadersLock);
}


void CReadWriteLockPreferWriter::ReaderUnlock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount--;
    if (ReadCount == 0) {
        xSemaphoreGive(ResourceLock);
    }

    xSemaphoreGive(ReadLock);
}


void CReadWriteLockPreferWriter::WriterLock()
{
    xSemaphoreTake(WriteLock, portMAX_DELAY);

    WriteCount++;
    if (WriteCount == 1) {
        xSemaphoreTake(BlockReadersLock, portMAX_DELAY);
    }

    xSemaphoreGive(WriteLock);

    xSemaphoreTake(ResourceLock, portMAX_DELAY);
}


void CReadWriteLockPreferWriter::WriterUnlock()
{
    xSemaphoreGive(ResourceLock);

    xSemaphoreTake(WriteLock, portMAX_DELAY);

    WriteCount--;
    if (WriteCount == 0) {
        xSemaphoreGive(BlockReadersLock);
    }
    
    xSemaphoreGive(WriteLock);
}



