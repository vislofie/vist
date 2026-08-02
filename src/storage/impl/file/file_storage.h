#pragma once
#include <fstream>
#include <queue>

#include "storage/storage.h"

class job;
class write_job;
class read_job;

class file_storage : public storage {
public:
    file_storage();

    enum operation_type {
        read,
        write,
    };

    virtual void async_write(
        std::string_view key, std::string_view value,
        std::function<void(int)> write_status_message) override;
    virtual int sync_write(std::string_view key, std::string_view value) override;
    virtual void async_read(
        std::string_view key,
        std::function<void(const std::string& value)> read_status_message) override;
    virtual std::string sync_read(std::string_view key) override;

protected:
    virtual bool is_key_exists(std::string_view key) override;

private:
    void run();


    class job {
    public:
        job(const operation_type type,
            const std::string& key) {
            this->type = type;
            this->key = key;
        }
        virtual ~job() = default;

        operation_type type{};
        std::string key;
    };

    class write_job : public job {
    public:
        write_job(const operation_type type,
                  const std::string& key,
                  const std::string& value,
                  const std::function<void(int)>& callback) : job(type, key) {
            this->type = type;
            this->key = key;
            this->value = value;
            this->callback = callback;
        }

        std::string value;
        std::function<void(int)> callback;
    };

    class read_job : public job {
    public:
        read_job(const operation_type type,
                 const std::string& key,
                 const std::function<void(std::string value)>& callback) : job(type, key) {
            this->type = type;
            this->key = key;
            this->callback = callback;
        }

        std::function<void(const std::string& value)> callback;
    };

    std::queue<std::shared_ptr<job>> m_job_queue{};
    std::mutex m_queue_mutex;

    std::fstream m_read_file_handle{};
    std::fstream m_write_file_handle{};
    std::mutex m_file_mutex;

    std::condition_variable m_cv;
};