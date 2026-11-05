#pragma once

namespace STM32 {

class PI {
public:
    PI(const PI& other) = delete;
    PI(PI&& other) = delete;
    PI& operator=(const PI& other) = delete;
    PI& operator=(PI&& other) = delete;
    
    PI() = default;
    
    
    ~PI() = default;
};

}
