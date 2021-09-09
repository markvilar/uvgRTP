#pragma once

#include "h26x.hh"
#include "util.hh"
#include "frame.hh"
#include "socket.hh"

#include <deque>

namespace uvgrtp {

    class rtp;

    namespace formats {

        enum H264_NAL_TYPES {
            H264_PKT_AGGR = 24,
            H264_PKT_FRAG = 28
        };

        struct h264_aggregation_packet {
            uint8_t fu_indicator[uvgrtp::frame::HEADER_SIZE_H264_FU];
            uvgrtp::buf_vec nalus;  /* discrete NAL units */
            uvgrtp::buf_vec aggr_pkt; /* crafted aggregation packet */
        };

        struct h264_headers {
            uint8_t fu_indicator[uvgrtp::frame::HEADER_SIZE_H264_FU];

            /* there are three types of Fragmentation Unit headers:
             *  - header for the first fragment
             *  - header for all middle fragments
             *  - header for the last fragment */
            uint8_t fu_headers[3 * uvgrtp::frame::HEADER_SIZE_H264_FU];
        };

        class h264 : public h26x {
            public:
                h264(uvgrtp::socket *socket, uvgrtp::rtp *rtp, int flags);
                ~h264();

                /* Packet handler for RTP frames that transport HEVC bitstream
                 *
                 * If "frame" is not a fragmentation unit, packet handler checks
                 * if "frame" is SPS/VPS/PPS packet and if so, returns the packet
                 * to user immediately.
                 *
                 * If "frame" is a fragmentation unit, packet handler checks if
                 * it has received all fragments of a complete HEVC NAL unit and if
                 * so, it merges all fragments into a complete NAL unit and returns
                 * the NAL unit to user. If the NAL unit is not complete, packet
                 * handler holds onto the frame and waits for other fragments to arrive.
                 *
                 * Return RTP_OK if the packet was successfully handled
                 * Return RTP_PKT_READY if "frame" contains an RTP that can be returned to user
                 * Return RTP_PKT_NOT_HANDLED if the packet is not handled by this handler
                 * Return RTP_PKT_MODIFIED if the packet was modified but should be forwarded to other handlers
                 * Return RTP_GENERIC_ERROR if the packet was corrupted in some way */
                static rtp_error_t packet_handler(void *arg, int flags, frame::rtp_frame **frame);

            protected:
                // get h264 nal type
                virtual uint8_t get_nal_type(uint8_t* data);

                // the aggregation packet is not enabled
                virtual rtp_error_t handle_small_packet(uint8_t* data, size_t data_len, bool more);
                
                /* Construct an aggregation packet from data in "aggr_pkt_info_" 
                 * TODO: The code exists, but it is not used */
                virtual rtp_error_t make_aggregation_pkt();

                /* Clear aggregation buffers */
                virtual void clear_aggregation_info();

                // constructs h264 RTP header with correct values
                virtual rtp_error_t construct_format_header_divide_fus(uint8_t* data, size_t& data_left, 
                    size_t& data_pos, size_t payload_size,  uvgrtp::buf_vec& buffers);

            private:
                h264_aggregation_packet aggr_pkt_info_;
        };
    };
};

namespace uvg_rtp = uvgrtp;
