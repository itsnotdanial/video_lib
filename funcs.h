#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Funcs{
    private:

        long frame_no;
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
        int file_size;
        int remainder_bytes; // define in file
        int file_pos; // define in file

        std::vector<char> data;

        std:: ofstream out;

    public:
        Funcs(const std::string &filename, const std::string &out);
        void extract_header_data();
        void reverse();
        void swap_channel(int channel1, int channel2);
        
        // int swap_channel_helper(unsigned char, int frame_size); // add mode in as well?
        void clip_channel(int channel, int minimum, int maximum);
        void scale_channel(int channel, float scale_factor);
};
