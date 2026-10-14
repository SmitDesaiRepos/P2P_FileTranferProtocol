#include <p2p/chunker.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <openssl/sha.h>


namespace p2p {

    Chunker::Chunker(size_t piece_size) : chunk_size(piece_size), total_fsize(0) {}

    std::string Chunker::compute_sha256(const char* data, size_t length) {
        unsigned char hash[SHA256_DIGEST_LENGTH];

        //Call Openssl's High-level SHA256 function
        SHA256(reinterpret_cast<const unsigned char*>(data), length, hash);

        //COnvert raw binary hash bytes into a readable string
        std::ostringstream oss;
        for(int i=0; i< SHA256_DIGEST_LENGTH; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        return oss.str();
    }

    bool Chunker::split_file(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        if(!file.is_open()) {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            return false;
        }

        total_fsize = file.tellg();
        file.seekg(0, std::ios::beg);

        src_fname = file_path;
        chunks.clear();

        uint32_t i=0;
        std::vector<char> buffer(chunk_size);
        while(file) {
            file.read(buffer.data(), chunk_size);
            std::streamsize bytes_read = file.gcount();

            if(bytes_read <= 0) break;

            Chunk chunk;
            chunk.i = i++;
            chunk.data.assign(buffer.begin(), buffer.begin() + bytes_read);
            chunk.hash = compute_sha256(chunk.data.data(), chunk.data.size());

            chunks.push_back(chunk);
        }
        file.close();
        return true;
    }

    bool Chunker::assemble_file(const std::string& output_path) const {
        std::ofstream output_file(output_path, std::ios::binary);
        if(!output_file.is_open()) {
            std::cerr << "Failed to open output file: " << output_path << std::endl;
            return false;
        }

        for(const auto& chunk : chunks) {
            output_file.write(chunk.data.data(), chunk.data.size());
        }
        output_file.close();
        return true;
    }

    const std::vector<Chunk>& Chunker::get_chunks() const {return chunks;}
    size_t Chunker::get_chunk_count() const {return chunks.size();}
    size_t Chunker::get_chunk_size() const {return chunk_size;}
    uintmax_t Chunker::get_total_file_size() const {return total_fsize;}
}