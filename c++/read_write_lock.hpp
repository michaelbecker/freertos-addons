/**
 *
 *  https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem
 */

#ifndef READ_WRITE_LOCK_HPP_
#define READ_WRITE_LOCK_HPP_


/**
 *  This is the exception that is thrown if a CReadWriteLock constructor fails.
 */
class CReadWriteLockCreationException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        CReadWriteLockCreationException()
        {
            sprintf(errorString, "CReadWriteLock Constructor Failed");
        }

        /**
         *  Get what happened as a string. 
         *  We are overriding the base implementation here.
         */
        virtual const char *what() const throw()
        {
            return errorString;
        }

    private:
        /**
         *  A text string representing what failed.
         */
        char errorString[80];
};


class CReadWriteLock {

    public:
        CReadWriteLock();
        virtual ~CReadWriteLock();

        virtual void ReaderLock() = 0;
        virtual void ReaderUnlock() = 0;

        virtual void WriterLock() = 0;
        virtual void WriterUnlock() = 0;

    protected:
        int ReadCount;
        SemaphoreHandle_t ReadLock;
        SemaphoreHandle_t ResourceLock;
};


class CReadWriteLockPreferReader : public CReadWriteLock {

    public:
        virtual void ReaderLock();
        virtual void ReaderUnlock();

        virtual void WriterLock();
        virtual void WriterUnlock();

};


class CReadWriteLockPreferWriter : public CReadWriteLock {

    public:
        CReadWriteLockPreferWriter();
        virtual ~CReadWriteLockPreferWriter();

        virtual void ReaderLock();
        virtual void ReaderUnlock();

        virtual void WriterLock();
        virtual void WriterUnlock();

    private:
        int WriteCount;
        SemaphoreHandle_t WriteLock;
        SemaphoreHandle_t BlockReadersLock;
};


#endif


