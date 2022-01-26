//
//  integration_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 22/1/2022.
//

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>

#include "cartridge.hpp"
#include "memory.hpp"
#include "cpu.hpp"
#include "registers.hpp"

namespace {

using namespace nes_emu;

const std::string SPACER = "                             ";

template< typename T >
std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << ""
         << std::setfill ('0') << std::setw(sizeof(T)*2)
         << std::hex << i;
  return stream.str();
}

template<>
std::string int_to_hex( uint8_t i )
{
  std::stringstream stream;
  stream << ""
         << std::setfill ('0') << std::setw(2)
         << std::hex << static_cast<uint32_t>(i);
  return stream.str();
}

std::runtime_error formattedException(const std::string& msg, uint64_t line) {
    std::stringstream ss;
    ss << msg << " on line " << line;
    return std::runtime_error(ss.str());
}

class IntegrationTest : public nes_emu::CPU, public ::testing::Test {
public:
    std::string printStatus() const {
        std::stringstream ss;
        ss << int_to_hex(_registers->programCounter) << " ";
        
        uint8_t opCode = _memory->read_uint8(_registers->programCounter);
        auto opCodeComponents = _ops.at(opCode);
        
        for (int i = 0; i < 3; i++) {
            if (i < opCodeComponents.bytes) {
                ss << int_to_hex(_memory->read_uint8(_registers->programCounter + i)) << " ";
            } else {
                ss << "   ";
            }
        }
        
        ss << " " << opCodeComponents.shortHand << " " << SPACER;
        
        ss << "A:" << int_to_hex(_registers->a) << " ";
        ss << "X:" << int_to_hex(_registers->x) << " ";
        ss << "Y:" << int_to_hex(_registers->y) << " ";
        ss << "P:" << int_to_hex(_registers->statusRegister) << " ";
        ss << "SP:" << int_to_hex(_registers->stackPointer) << " ";
        ss << "CYC:" << "XXX" << " ";
        ss << "SL:" << "XXX" << " ";
        return ss.str();
    }
    
    bool statusCompare(const std::string& statusLine, uint64_t line) {
        std::istringstream iss(statusLine);
        
        uint16_t statusPC;
        iss >> std::hex >> statusPC;
        if (statusPC != _registers->programCounter) {
            throw formattedException("PC doesn't match", line);
        }
        
        auto opCode = _ops.at(_memory->read_uint8(statusPC));
        for (int i = 0; i < opCode.bytes; i++) {
            uint16_t byte;
            iss >> std::hex >> byte;
            if (byte != _memory->read_uint8(statusPC + i)) {
                throw formattedException("op code components don't match", line);
            }
        }
        
        uint16_t a, x, y, p, sp;
        
        iss.seekg(50);
        iss >> std::hex >> a;
        iss.seekg(55);
        iss >> std::hex >> x;
        iss.seekg(60);
        iss >> std::hex >> y;
        iss.seekg(65);
        iss >> std::hex >> p;
        iss.seekg(71);
        iss >> std::hex >> sp;
        
        if ( (a != _registers->a) ||
            (x != _registers->x) ||
            (y != _registers->y) ||
            (p != _registers->p) ||
            (sp != _registers->sp)) {
            throw formattedException("Registers don't match", line);
        }
        
        return true;
    }
};

TEST_F(IntegrationTest, nestest) {
    std::ifstream cartFile("/Users/daniel/Projects.nosync/nes_emu/nes_emu/nes_emu-tests/nestest/nestest.nes", std::fstream::in | std::fstream::binary);
    
    std::ifstream logFile("/Users/daniel/Projects.nosync/nes_emu/nes_emu/nes_emu-tests/nestest/nestest.log");

    ASSERT_TRUE(cartFile.good());

    _memory->_cartridge = nes_emu::Cartridge::cartridgeFromStream(cartFile);

    _registers->programCounter = 0xC000;
    _registers->stackPointer = 0xFD;
    _registers->statusRegister = 0x24;

    uint64_t count = 0;
    std::string logLine;
    
    while (1) {
        std::string statusLine = printStatus();
        std::getline(logFile, logLine);
        ASSERT_TRUE(statusCompare(statusLine, count));
        executeOne();
        count++;
    }
}

}
