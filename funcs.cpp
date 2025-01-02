#include "funcs.h"
#include <iostream>
#include <algorithm>

Funcs::Funcs(const std::string &filename) {
    std::cout << "Opening file: " << filename << std::endl;
    file_name_in = filename;
    file_name_out = "out.bin";
    file_data.open(file_name_in, std::ios::binary);
    out_data.open(file_name_out, std::ios::binary | std::ios::app);

    if (!file_data) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    extract_header_data();
    std::cout << "Header data extracte1d" << std::endl;
    frame_size = height * width * channels;
    std::cout << "Frame size: " << frame_size << std::endl;
    chunk_size = frame_size;
    std::cout << "Chunk size: " << chunk_size << std::endl;
    file_size = frame_size * frame_no + 11;
    std::cout << "File size: " << file_size << std::endl;
    file_pos = file_size - chunk_size;
    std::cout << "File position: " << file_pos << std::endl;
    remainder_bytes = (file_size - 11) % chunk_size;
    //std::cout << "Remainder bytes: " << remainder_bytes << std::endl;

    std::cout << "Initialization complete" << std::endl;
}

void Funcs::extract_header_data() {
    std::cout << "Extracting header data" << std::endl;
    file_data.read(reinterpret_cast<char*>(&frame_no), sizeof(long));
    std::cout << "Frame number: " << frame_no << std::endl;
    file_data.seekg(8, std::ios::beg);
    file_data.read(reinterpret_cast<char*>(&channels), sizeof(unsigned char));
    std::cout << "Channels: " << static_cast<int>(channels) << std::endl;
    file_data.read(reinterpret_cast<char*>(&height), sizeof(unsigned char));
    std::cout << "Height: " << static_cast<int>(height) << std::endl;
    file_data.read(reinterpret_cast<char*>(&width), sizeof(unsigned char));
    std::cout << "Width: " << static_cast<int>(width) << std::endl;

    file_data.seekg(0, std::ios::beg);
    std::vector<char> header_binary(11);
    file_data.read(header_binary.data(), header_binary.size());

    out_data.write(header_binary.data(), header_binary.size());
    std::cout << "Header data extracted" << std::endl;
    return;
}

void Funcs::reverse() {
    std::cout << "Reversing data" << std::endl;
    while (file_pos >= (11)) {
        std::cout << "Reading chunk at position: " << file_pos << std::endl;
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);

        std::reverse(data.begin(), data.end());

        out_data.write(data.data(), data.size());
        std::cout << "Chunk reversed and written" << std::endl;
        file_pos -= chunk_size;
    }

    // file_pos = 11;
    // data.resize(remainder_bytes);

    // file_data.read(data.data(), data.size());

    // std::reverse(data.begin(), data.end());

    // out_data.write(data.data(), remainder_bytes);
    std::cout << "Remainder reversed and written" << std::endl;
    out_data.close();
}
