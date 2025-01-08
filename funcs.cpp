#include "funcs.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>

Funcs::Funcs(const std::string &filename, const std::string &out) {
    std::cout << "Opening file: " << filename << std::endl;
    
    file_name_in = filename;
    file_name_out = out;
    
    file_data.open(file_name_in, std::ios::binary);
    out_data.open(file_name_out, std::ios::binary | std::ios::app);

    if (!file_data) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    extract_header_data();

    // channel_offset = swap_channel_helper(channels, frame_size); // maybe do inside the function

    std::cout << "Header data extracte1d" << std::endl;
    frame_size = height * width * channels;
    channel_size = height * width;
    std::cout << "Frame size: " << frame_size << std::endl;
    chunk_size = frame_size; //define chunk size in the function
    std::cout << "Chunk size: " << chunk_size << std::endl;
    
    file_size = (frame_size*frame_no) + 11;
    
    std::cout << "File size: " << file_size << std::endl;
    file_pos = 11;
    
    std::cout << "File position: " << file_pos << std::endl;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    std::cout << "Remainder bytes: " << remainder_bytes << std::endl; 
    // do it in the function
    //std::cout << "Remainder bytes: " << remainder_bytes << std::endl;
    // std::vector<char> data(chunk_size);
    // std::cout << "data size" << data.size() << std::endl;
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

    file_pos = file_size - chunk_size;
    std::vector<char> data(chunk_size);
    while (file_pos > (11)) { // change the 11 >= most likely
        
        file_data.seekg(file_pos, std::ios::beg);
        // std::cout << "Reading chunk at position: " << file_pos << std::endl;
        file_data.read(data.data(), chunk_size);

        //std::reverse(data.begin(), data.end()); //might be faster if i do it manually?
        for(int i = 0; i < chunk_size/2; i+=frame_size){
            std::swap_ranges(data.begin() + i, 
            data.begin() + i + frame_size, 
            data.begin() + chunk_size - i - frame_size);
        }
        std::cout << "data size" << data.size() << std::endl;
        out_data.write(data.data(), data.size());
        std::cout << "Chunk reversed and written" << std::endl;
        file_pos -= chunk_size;
        std::cout << "File position: " << file_pos << std::endl; 
    }

    file_pos = 11;
    file_data.seekg(file_pos, std::ios::beg);
    data.resize(remainder_bytes);

    file_data.read(data.data(), data.size());

    //std::reverse(data.begin(), data.end());
    for(int i = 0; i < remainder_bytes/2; i+=frame_size){
        std::swap_ranges(data.begin() + i, 
        data.begin() + i + frame_size, 
        data.begin() + remainder_bytes - i - frame_size);
    }

    out_data.write(data.data(), remainder_bytes);

    std::cout << "Remainder reversed and written" << std::endl;
    out_data.close();
}

void Funcs::swap_channel(int channel1, int channel2){
    //read in chunks?
    //jump from frame to frame
    // swap the channel using a temp vector?
    // CHANNEL 1 IS LARGER THAN CHANNEL 2
    std::vector<char> data(chunk_size);
    int channel_offset = (channel1 - channel2)*channel_size ;
    int channel2_start = (channel2*channel_size); 
    std::cout << "Starting swap" << std::endl;
    while (file_pos < (file_size - chunk_size)){

        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = 0; i < chunk_size; i+=frame_size){
            std::swap_ranges(data.begin() + i + channel2_start, 
                            data.begin() + i + (channel2_start + channel_size),
                            data.begin() + i + channel_offset + channel2_start);
        }
        out_data.write(data.data(), chunk_size);                    
        
        file_pos += chunk_size;
        std::cout << "File position: " << file_pos << std::endl; 
        std::cout << "Chunk swapped and written" << std::endl;
        

    } 
    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    
    for (int i = 0; i < remainder_bytes; i+=frame_size){
        std::swap_ranges(data.begin() + i + channel2_start, 
                        data.begin() + i + (channel2_start + channel_size),
                        data.begin() + i + channel_offset + channel2_start);   
    }
    out_data.write(data.data(), remainder_bytes);
    return;
}

void Funcs::clip_channel(int channel, int minimum, int maximum){
    
    int channel_start = channel*channel_size; 
    std::vector<char> data(chunk_size);

    while (file_pos < (file_size - chunk_size)){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = channel_start; i < chunk_size; i+=frame_size){
            
            for (int j = i; j < i + channel_size; j++){
                
                if (data[j] < minimum) {
                    data[j] = minimum;
                } else if (data[j] > maximum) {
                    data[j] = maximum;
                }
                    
                }
            }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;                          
        }

        
        data.resize(remainder_bytes);
        file_data.read(data.data(), data.size());
        for (int i = channel_start; i < remainder_bytes; i+=frame_size){
            for (int j = i; j < i + channel_size; j++){
                if (data[j] < minimum) {
                    data[j] = minimum;
                } else if (data[j] > maximum) {
                    data[j] = maximum;
                }
            }
        }
        out_data.write(data.data(), remainder_bytes);
        return;

    }


void Funcs::scale_channel(int channel, float scale_factor){
    // Implement the function logic here
    int channel_start = channel*channel_size;
    std::vector<char> data(chunk_size);
    
    while (file_pos < (file_size - chunk_size)){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = channel_start; i < chunk_size; i+=frame_size){
            for (int j = i; j < i + channel_size; j++){
                data[j] = ceil(data[j] * scale_factor);
                if (data[j] > 255) {
                    data[j] = 255;
                } 
                data[j] = static_cast<unsigned char>(data[j]);
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    for (int i = channel_start; i < remainder_bytes; i+=frame_size){
        for (int j = i; j < i + channel_size; j++){
            data[j] = ceil(data[j] * scale_factor);
            if (data[j] > 255) {
                data[j] = 255;
            } 
            data[j] = static_cast<unsigned char>(data[j]);
        }
    }
    out_data.write(data.data(), remainder_bytes);
    return;

    
}


// int Funcs::swap_channel_helper(unsigned char channels, int frame_size){
//     //switch statement to determine the offset
//     //return the offs

//         return 0;
//     }
