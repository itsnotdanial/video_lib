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

    frame_size = height * width * channels;
    channel_size = height * width;
     //define chunk size in the function
    
    file_size = (frame_size*frame_no) + 11;
    file_pos = 11;


    // do it in the function
    //std::cout << "Remainder bytes: " << remainder_bytes << std::endl;
    // std::vector<char> data(chunk_size);
    // std::cout << "data size" << data.size() << std::endl;
    
}

void Funcs::extract_header_data() { 
    file_data.read(reinterpret_cast<char*>(&frame_no), sizeof(long));
    file_data.seekg(8, std::ios::beg);
    file_data.read(reinterpret_cast<char*>(&channels), sizeof(unsigned char));
    file_data.read(reinterpret_cast<char*>(&height), sizeof(unsigned char));
    file_data.read(reinterpret_cast<char*>(&width), sizeof(unsigned char));
    file_data.seekg(0, std::ios::beg);
    std::vector<char> header_binary(11);
    file_data.read(header_binary.data(), header_binary.size());

    out_data.write(header_binary.data(), header_binary.size());
    return;
}

void Funcs::reverse() {
    chunk_size = frame_size*8;
    remainder_bytes = (file_size - 11) % chunk_size;
    
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
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
        out_data.write(data.data(), data.size());
        file_pos -= chunk_size; 
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
    out_data.close();
}

void Funcs::swap_channel(int channel1, int channel2){
    if (channel1 < channels-1 || channel2 == channel1){
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    
    chunk_size = frame_size;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
    std::vector<char> data(chunk_size);
    int channel_offset = (channel1 - channel2)*channel_size ;
    int channel2_start = (channel2*channel_size); 
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
    chunk_size = frame_size*8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
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
    
    chunk_size = frame_size*8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }

    int channel_start = channel*channel_size;
    int temp;
    std::vector<char> data(chunk_size);
    
    while (file_pos < (file_size - chunk_size)){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = channel_start; i < chunk_size; i+=frame_size){
            for (int j = i; j < i + channel_size; j++){
                temp = ceil(data[j] * scale_factor);
                if (temp > 255) {
                    temp = 255;
                } 
                data[j] = static_cast<unsigned char>(temp);
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    for (int i = channel_start; i < remainder_bytes; i+=frame_size){
        for (int j = i; j < i + channel_size; j++){
            temp = ceil(data[j] * scale_factor);
            if (temp > 255) {
                temp = 255;
            } 
            data[j] = static_cast<unsigned char>(temp);
        }
    }
    out_data.write(data.data(), remainder_bytes);
    return;

    
}

void Funcs::grayscale(){
    // Implement the function logic here
    if (channels != 3){
        std::cerr << "Invalid channel count" << std::endl;
        return;
    }
    chunk_size = frame_size*8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
    std::vector<char> data(chunk_size);
    
    while (file_pos < (file_size - chunk_size)){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = 0; i < chunk_size; i+=frame_size){
            for (int j = i; j < i + channel_size; j++){
                int grayscale = ceil(data[j] * 0.299) + ceil(data[j + channel_size]*0.587) + ceil(data[j + channel_size*2]*0.114);
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2*channel_size)] = grayscale;
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    for (int i = 0; i < remainder_bytes; i+=frame_size){
        for (int j = i; j < i + channel_size; j++){
                int grayscale = ceil(data[j] * 0.299) + ceil(data[j + channel_size]*0.587) + ceil(data[j + channel_size*2]*0.114);
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2*channel_size)] = grayscale;
        }
    }
    out_data.write(data.data(), remainder_bytes);
    return;
}

void Funcs::reverse_memory() {
    std::cout << "Reversing data" << std::endl;
    chunk_size = frame_size;
    
    file_pos = file_size-chunk_size;
    std::vector<char> data(chunk_size);
    
    while (file_pos >= (11)) { // change the 11 >= most likely
        
        file_data.seekg(file_pos, std::ios::beg);
        // std::cout << "Reading chunk at position: " << file_pos << std::endl;
        file_data.read(data.data(), chunk_size);

        //std::reverse(data.begin(), data.end()); //might be faster if i do it manually?
        out_data.write(data.data(), data.size());
        file_pos -= chunk_size; 
    }
    return;
}

void Funcs::swap_channel_memory(int channel1, int channel2){
    
    chunk_size = frame_size;
    
    std::vector<char> data(chunk_size);
    int channel_offset = (channel1 - channel2)*channel_size ;
    int channel2_start = (channel2*channel_size); 
    while (file_pos < (file_size)){

        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = 0; i < chunk_size; i+=frame_size){
            std::swap_ranges(data.begin() + i + channel2_start, 
                            data.begin() + i + (channel2_start + channel_size),
                            data.begin() + i + channel_offset + channel2_start);
        }
        out_data.write(data.data(), chunk_size);                    
        
        file_pos += chunk_size;
        

    } 
}

