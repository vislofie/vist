#include "file_storage.h"

#include <cassert>
#include <thread>
#include <sys/stat.h>

file_storage::file_storage() {
    auto t = std::thread([this] { run(); });
    t.detach();
}

void file_storage::async_write(
    std::string_view key, std::string_view value,
    const std::function<void(int)> write_status_message) {
    {
        std::lock_guard lock(m_queue_mutex);
        m_job_queue.emplace(std::make_shared<write_job>(
            write, std::string(key.data()), std::string(value.data()),
            write_status_message)
        );
    }
}

void file_storage::async_read(
    std::string_view key,
    const std::function<void(const std::string& value)> read_status_message) {
    {
        std::lock_guard lock(m_queue_mutex);
        m_job_queue.emplace(std::make_shared<read_job>(
            read, std::string(key.data()),
            read_status_message)
        );
    }
}

bool file_storage::is_key_exists(std::string_view key) {
    {
        std::lock_guard lock(m_queue_mutex);

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
    while (true)
    {
        std::shared_ptr<job> j;
        {
            std::lock_guard lock(m_queue_mutex);
            if (m_job_queue.empty())
                continue;

            j = m_job_queue.front();
            m_job_queue.pop();
        }

        if (j->type == read) {
            auto rj = dynamic_cast<read_job*>(j.get());

            std::string desired;
            {
                std::lock_guard lock(m_queue_mutex);

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
                const char* text = (wj->key + '|' + wj->value + '\n').c_str();
                std::lock_guard lock(m_file_mutex);

                m_write_file_handle.open("state", std::ios::binary | std::ios::app);
                m_write_file_handle.write(text, strlen(text));
                m_write_file_handle.close();
            }

            wj->callback(0);
        }
        else {
            assert(false);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
