#include "file_storage.h"

#include <cassert>
#include <future>
#include <thread>

file_storage::file_storage() {
    auto t = std::thread([this] { run(); });
    t.detach();
}

void file_storage::async_write(
    std::string_view key, std::string_view value,
    const std::function<void(int)> write_status_message) {
    std::lock_guard lock(m_queue_mutex);
    m_job_queue.emplace(std::make_shared<write_job>(
        write, std::string(key), std::string(value),
        write_status_message)
    );

    m_cv.notify_one();
}

int file_storage::sync_write(std::string_view key, std::string_view value) {
    std::promise<int> promise;
    auto future = promise.get_future();

    {
        std::lock_guard lock(m_queue_mutex);
        m_job_queue.emplace(std::make_shared<write_job>(write, std::string(key), std::string(value),
        [p = &promise](const int status_message) {
            p->set_value(status_message);
        }));

        m_cv.notify_one();
    }


    return future.get();
}

void file_storage::async_read(
    std::string_view key,
    const std::function<void(const std::string& value)> read_status_message) {
    {
        std::lock_guard lock(m_queue_mutex);
        m_job_queue.emplace(std::make_shared<read_job>(
            read, std::string(key),
            read_status_message)
        );

        m_cv.notify_one();
    }
}

std::string file_storage::sync_read(std::string_view key) {
    std::promise<std::string> promise;
    auto future = promise.get_future();

    {
        std::lock_guard lock(m_queue_mutex);
        m_job_queue.emplace(std::make_shared<read_job>(read, std::string(key),
        [p = &promise](const std::string& val) {
            p->set_value(val);
        }));

        m_cv.notify_one();
    }

    return future.get();
}

bool file_storage::is_key_exists(std::string_view key) {
    {
        std::lock_guard lock(m_file_mutex);

        m_read_file_handle.open("state", std::ios::binary | std::ios::in);
        std::stringstream buff;
        buff << m_read_file_handle.rdbuf();
        std::string desired = buff.str();
        m_read_file_handle.close();

        auto search_target = std::string(key) + '|';
        return desired.find(search_target) != std::string::npos;
    }
}

void file_storage::run() {
    for (;;)
    {
        std::shared_ptr<job> j;
        {
            std::unique_lock lock(m_queue_mutex);
            m_cv.wait(lock, [this] { return !m_job_queue.empty(); });

            j = m_job_queue.front();
            m_job_queue.pop();
        }

        if (j->type == read) {
            auto rj = dynamic_cast<read_job*>(j.get());

            std::string desired;
            {
                std::lock_guard lock(m_file_mutex);

                m_read_file_handle.open("state", std::ios::binary | std::ios::in);
                std::stringstream buff;
                buff << m_read_file_handle.rdbuf();
                desired = buff.str();
                m_read_file_handle.close();
            }



            auto str_to_find = rj->key + '|';
            auto it = desired.find(str_to_find);
            if (it != std::string::npos) {
                auto it_end = desired.find('\n', it);
                if (it_end != std::string::npos) {
                    auto result = std::string(desired.begin() + it + str_to_find.size(), desired.begin() + it_end);
                    rj->callback(result);

                    continue;
                }
            }

            rj->callback("");
        }
        else if (j->type == write) {
            auto wj = dynamic_cast<write_job*>(j.get());
            if (is_key_exists(wj->key)) {
                wj->callback(-1);
                continue;
            }

            {
                std::string text = wj->key + '|' + wj->value + '\n';
                std::lock_guard lock(m_file_mutex);

                m_write_file_handle.open("state", std::ios::binary | std::ios::app);
                m_write_file_handle.write(text.c_str(), text.size());
                m_write_file_handle.close();
            }

            wj->callback(0);
        }
        else {
            assert(false);
        }
    }
}