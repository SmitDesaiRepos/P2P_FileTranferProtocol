#ifndef P2P_CHUNKER_HPP
#define P2P_CHUNKER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>

namespace p2p {
    struct Chunk {
        uint32_t i;
        std::string hash;
        std::vector<char> data;
    };

    class Chunker {
        private:
            size_t chunk_size;
            std::vector<Chunk> chunks;
            std::string src_fname;
            uintmax_t total_fsize;

            std::string compute_sha256(const char* data, size_t len);

        public:
            explicit Chunker(size_t piece_Size = 256 * 1024); // 256kb per chunk

            // slices a file on disk into indexed, hashed chunks
            bool split_file(const std::string& file_path);

            // reassembles a vector of chunks back into a complete file on disk
            bool assemble_file(const std::string& output_path) const;

            //Getters
            const std::vector<Chunk>& get_chunks() const;
            size_t get_chunk_size() const;
            size_t get_chunk_count() const;
            uintmax_t get_total_file_size() const;
    };
}


#endif // P2P_CHUNKER_HPP