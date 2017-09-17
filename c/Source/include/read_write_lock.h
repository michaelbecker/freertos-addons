

#ifndef READ_WRITE_LOCK_H_
#define READ_WRITE_LOCK_H_


typedef void * ReadWriteLock_t;


ReadWriteLock_t *CreateReadWriteLockPreferReader(void);

ReadWriteLock_t *CreateReadWriteLockPreferWriter(void);

void FreeReadWriteLock(ReadWriteLock_t *lock)

void ReaderLock(ReadWriteLock_t *Lock);

void ReaderUnlock(ReadWriteLock_t *Lock);

void WriterLock(ReadWriteLock_t *Lock);

void WriterUnlock(ReadWriteLock_t *Lock);


#endif

