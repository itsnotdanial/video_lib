#include <iostream>
#include <fstream>
#include <vector>


using namespace std;

class Balanced{
    public:
        string filename;
        string filename_out;

        //Constructor
        Balanced(string filename){
            this->filename = filename;
            this->filename_out = "out.bin";
            const int data_size = 1024;
        };

        void read_file(){
            //copied line
            //open file in binary
            
            std::ifstream file_data(filename, std::ifstream::binary);
            std::ofstream out_data(filename_out, std::ios::binary | std::ios::app);
            std::vector<char> data (1024); //reads only the first 1024 bytes

            // while (file_data.read(data.data(), data.size())) {
            //     std::streamsize bytes_read = file_data.gcount(); // change variable name?  
            //     out_data.write(data.data(), bytes_read);
            //     std::fill(data.begin(), data.end(), 0);  
 
            // }
            
            // if (file_data.gcount() > 0) {
            //     out_data.write(data.data(), file_data.gcount());

            // }
            long value;
            file_data.read(reinterpret_cast<char*>(&value), sizeof(long));
            cout << "Value " << value;
            
            file_data.close();
            out_data.close();

            return;
            }
        };

int main() {
    Balanced bal("test.bin");
    bal.read_file();
    return 0;

};
