


typedef void * ReadWriteLock_t;



ReadWriteLock_t *CreateReadWriteLockPreferReader(void);

ReadWriteLock_t *CreateReadWriteLockPreferWriter(void);

void ReaderLock(ReadWriteLock_t *Lock);

void ReaderUnlock(ReadWriteLock_t *Lock);

void WriterLock(ReadWriteLock_t *Lock);

void WriterUnlock(ReadWriteLock_t *Lock);


