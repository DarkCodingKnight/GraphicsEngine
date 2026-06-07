#pragma once

#include <iostream>
#include <string>

namespace ConsoleText {

const std::string beginMessage = "\033[";
const std::string endMessage = "\033[0m";

//Colors
const std::string blackColor = "30m";
const std::string redColor = "31m";
const std::string greenColor = "32m";
const std::string yellowColor = "33m";
const std::string blueColor = "34m";
const std::string purpleColor = "35m";
const std::string turquoiseColor = "36m";
const std::string whiteColor = "37m";

const std::string errorMessage = "31mERROR :( ";

inline static void printText(std::string color, std::string message, std::string className = "") {
    if (className == "") {
        std::cout << beginMessage + color + message + endMessage << std::endl;
    }
    else std::cout << beginMessage + color + className + ": " + message + endMessage << std::endl;
}
inline void printError(const std::string message, std::string className = "") {
    if (className == "") {
        std::cout << beginMessage + errorMessage + message + endMessage << std::endl;
    }
    else std::cout << beginMessage + errorMessage + className + ": " + message + endMessage << std::endl;
}
inline void printBlack(const std::string message, std::string className = "") {
    printText(blackColor, message, className);
}
inline void printGreen(const std::string message, std::string className = "") {
    printText(greenColor, message, className);
}
inline void printRed(const std::string message, std::string className = "") {
    printText(redColor, message, className);
}
inline void printYellow(const std::string message, std::string className = "") {
    printText(yellowColor, message, className);
}
inline void printBlue(const std::string message, std::string className = "") {
    printText(blueColor, message, className);
}
inline void printPurple(const std::string message, std::string className = "") {
    printText(purpleColor, message, className);
}
inline void printTurquoise(const std::string message, std::string className = "") {
    printText(turquoiseColor, message, className);
}
inline void printWhite(const std::string message, std::string className = "") {
    printText(whiteColor, message, className);
}
inline void print(const std::string message, std::string className = "") {
    if (className == "") std::cout << message << std::endl;
    else std::cout << className + ": " + message << std::endl;
}
inline void printInt(const int message, std::string className = "") {
    if (className == "") std::cout << message << std::endl;
    else std::cout << className + ": " << message << std::endl;
}
inline void printNewLine() {
    std::cout << "\n";
}

}
