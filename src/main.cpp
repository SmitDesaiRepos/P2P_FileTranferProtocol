#include <iostream>
#include "p2p/chunker.hpp"

int main() {
    std::cout<<"=== P2P File Transfer Protocol ==="<<std::endl;

    std::string fname = "testfile.txt"; // Replace with your test file path
    {
        std::ofstream outfile(fname);
        outfile << "Hello P2P Networking! This is a test string that will be sliced and reassembled." << std::endl;
    }
    
    p2p::Chunker chunker(32);
    if (chunker.split_file(fname)) {
        std::cout<<"File split into " << chunker.get_chunk_count() << " chunks." << std::endl;
        for (const auto& chunk : chunker.get_chunks()) {
            std::cout << "Chunk " << chunk.i << ": Size = " << chunk.data.size() << " bytes, Hash = " << chunk.hash.substr(0,12) << std::endl;
        }

        chunker.assemble_file("output.txt");
        std::cout<<"File reassembled into output.txt"<<std::endl;
    } else {
        std::cerr<<"Failed to split file."<<std::endl;
    }

    return 0;
}