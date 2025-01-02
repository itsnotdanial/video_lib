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
        
        int chunk_size; // define in file
        int file_size;
        int remainder_bytes; // define in file
        int file_pos; // define in file

        std::vector<char> data;

        std:: ofstream out;

    public:
        Funcs(const std::string &filename);
        void extract_header_data();
        void reverse();
        // void swap_channel();
        // void clip_channel();
        // void scale_channel();
};
