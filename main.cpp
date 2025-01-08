#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "funcs.h"
#include <chrono>

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    Funcs funcs(argv[1], argv[2]);
    std::cout << " ARGC "<< argc << std::endl;
    switch (argc) {
        case 4: // if only 4 args it must be reverse
            std::cout << argv[3] << std::endl; 
            if (argv[3] == std::string("reverse")) {
                funcs.reverse();
            } else {
                std::cerr << "Invalid argument" << std::endl;
                return 1;
            }
            break;

        case 5: // if 5 args it must be reverse with mode
            if (argv[3][0] == 'M') {
                // call reverse in memory mode       
            } else if (argv[3][0] == 'S') {
                // call reverse in speed mode     
            } else {
                std::cerr << "Invalid argument" << std::endl;
                return 1;
            }
            break;

        case 6: // if 6 args it must be swap_channel, clip_channel or scale_channel
            if (argv[3] == std::string("swap_channel")) {
                try {
                    int channel1 = std::stoi(argv[4]);
                    int channel2 = std::stoi(argv[5]);

                    if (channel1 > channel2) {
                        funcs.swap_channel(channel1, channel2);
                    } else {
                        funcs.swap_channel(channel2, channel1); // add arguments idk 
                    }
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    return 1;
                }
            } else if (argv[3] == std::string("clip_channel")) {
                std::cout << "CLIP CHANNELooooo" << std::endl;
                try {
                    std::cout << "CLIP CHANNEL" << std::endl;
                    int channel = std::stoi(argv[4]);
                    bool comma = false;
                    std::string range = argv[5];
                    std::string min;
                    std::string max; 
                    std::cout << "RANGE " << range << std::endl;
                    
                    for (int i = 1 ; i < range.length(); i++){
                        if (range[i] == ','){
                            comma = true;
                        }
                            else if (!comma) {
                                min += range[i];
                            } else {
                                max += range[i];
                            }
                        }
                    int minimum = std::stoi(min);
                    int maximum = std::stoi(max);
                    std::cout << "MINIMUM " << minimum << " MAXIMUM " << maximum << std::endl;

                    if (minimum > maximum || minimum < 0 || maximum > 255 || channel < 0) {
                        std::cerr << "Invalid range" << std::endl;
                        return 1; 
                    } // stick this all into a function

                    funcs.clip_channel(channel, minimum, maximum); 
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    return 1;
                }
            } else if (argv[3] == std::string("scale_channel")) {
                int channel = std::stoi(argv[4]);
                float scale_factor = std::stof(argv[5]); 
                funcs.scale_channel(channel, scale_factor); // add arguments idk    
            } else {
                std::cerr << "Invalid argument" << std::endl;
                return 1;
            }
            break;

        case 7:
            // handle case 6 if needed
            if (argv[4] == std::string("swap_channel")){
                try {
                    int channel1 = std::stoi(argv[5]);
                    int channel2 = std::stoi(argv[6]);
                    
                    if (channel1 < channel2){
                        int temp = channel1;
                        channel1 = channel2;
                        channel2 = temp;
                    }

                    if (argv[3][0] == 'M') {
                        // call memory swap
                        } else {
                        // call fast swap

                    }
                    break;
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    return 1;
                }
            } else if (argv[4] == std::string("clip_channel")) {
                try {
                    int channel = std::stoi(argv[5]);
                    bool comma = false;
                    std::string range = argv[6];
                    std::string min;
                    std::string max; 
                    
                    for (int i = 1 ; i < range.length(); i++){
                        if (range[i] == ','){
                            comma = true;
                        }
                            else if (comma) {
                                min += range[i];
                            } else {
                                max += range[i];
                            }
                        }
                    int minimum = std::stoi(min);
                    int maximum = std::stoi(max);

                    if (minimum > maximum || minimum < 0 || maximum > 255 || channel < 0) {
                        std::cerr << "Invalid range" << std::endl;
                        return 1; 
                    } // stick this all into a function
                    if (argv[3][0] == 'M') {
                        // call memory clip
                        } else {
                        // call fast clip

                    } 
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    return 1;
                }
            } else if (argv[4] == std::string("scale_channel")) {
                int channel = std::stoi(argv[5]);
                float scale_factor = std::stof(argv[6]);
                if (argv[3][0] == 'M') {
                    // call memory scale
                    } else {
                    // call fast scale

                }  
            } else {
                std::cerr << "Invalid argument" << std::endl;
                return 1;
            }
            break;

        default:
            std::cerr << "Invalid number of arguments" << std::endl;
            return 1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
    return 0;
}
