#include "funcs.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>

Funcs::Funcs(const std::string &filename, const std::string &out) {
    // Open the input file in read mode and output file in append mode
    file_name_in = filename;
    file_name_out = out;
    file_data.open(file_name_in, std::ios::binary);
    out_data.open(file_name_out, std::ios::binary | std::ios::app);
    // Check if the file is open
    if (!file_data) {
        std::cerr << "Error opening file!" << std::endl;
        std::exit(1);
    }
    // Extract the header data
    extract_header_data();
    // Calculate the frame size, channel size, file size, remainder bytes and file position
    std::cout << height << " " << width << " " << channels << std::endl;
    frame_size = height * width * channels;
    std::cout << frame_size << std::endl;
    channel_size = height * width;
    std::cout << frame_no << std::endl; 
    file_size = (frame_size * frame_no) + 11;
    file_data.seekg(0, std::ios::end);
    file_pos = file_data.tellg();
    std::cout << "file pos: " << file_pos << std::endl;
    std::cout << "file size: " << file_size << std::endl;
    // Check if the file data matches the header data
    if (file_size != file_pos) {
        std::cerr << "File wasn't opened properly or its corrupt" << std::endl;
        std::exit(1);
    }
    file_pos = 11;
    file_data.seekg(file_pos, std::ios::beg);
}

void Funcs::extract_header_data() {
    file_data.read(reinterpret_cast<char*>(&frame_no), sizeof(std::int64_t));
    file_data.seekg(8, std::ios::beg);
    file_data.read(reinterpret_cast<char*>(&channels), sizeof(unsigned char));
    file_data.read(reinterpret_cast<char*>(&height), sizeof(unsigned char));
    file_data.read(reinterpret_cast<char*>(&width), sizeof(unsigned char));
    file_data.seekg(0, std::ios::beg);
    std::vector<char> header_binary(11);
    file_data.read(header_binary.data(), header_binary.size());

    out_data.write(header_binary.data(), header_binary.size());
}

void Funcs::reverse() {
    // Set the chunk size and remainder bytes
    chunk_size = channel_size; 
    // Set the file position to read last chunk 
    // So the chunks are reversed
    file_pos = file_size - frame_size;
    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos >= 11) {
        // Read the chunk of data

        file_data.seekg(file_pos, std::ios::beg);
        for (int i = 0; i < channels; i++){
            file_data.read(data.data(), chunk_size);
            out_data.write(data.data(), data.size());}
        file_pos -= frame_size;
    }
    out_data.close();
}

