#include <iostream>
#include <asio.hpp>

class printer {
public:
    printer(asio::io_context& io) :
        m_strand(asio::make_strand(io)),
        m_timer1(io, asio::chrono::seconds(1)),
        m_timer2(io, asio::chrono::seconds(1)),
        m_count(0) {

        m_timer1.async_wait(asio::bind_executor(m_strand, [this](auto errorCode) {
            print1();
        }));
        m_timer2.async_wait(asio::bind_executor(m_strand, [this](auto errorCode) {
            print2();
        }));
    }

    void print1()
    {
        if (m_count < 10)
        {
            std::cout << "Timer 1: " << m_count << std::endl;
            ++m_count;

            m_timer1.expires_at(m_timer1.expiry() + asio::chrono::seconds(1));
            m_timer1.async_wait(asio::bind_executor(m_strand, [this](auto errorCode) {
                print1();
            }));
        }
    }

    void print2()
    {
        if (m_count < 10)
        {
            std::cout << "Timer 2: " << m_count << std::endl;
            ++m_count;

            m_timer2.expires_at(m_timer2.expiry() + asio::chrono::milliseconds(1200));
            m_timer2.async_wait(asio::bind_executor(m_strand, [this](auto errorCode) {
                print2();
            }));
        }
    }
private:
    asio::steady_timer m_timer1;
    asio::steady_timer m_timer2;
    asio::strand<asio::io_context::executor_type> m_strand;
    int m_count;
};

int main() {
    asio::io_context io;
    printer p(io);
    std::thread t([&]{ io.run(); });
    io.run();
    t.join();
    return 0;
}
