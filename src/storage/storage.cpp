#include "storage.h"

#ifdef FILE_STORAGE
#include "storage/impl/file/file_storage.h"
#endif

storage* storage::instance() {
#ifdef FILE_STORAGE
    static file_storage storage_impl;
#elif defined(DB_STORAGE)
#endif

    return &storage_impl;
}