void Funcs::swap_channel(int channel1, int channel2) {
    // Check if the channels passed are valid
    if (channel1 > channels - 1 || channel2 == channel1 || channel2 < 0) {
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    // If there are no remainder bytes, set it to the chunk size
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }
    std::vector<char> data(chunk_size);
    // Calculate the offset between the two channels
    int channel_offset = (channel1 - channel2) * channel_size;
    // Calcualte where the first channel begins from
    int channel2_start = channel2 * channel_size;
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {
        // Read the chunk of data
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        // Loop through the chunk of data
        // Swap the two channels for each frame
        for (int i = 0; i < chunk_size; i += frame_size){
            std::swap_ranges(data.begin() + i + channel2_start, 
                             data.begin() + i + (channel2_start + channel_size),
                             data.begin() + i + channel_offset + channel2_start);
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    } 
    // Repeat everything for the remainder bytes
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());

    for (int i = 0; i < remainder_bytes; i += frame_size) {
        std::swap_ranges(data.begin() + i + channel2_start,
                         data.begin() + i + (channel2_start + channel_size),
                         data.begin() + i + channel_offset + channel2_start);
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::clip_channel(int channel, int minimum, int maximum) {
    // Check if thr arguments are valid
    if(channel > channels - 1 || maximum > 255 || minimum < 0 || minimum > maximum || channel < 0 || maximum < minimum) {
        std::cerr << "Invalid arguments" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }
    // Calculate the start of the channel
    int channel_start = channel * channel_size;
    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {
        file_data.read(data.data(), chunk_size);
        // Increment in frames from the start of the channel to be clipped
        for (int i = channel_start; i < chunk_size; i += frame_size) {
            // Loop through the channel
            for (int j = i; j < i + channel_size; j++) {
                // Clip the channel
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
    // Repeat everything for the remainder bytes
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    for (int i = channel_start; i < remainder_bytes; i += frame_size) {
        for (int j = i; j < i + channel_size; j++) {
            if (data[j] < minimum) {
                data[j] = minimum;
            } else if (data[j] > maximum) {
                data[j] = maximum;
            }
        }
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::scale_channel(int channel, float scale_factor) {
    // Check if the arguments are valid
    if(channel > channels - 1 || scale_factor < 0 || channel < 0) {
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }
    // Calculate the start of the channel
    int channel_start = channel * channel_size;
    // Temporary variable to store the scaled value
    float temp;
    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {
        // Read the chunk of data
        file_data.read(data.data(), chunk_size);
        // Increment in frames from the start of the channel to be scaled
        for (int i = channel_start; i < chunk_size; i += frame_size) {
            // Loop through the channel
            for (int j = i; j < i + channel_size; j++) {
                // Scale the channel
                temp = (static_cast<unsigned char>(data[j]) * scale_factor);
                if (temp > 255) {
                    // Clamp the pixel value if it exceeds 255
                    temp = 255;
                }
                // Store the scaled value
                data[j] = static_cast<unsigned char>(temp);
            }
        }
        // Write the scaled chunk of data to the output file
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    // Repeat everything for the remainder bytes
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    for (int i = channel_start; i < remainder_bytes; i += frame_size) {
        for (int j = i; j < i + channel_size; j++) {
            temp = (static_cast<unsigned char>(data[j]) * scale_factor);
            if (temp > 255) {
                temp = 255;
            }
            data[j] = static_cast<unsigned char>(temp);
        }
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::grayscale() {
    // Check if the channel count is valid
    if (channels != 3) {
        std::cerr << "Invalid channel count" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }

    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {
        // Read the chunk of data
        file_data.read(data.data(), chunk_size);
        // Increment in frames from the start of the chunk
        for (int i = 0; i < chunk_size; i += frame_size) {
            // Loop through the chunk of data
            for (int j = i; j < i + channel_size; j++) {
                // Calculate the grayscale value
                unsigned char grayscale =   (data[j] * 0.299 + 
                                            (data[j + channel_size] * 0.587) +
                                            data[j + channel_size * 2] * 0.114);
                // Set the grayscale value for each RGB pixel in the current index                              
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2 * channel_size)] = grayscale;
            }   
        }
        // Write the grayscale chunk of data to the output file
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    // Repeat everything for the remainder bytes
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    for (int i = 0; i < remainder_bytes; i += frame_size) {
        for (int j = i; j < i + channel_size; j++) {
            unsigned char grayscale = ceil(data[j] * 0.299 +
                                           (data[j + channel_size] * 0.587) +
                                           (data[j + channel_size * 2] * 0.114));
            data[j] = grayscale;
            data[j + channel_size] = grayscale;
            data[j + (2 * channel_size)] = grayscale;
        }
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::reverse_memory() {
    // Initialize file position to the last chunk
    std::cout << "reverse memory" << std::endl;
    file_pos = file_size - frame_size;
    std::vector<char> data(frame_size);
    // Loop until the start of the file
    while (file_pos >= 11) {
        file_data.seekg(file_pos, std::ios::beg);
        // Reverse the frame positions
        file_data.read(data.data(), frame_size);
        out_data.write(data.data(), data.size());
        file_pos -= frame_size;
    }
}

void Funcs::swap_channel_memory(int channel1, int channel2) {
    // Check if the channels passed are valid
    if(channel1 > channels - 1 || channel2 == channel1 || channel2 < 0)  {
        std::cerr << "Invalid arguments" << std::endl;
        return;
    }

    std::vector<char> data(frame_size);
    // Calculate the offset between the two channels
    int channel_offset = (channel1 - channel2) * channel_size;
    int channel2_start = channel2 * channel_size;
    // Loop until the end of the file
    while (file_pos < file_size) {
        // Read the data in frames
        file_data.read(data.data(), frame_size);
        // Swap the two channels for each frame
        std::swap_ranges(data.begin() + channel2_start,
                        data.begin() + (channel2_start + channel_size),
                        data.begin() + channel_offset + channel2_start);

        // Write the swapped frame to the output file 
        out_data.write(data.data(), frame_size);                    
        file_pos += frame_size;
    }
    out_data.close();
}

void Funcs::clip_channel_memory(int channel, int minimum, int maximum) {
    // Check if the arguments are valid
    if(channel > channels - 1 || maximum > 255 || minimum < 0 || minimum > maximum || channel < 0 || maximum < minimum) {
        std::cerr << "Invalid arguments" << std::endl;
        return;
    }
    std::cout << "clip channel memory" << std::endl;
    // Calculate the start of the channel to be clipped
    int channel_start = channel * channel_size; 
    std::vector<char> data(frame_size);
    // Loop until the end of the file
    while (file_pos < file_size) {
        // Read the data in frames
        file_data.read(data.data(), frame_size);
        // Loop through the channel
        for (int j = channel_start; j < channel_size + channel_start; j++) {
            // Clip the channel
            if (data[j] < minimum) {
                data[j] = minimum;
            } else if (data[j] > maximum) {
                data[j] = maximum;
            }
            }
        // Write the clipped frame to the output file
        out_data.write(data.data(), frame_size);
        file_pos += frame_size;                    
    }
    out_data.close();
}

void Funcs::scale_channel_memory(int channel, float scale_factor) {
    // Check if the arguments are valid
    std::cout << "scale channel memory" << std::endl;
    if(channel > channels - 1 || scale_factor < 0 || channel < 0) {
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    // Calculate the start of the channel to be scaled
    int channel_start = channel * channel_size;
    float temp;
    std::vector<char> data(frame_size);
    // Loop until the end of the file
    while (file_pos < file_size) {
        // Read the data in frames
        file_data.read(data.data(), frame_size);
        // Loop through the channel      
        for (int j = channel_start; j <  channel_size + channel_start; j++) {
            // Scale the channel
            temp = (static_cast<unsigned char>(data[j]) * scale_factor);
            if (temp > 255) {
                temp = 255;
            }
            // Store the scaled value
            data[j] = static_cast<unsigned char>(temp);
            }   
        // Write the scaled frame to the output file
        out_data.write(data.data(), frame_size);
        file_pos += frame_size;
    }
    out_data.close();
}

void Funcs::grayscale_memory() {
    // Check if the channel count is valid
    if (channels != 3) {
        std::cerr << "Invalid channel count" << std::endl;
        return;
    }

    std::vector<char> data(frame_size);
    // Loop until the end of the file
    while (file_pos < file_size) {
        // Read the data in frames
        file_data.read(data.data(), frame_size);
            for (int j = 0; j < channel_size; j++){
                unsigned char grayscale = ((data[j] * 0.299) +
                                        (data[j + channel_size] * 0.587) +
                                        (data[j + channel_size * 2] * 0.114));
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2 * channel_size)] = grayscale;
            }   
        
        out_data.write(data.data(), frame_size);
        file_pos += frame_size;
    }
    out_data.close();
}

void Funcs::reverse_speed() {
    // Set the chunk size and remainder bytes
    
    chunk_size = sizeof(unsigned char);
    // Set the file position to read last chunk
    file_pos = file_size - frame_size;
    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos >= 11) {
        // Read the chunk of data
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        // Reverse the chunk of data in parallel
        for (int i = 0; i < frame_size; i ++) {
            file_data.read(data.data(), chunk_size);
            out_data.write(data.data(), chunk_size);
        file_pos -= frame_size;       
}
    }
}

void Funcs::swap_channel_speed(int channel1, int channel2) {
    // Check if the channels passed are valid
    if (channel1 > channels - 1 || channel2 == channel1 || channel2 < 0 || channel2 < 0) {
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }
    // Calculate the offset between the two channels
    std::vector<char> data(chunk_size);
    int channel_offset = (channel1 - channel2) * channel_size;
    int channel2_start = channel2 * channel_size;
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {
        // Read the chunk of data
        file_data.read(data.data(), chunk_size);
        // Swap the two channels for each frame in parallel
        #pragma omp parallel
        for (int i = 0; i < chunk_size; i += frame_size){
            std::swap_ranges(data.begin() + i + channel2_start, 
                             data.begin() + i + (channel2_start + channel_size),
                             data.begin() + i + channel_offset + channel2_start);
        }
        // Write the swapped chunk of data to the output file
        out_data.write(data.data(), chunk_size);                    
        file_pos += chunk_size;
    }
    // Repeat everything for the remainder bytes
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    #pragma omp parallel
    for (int i = 0; i < remainder_bytes; i += frame_size){
        std::swap_ranges(data.begin() + i + channel2_start, 
                         data.begin() + i + (channel2_start + channel_size),
                         data.begin() + i + channel_offset + channel2_start); 
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::clip_channel_speed(int channel, int minimum, int maximum) {
    // Check if the arguments are valid
    if(channel > channels - 1) {
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }

    int channel_start = channel * channel_size;
    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {
        // Read the chunk of data
        file_data.read(data.data(), chunk_size);
        # pragma omp parallel for
        for (int i = channel_start; i < chunk_size; i += frame_size) {
            for (int j = i; j < i + channel_size; j++) {
                if (data[j] < minimum) {
                    data[j] = minimum;
                } else if (data[j] > maximum) {
                    data[j] = maximum;
                }
            }
        }
        // Write the clipped chunk of data to the output file
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;                        
    }
    // Repeat everything for the remainder bytes
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    # pragma omp parallel
    for (int i = channel_start; i < remainder_bytes; i += frame_size) {
        for (int j = i; j < i + channel_size; j++) {
            if (data[j] < minimum) {
                data[j] = minimum;
            } else if (data[j] > maximum) {
                data[j] = maximum;
            }
        }
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::scale_channel_speed(int channel, float scale_factor) {
    // Check if the arguments are valid
    if(channel > channels - 1 || scale_factor < 0) {
        std::cerr << "Invalid channel" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }
    // Calculate the start of the channel to be scaled
    int channel_start = channel * channel_size;
    float temp;
    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {
        file_data.read(data.data(), chunk_size);
        // Increment in frames from the start of the channel to be scaled
        // In parallel
        # pragma omp parallel for
        for (int i = channel_start; i < chunk_size; i += frame_size) {
            for (int j = i; j < i + channel_size; j++) {
                temp = (static_cast<unsigned char>(data[j]) * scale_factor);
                if (temp > 255) {
                    temp = 255;
                }
                data[j] = static_cast<unsigned char>(temp);
            }
        }
        // Write the scaled chunk of data to the output file
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    // Repeat everything for the remainder bytes
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    # pragma omp parallel for
    for (int i = channel_start; i < remainder_bytes; i += frame_size) {
        for (int j = i; j < i + channel_size; j++) {
            temp = (static_cast<unsigned char>(data[j]) * scale_factor);
            if (temp > 255) {
                temp = 255;
            }
            data[j] = static_cast<unsigned char>(temp);
        }
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}

void Funcs::grayscale_speed() {
    // Check if the channel count is valid
    if (channels != 3) {
        std::cerr << "Invalid channel count" << std::endl;
        return;
    }
    // Set the chunk size and remainder bytes
    chunk_size = frame_size * 8;
    remainder_bytes = (file_size - 11) % chunk_size;
    if (remainder_bytes == 0) {
        remainder_bytes = chunk_size;
    }

    std::vector<char> data(chunk_size);
    // Loop until the remainder bytes
    while (file_pos < (file_size - chunk_size)) {   
        file_data.seekg(file_pos, std::ios::beg);
        file_data.read(data.data(), chunk_size);
        // Increment in frames from the start of the chunk
        # pragma omp parallel for
        for (int i = 0; i < chunk_size; i += frame_size) {
            for (int j = i; j < i + channel_size; j++) {
                unsigned char grayscale = (data[j] * 0.299 + 
                                            (data[j + channel_size] * 0.587) + 
                                            (data[j + channel_size * 2] * 0.114));
                data[j] = grayscale;
                data[j + channel_size] = grayscale;
                data[j + (2 * channel_size)] = grayscale;
            }   
        }
        out_data.write(data.data(), chunk_size);
        file_pos += chunk_size;
    }
    // Repeat everything for the remainder bytes    
    data.resize(remainder_bytes);
    file_data.read(data.data(), data.size());
    # pragma omp parallel for
    for (int i = 0; i < remainder_bytes; i += frame_size) {
        for (int j = i; j < i + channel_size; j++) {
            unsigned char grayscale = ceil(data[j] * 0.299 + 
                                           (data[j + channel_size] * 0.587) + 
                                           (data[j + channel_size * 2] * 0.114));
            data[j] = grayscale;
            data[j + channel_size] = grayscale;
            data[j + (2 * channel_size)] = grayscale;
        }
    }
    out_data.write(data.data(), remainder_bytes);
    out_data.close();
}
