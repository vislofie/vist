#include "storage.h"

#ifdef FILE_STORAGE
#include "storage/impl/file/file_storage.h"
#endif

static std::shared_ptr<storage> m_storage;

std::shared_ptr<storage> storage::instance() {
    if (!m_storage) {
#ifdef FILE_STORAGE
        m_storage = std::make_shared<file_storage>();
#elif defined(DB_STORAGE)
#endif
    }

    return m_storage;
}
