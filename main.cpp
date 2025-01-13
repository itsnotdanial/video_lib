#include <iostream>
#include <vector>
#include <string>
#include "funcs.h"
#include <chrono>

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    Funcs funcs(argv[1], argv[2]);
    switch (argc) {
        case 4: // if only 4 args it must be reverse
            if (argv[3] == std::string("reverse")) {
                funcs.reverse();
            } else if (argv[3] == std::string("grayscale")) {
                funcs.grayscale();    
            } else {
                std::cerr << "Invalid argument" << std::endl;
                return 1;
            }
            break;

        case 5: // if 5 args it must be reverse or grayscale with a mode or swap_channel
            if (argv[3] == std::string("-M") && argv[4] == std::string("reverse")) {
                // call reverse in memory mode
                funcs.reverse_memory();       
            } else if (argv[3] == std::string("-S") && argv[4] == std::string("reverse")) {
                funcs.reverse_speed();     
            } else if(argv[3] == std::string("-M") && argv[4] == std::string("grayscale")) {
                funcs.grayscale_memory();
                
            }
            else if(argv[3] == std::string("-S") && argv[4] == std::string("grayscale")) {
                funcs.grayscale_memory();
            } else if(argv[3] == std::string("swap_channel")) {
                try{
                    int channel1 = argv[4][0] - '0';
                    int channel2 = argv[4][2] - '0';
                    if(channel1 < channel2){
                        int temp = channel1;
                        channel1 = channel2;
                        channel2 = temp;
                    }
                    funcs.swap_channel(channel1, channel2);
                    
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    return 1;
                }
            }
            else {
                std::cerr << "Invalid argument" << std::endl;
                return 1;
            }
            break;

        case 6: // if 6 args it must be swap_channel, clip_channel or scale_channel
            if (argv[4] == std::string("swap_channel")){
                try {
                    int channel1 = argv[5][0] - '0';
                    int channel2 = argv[5][2] - '0';
                    
                    if (channel1 < channel2){
                        int temp = channel1;
                        channel1 = channel2;
                        channel2 = temp;
                    }
                    if (argv[3] == std::string("-M")) {
                        funcs.swap_channel_memory(channel1, channel2);
                    } else if(argv[3] == std::string("-S")) {
                    funcs.swap_channel_speed(channel1, channel2);
                    } else {
                        std::cerr << "Invalid argument" << std::endl;
                    }
                    break;
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    return 1;
                }
            } else if (argv[3] == std::string("clip_channel")) {
                try {
                    int channel = std::stoi(argv[4]);
                    bool comma = false;
                    std::string range = argv[5];
                    std::string min;
                    std::string max; 
                    
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
            if (argv[4] == std::string("clip_channel")) {
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
                            else if (!comma) {
                                min += range[i];
                            } else {
                                max += range[i];
                            }
                        }
                    int minimum = std::stoi(min);
                    int maximum = std::stoi(max);
                    if (argv[3] == std::string("-M")) {
                        funcs.clip_channel_memory(channel, minimum, maximum);
                        } else if (argv[3] == std::string("-S")) {
                        funcs.clip_channel_speed(channel, minimum, maximum);

                    }
                    else{
                        std::cerr << "Invalid argument" << std::endl;
                    } 
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    return 1;
                }
            } else if (argv[4] == std::string("scale_channel")) {
                int channel = std::stoi(argv[5]);
                float scale_factor = std::stof(argv[6]);
                if (argv[3] == std::string("-M")) {
                    funcs.scale_channel_memory(channel, scale_factor);
                    } else if (argv[3] == std::string("-S")) {
                    funcs.scale_channel_speed(channel, scale_factor);

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
