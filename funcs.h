#ifndef FUNCS_H 
#define FUNCS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

// Class definition
class Funcs {
private:
    std::int64_t frame_no;
    unsigned char width;
    unsigned char height;
    unsigned char channels;

    int frame_size;

    std::string file_name_in;
    std::string file_name_out;

    std::ifstream file_data;
    std::ofstream out_data;

    std::vector<char> header_data;

    int channel_offset; // defined in a helper function?
    int channel_size;

    int chunk_size; // define in file
    std::int64_t file_size;
    int remainder_bytes; // define in file
    std::int64_t file_pos; // define in file

    std::vector<char> data;

    std::ofstream out;

public:
    Funcs(const std::string &filename, const std::string &out);
    void extract_header_data();

    void reverse();
    void swap_channel(int channel1, int channel2);
    void clip_channel(int channel, int minimum, int maximum);
    void scale_channel(int channel, float scale_factor);
    void grayscale();

    void reverse_memory();
    void swap_channel_memory(int channel1, int channel2);
    void clip_channel_memory(int channel, int minimum, int maximum);
    void scale_channel_memory(int channel, float scale_factor);
    void grayscale_memory();

    void reverse_speed();
    void swap_channel_speed(int channel1, int channel2);
    void clip_channel_speed(int channel, int minimum, int maximum);
    void scale_channel_speed(int channel, float scale_factor);
    void grayscale_speed();

    // int swap_channel_helper(unsigned char, int frame_size); // add mode in as well?
};

#endif 
