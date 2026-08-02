#pragma once
#include <functional>
#include <string_view>

class storage {
public:
    static std::shared_ptr<storage> instance();

    virtual ~storage() = default;

    virtual void async_write(
        std::string_view key, std::string_view value,
        std::function<void(int)> write_status_message) = 0;
    virtual int sync_write(std::string_view key, std::string_view value) = 0;
    virtual void async_read(
        std::string_view key,
        std::function<void(const std::string& value)> read_status_message) = 0;
    virtual std::string sync_read(std::string_view key) = 0;

protected:
    virtual bool is_key_exists(std::string_view key) = 0;
};