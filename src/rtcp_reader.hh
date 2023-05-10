#pragma once

#include "uvgrtp/clock.hh"
#include "uvgrtp/util.hh"
#include "uvgrtp/frame.hh"

#ifdef _WIN32
#include <ws2ipdef.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <functional>
#include <mutex>


namespace uvgrtp {
    class socketfactory;
    class rtcp;
    class socket;

    class rtcp_reader {

        public: 
            rtcp_reader();
            ~rtcp_reader();
            rtp_error_t start();
            rtp_error_t stop();

            void rtcp_report_reader();
            bool set_socket(std::shared_ptr<uvgrtp::socket> socket, uint16_t port);
            // Map REMOTE ssrc to rtcp
            bool map_ssrc_to_rtcp(std::shared_ptr<std::atomic<uint32_t>> ssrc, std::shared_ptr<uvgrtp::rtcp> rtcp);
            int clear_rtcp_from_reader(std::shared_ptr<std::atomic<std::uint32_t>> remote_ssrc, uint16_t port);

        private:
            bool active_;
            std::shared_ptr<uvgrtp::socket> socket_;
            std::map<std::shared_ptr<std::atomic<uint32_t>>, std::shared_ptr<uvgrtp::rtcp>> rtcps_map_;
            std::unique_ptr<std::thread> report_reader_;
            std::mutex map_mutex_;
    };


}