void Funcs::clip_channel_memory(int channel, int minimum, int maximum){
    chunk_size = frame_size;

    int channel_start = channel*channel_size; 
    std::vector<char> data(chunk_size);

    while (file_pos < (file_size)){
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
        return;

    }


void Funcs::scale_channel_memory(int channel, float scale_factor){
    
    chunk_size = frame_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }

    int channel_start = channel*channel_size;
    int temp;
    std::vector<char> data(chunk_size);
    
    while (file_pos < file_size){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = channel_start; i < chunk_size; i+=frame_size){
            for (int j = i; j < i + channel_size; j++){
                temp = ceil(data[j] * scale_factor);
                if (temp > 255) {
                    temp = 255;
                } 
                data[j] = static_cast<unsigned char>(temp);
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    return;

    
}

void Funcs::grayscale_memory(){
    // Implement the function logic here

    chunk_size = frame_size;
    
    std::vector<char> data(chunk_size);
    
    while (file_pos < file_size){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        for (int i = 0; i < chunk_size; i+=frame_size){
            for (int j = i; j < i + channel_size; j++){
                int grayscale = ceil(data[j] * 0.299) + ceil(data[j + channel_size]*0.587) + ceil(data[j + channel_size*2]*0.114);
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2*channel_size)] = grayscale;
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    return;
}

void Funcs::reverse_speed() {
    chunk_size = frame_size*25000;
    remainder_bytes = (file_size - 11) % chunk_size;
    
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
    file_pos = file_size - chunk_size;
    std::vector<char> data(chunk_size);
    
    while (file_pos > (11)) { // change the 11 >= most likely
        
        file_data.seekg(file_pos, std::ios::beg);
        // std::cout << "Reading chunk at position: " << file_pos << std::endl;
        file_data.read(data.data(), chunk_size);

        //std::reverse(data.begin(), data.end()); //might be faster if i do it manually?
        #pragma omp parallel for
        for(int i = 0; i < chunk_size/2; i+=frame_size){
            std::swap_ranges(data.begin() + i, 
            data.begin() + i + frame_size, 
            data.begin() + chunk_size - i - frame_size);
        }
        out_data.write(data.data(), data.size());
        file_pos -= chunk_size; 
    }

    file_pos = 11;
    file_data.seekg(file_pos, std::ios::beg);
    data.resize(remainder_bytes);

    file_data.read(data.data(), data.size());

    //std::reverse(data.begin(), data.end());
    #pragma omp parallel for
    for(int i = 0; i < remainder_bytes/2; i+=frame_size){
        std::swap_ranges(data.begin() + i, 
        data.begin() + i + frame_size, 
        data.begin() + remainder_bytes - i - frame_size);
    }

    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::swap_channel_speed(int channel1, int channel2){
    if (channel1 < channels-1 || channel2 == channel1){
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    
    chunk_size = frame_size*25000;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
    std::vector<char> data(chunk_size);
    int channel_offset = (channel1 - channel2)*channel_size ;
    int channel2_start = (channel2*channel_size); 
    while (file_pos < (file_size - chunk_size)){

        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        #pragma omp parallel for
        for (int i = 0; i < chunk_size; i+=frame_size){
            std::swap_ranges(data.begin() + i + channel2_start, 
                            data.begin() + i + (channel2_start + channel_size),
                            data.begin() + i + channel_offset + channel2_start);
        }
        out_data.write(data.data(), chunk_size);                    
        
        file_pos += chunk_size;
        

    } 
    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    #pragma omp parallel for
    for (int i = 0; i < remainder_bytes; i+=frame_size){
        std::swap_ranges(data.begin() + i + channel2_start, 
                        data.begin() + i + (channel2_start + channel_size),
                        data.begin() + i + channel_offset + channel2_start);   
    }
    out_data.write(data.data(), remainder_bytes);
    return;
}

void Funcs::clip_channel_speed(int channel, int minimum, int maximum){
    chunk_size = frame_size*25000;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
    int channel_start = channel*channel_size; 
    std::vector<char> data(chunk_size);

    while (file_pos < (file_size - chunk_size)){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        
        
        for (int i = channel_start; i < chunk_size; i+=frame_size){
            #pragma omp parallel for
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

            # pragma omp parallel for
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


void Funcs::scale_channel_speed(int channel, float scale_factor){
    
    chunk_size = frame_size*25000;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }

    int channel_start = channel*channel_size;
    int temp;
    std::vector<char> data(chunk_size);
    
    while (file_pos < (file_size - chunk_size)){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
 
        for (int i = channel_start; i < chunk_size; i+=frame_size){

            # pragma omp parallel for
            for (int j = i; j < i + channel_size; j++){
                temp = ceil(data[j] * scale_factor);
                if (temp > 255) {
                    temp = 255;
                } 
                data[j] = static_cast<unsigned char>(temp);
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());

    for (int i = channel_start; i < remainder_bytes; i+=frame_size){

        # pragma omp parallel for
        for (int j = i; j < i + channel_size; j++){
            temp = ceil(data[j] * scale_factor);
            if (temp > 255) {
                temp = 255;
            } 
            data[j] = static_cast<unsigned char>(temp);
        }
    }
    out_data.write(data.data(), remainder_bytes);
    return;

    
}

void Funcs::grayscale_speed(){
    // Implement the function logic here
    if (channels != 3){
        std::cerr << "Invalid channel count" << std::endl;
        return;
    }
    chunk_size = frame_size*25000;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0){
        remainder_bytes = chunk_size;
    }
    
    std::vector<char> data(chunk_size);
    
    while (file_pos < (file_size - chunk_size)){
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
  
        for (int i = 0; i < chunk_size; i+=frame_size){

            # pragma omp parallel for
            for (int j = i; j < i + channel_size; j++){
                int grayscale = ceil(data[j] * 0.299) + ceil(data[j + channel_size]*0.587) + ceil(data[j + channel_size*2]*0.114);
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2*channel_size)] = grayscale;
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());

    for (int i = 0; i < remainder_bytes; i+=frame_size){
        # pragma omp parallel for
        for (int j = i; j < i + channel_size; j++){
                int grayscale = ceil(data[j] * 0.299) + ceil(data[j + channel_size]*0.587) + ceil(data[j + channel_size*2]*0.114);
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2*channel_size)] = grayscale;
        }
    }
    out_data.write(data.data(), remainder_bytes);
    return;
}
