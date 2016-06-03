/**
 *
 *  https://en.wikipedia.org/wiki/Readers%E2%80%93writers_problem
 */

#ifndef READ_WRITE_LOCK_HPP_
#define READ_WRITE_LOCK_HPP_


namespace cpp_freertos {


/**
 *  This is the exception that is thrown if a ReadWriteLock constructor fails.
 */
class ReadWriteLockCreateException : public std::exception {

    public:
        /**
         *  Create the exception.
         */
        ReadWriteLockCreateException()
        {
            sprintf(errorString, "ReadWriteLock Constructor Failed");
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


class ReadWriteLock {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        ReadWriteLock();
        virtual ~ReadWriteLock();

        virtual void ReaderLock() = 0;
        virtual void ReaderUnlock() = 0;

        virtual void WriterLock() = 0;
        virtual void WriterUnlock() = 0;

    /////////////////////////////////////////////////////////////////////////
    //
    //  Protected API
    //  Not intended for use by application code.
    //
    /////////////////////////////////////////////////////////////////////////
    protected:
        int ReadCount;
        SemaphoreHandle_t ReadLock;
        SemaphoreHandle_t ResourceLock;
};


class ReadWriteLockPreferReader : public ReadWriteLock {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        virtual void ReaderLock();
        virtual void ReaderUnlock();

        virtual void WriterLock();
        virtual void WriterUnlock();

};


class ReadWriteLockPreferWriter : public ReadWriteLock {

    /////////////////////////////////////////////////////////////////////////
    //
    //  Public API
    //
    /////////////////////////////////////////////////////////////////////////
    public:
        ReadWriteLockPreferWriter();
        virtual ~ReadWriteLockPreferWriter();

        virtual void ReaderLock();
        virtual void ReaderUnlock();

        virtual void WriterLock();
        virtual void WriterUnlock();

    /////////////////////////////////////////////////////////////////////////
    //
    //  Private API
    //  The internals of this wrapper class.
    //
    /////////////////////////////////////////////////////////////////////////
    private:
        int WriteCount;
        SemaphoreHandle_t WriteLock;
        SemaphoreHandle_t BlockReadersLock;
};


}
#endif
