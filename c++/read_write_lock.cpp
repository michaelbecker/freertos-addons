#include "read_write_lock.hpp"


using namespace rtos_cpp;


ReadWriteLock::ReadWriteLock()
    : ReadCount(0)
{
    ReadLock = xSemaphoreCreateMutex();
    if (ReadLock == NULL) {
        throw ReadWriteLockCreateException();
    }

    ResourceLock = xSemaphoreCreateMutex();
    if (ResourceLock == NULL) {
        vSemaphoreDelete(ReadLock);
        throw ReadWriteLockCreateException();
    }
}


ReadWriteLock::~ReadWriteLock()
{
    vSemaphoreDelete(ReadLock);
    vSemaphoreDelete(ResourceLock);
}


void ReadWriteLockPreferReader::ReaderLock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount++;
    if (ReadCount == 1) {
        xSemaphoreTake(ResourceLock, portMAX_DELAY);
    }

    xSemaphoreGive(ReadLock);
}


void ReadWriteLockPreferReader::ReaderUnlock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount--;
    if (ReadCount == 0) {
        xSemaphoreGive(ResourceLock);
    }

    xSemaphoreGive(ReadLock);
}


void ReadWriteLockPreferReader::WriterLock()
{
    xSemaphoreTake(ResourceLock, portMAX_DELAY);
}


void ReadWriteLockPreferReader::WriterUnlock()
{
    xSemaphoreGive(ResourceLock);
}


ReadWriteLockPreferWriter::ReadWriteLockPreferWriter()
    : ReadWriteLock()
{
    WriteLock = xSemaphoreCreateMutex();
    if (WriteLock == NULL) {
        throw ReadWriteLockCreateException();
    }

    BlockReadersLock = xSemaphoreCreateMutex();
    if (BlockReadersLock == NULL) {
        vSemaphoreDelete(WriteLock);
        throw ReadWriteLockCreateException();
    }
}


ReadWriteLockPreferWriter::~ReadWriteLockPreferWriter()
{
    vSemaphoreDelete(WriteLock);
    vSemaphoreDelete(BlockReadersLock);
}


void ReadWriteLockPreferWriter::ReaderLock()
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


void ReadWriteLockPreferWriter::ReaderUnlock()
{
    xSemaphoreTake(ReadLock, portMAX_DELAY);

    ReadCount--;
    if (ReadCount == 0) {
        xSemaphoreGive(ResourceLock);
    }

    xSemaphoreGive(ReadLock);
}


void ReadWriteLockPreferWriter::WriterLock()
{
    xSemaphoreTake(WriteLock, portMAX_DELAY);

    WriteCount++;
    if (WriteCount == 1) {
        xSemaphoreTake(BlockReadersLock, portMAX_DELAY);
    }

    xSemaphoreGive(WriteLock);

    xSemaphoreTake(ResourceLock, portMAX_DELAY);
}


void ReadWriteLockPreferWriter::WriterUnlock()
{
    xSemaphoreGive(ResourceLock);

    xSemaphoreTake(WriteLock, portMAX_DELAY);

    WriteCount--;
    if (WriteCount == 0) {
        xSemaphoreGive(BlockReadersLock);
    }

    xSemaphoreGive(WriteLock);
}


