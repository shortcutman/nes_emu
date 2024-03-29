//
//  instructions.cpp
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#include "cpu.hpp"

#include <sstream>
#include <iomanip>
#include <iostream>

#include "registers.hpp"
#include "memory.hpp"

namespace {

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

}

nes_emu::CPU::CPU() :
    _registers(new nes_registers()),
    _ops(CPU::buildOpCodeMap(this)),
    _interrupt(Interrupt::None)
{
    _registers->programCounter = 0xFFFC;
}

nes_emu::CPU::~CPU() {
}

void nes_emu::CPU::setMemory(std::shared_ptr<nes_emu::Memory> memory) {
    _memory = memory;
    
    _memory->setNMIInterruptCallback([this] () {
        _interrupt = Interrupt::NMI;
    });
}

void nes_emu::CPU::setPC(uint16_t addr) {
    _registers->programCounter = addr;
}

void nes_emu::CPU::executeOne() {
    if (checkAndSetupInterrupt()) {
        return;
    }
    
    auto opCode = _memory->read_uint8(_registers->programCounter);
    auto opCodeComponents = _ops[opCode];
    _registers->programCounter++;
    
    if (opCodeComponents.func) {
        opCodeComponents.func();
    } else {
        std::stringstream ss;
        ss << "Unimplemented instruction, opcode: 0x" << std::hex << std::setfill('0') << std::setw(2) << (int)opCode;
        throw std::runtime_error(ss.str());
    }
    
    _memory->advanceClock(opCodeComponents.cycles);
}

std::string nes_emu::CPU::printStatus() const {
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

void nes_emu::CPU::aax(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _registers->accumulator & _registers->x;
    _memory->write(opAddress, value);
}

void nes_emu::CPU::adc(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t argument = _memory->read_uint8(opAddress);
    adc_impl(argument);
}

void nes_emu::CPU::instrAnd(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t argument = _memory->read_uint8(opAddress);

    _registers->accumulator &= argument;
    
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::asl(AddressMode mode) {
    uint8_t value;
    uint16_t opAddress = 0x0000;

    if (mode == AddressMode::Accumulator) {
        value = _registers->accumulator;
    } else {
        opAddress = decodeOperandAddress(mode, *_registers, *_memory);
        value = _memory->read_uint8(opAddress);
    }

    if (value & 0x80) {
        _registers->statusRegister |= nes_registers::StatusFlags::CarryFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::CarryFlag;
    }
    
    value = value << 1;
    setNumberFlags(value);
    
    if (mode == AddressMode::Accumulator) {
        _registers->accumulator = value;
    } else {
        _memory->write(opAddress, value);
    }
}

void nes_emu::CPU::bcc() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::CarryFlag));
}

void nes_emu::CPU::bcs() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::CarryFlag);
}

void nes_emu::CPU::beq() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::ZeroFlag);
}

void nes_emu::CPU::bit(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t value  = _memory->read_uint8(opAddress);
    
    if (value & _registers->accumulator) {
        _registers->statusRegister &= ~nes_registers::StatusFlags::ZeroFlag;
    } else {
        _registers->statusRegister |= nes_registers::StatusFlags::ZeroFlag;
    }
    
    _registers->setStatusFlag(nes_registers::StatusFlags::NegativeFlag, value & 0b10000000);
    _registers->setStatusFlag(nes_registers::StatusFlags::OverflowFlag, value & 0b01000000);
}

void nes_emu::CPU::bmi() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::NegativeFlag);
}

void nes_emu::CPU::bne() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::ZeroFlag));
}

void nes_emu::CPU::bpl() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::NegativeFlag));
}

void nes_emu::CPU::brk() {
    stack_pushu16(_registers->programCounter);
    stack_push(_registers->statusRegister);
    _registers->programCounter = _memory->read_uint16(0xFFFE);
    _registers->setStatusFlag(nes_registers::StatusFlags::BreakCommand, true);
}

void nes_emu::CPU::bvc() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::OverflowFlag));
}

void nes_emu::CPU::bvs() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::OverflowFlag);
}

void nes_emu::CPU::clc() {
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, false);
}

void nes_emu::CPU::cld() {
    _registers->setStatusFlag(nes_registers::StatusFlags::DecimalModeFlag, false);
}

void nes_emu::CPU::cli() {
    _registers->setStatusFlag(nes_registers::StatusFlags::InterruptDisable, false);
}

void nes_emu::CPU::clv() {
    _registers->setStatusFlag(nes_registers::StatusFlags::OverflowFlag, false);
}

void nes_emu::CPU::cmp(AddressMode mode) {
    compare_impl(mode, _registers->accumulator);
}

void nes_emu::CPU::cpx(AddressMode mode) {
    compare_impl(mode, _registers->x);
}

void nes_emu::CPU::cpy(AddressMode mode) {
    compare_impl(mode, _registers->y);
}

void nes_emu::CPU::dcp(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory, false);
    auto value = _memory->read_uint8(opAddress);
    value--;
    int result = _registers->accumulator - value;
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, result >= 0);
    _registers->setStatusFlag(nes_registers::StatusFlags::ZeroFlag, result == 0);
    _registers->setStatusFlag(nes_registers::StatusFlags::NegativeFlag, result & 0x80);
    
    _memory->write(opAddress, value);
}

void nes_emu::CPU::dec(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress) - 1;
    _memory->write(opAddress, value);
    setNumberFlags(value);
}

void nes_emu::CPU::dex() {
    _registers->x -= 1;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::dey() {
    _registers->y -= 1;
    setNumberFlags(_registers->y);
}

void nes_emu::CPU::eor(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);

    _registers->accumulator ^= value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::inc(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress) + 1;
    _memory->write(opAddress, value);
    setNumberFlags(value);
}

void nes_emu::CPU::inx() {
    _registers->x += 1;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::iny() {
    _registers->y += 1;
    setNumberFlags(_registers->y);
}

void nes_emu::CPU::isc(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory, false);
    auto value = _memory->read_uint8(opAddress) + 1;
    _memory->write(opAddress, value);
    
    adc_impl(~value);
}

void nes_emu::CPU::jmp(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _registers->programCounter = opAddress;
}

void nes_emu::CPU::jsr() {
    uint16_t opAddress = decodeOperandAddress(AddressMode::Absolute, *_registers, *_memory);
    stack_pushu16(_registers->programCounter - 1);
    _registers->programCounter = opAddress;
}

void nes_emu::CPU::lax(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->accumulator = value;
    _registers->x = value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::lda(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->accumulator = value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::ldx(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->x = value;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::ldy(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->y = value;
    setNumberFlags(_registers->y);
}

void nes_emu::CPU::lsr(AddressMode mode) {
    uint8_t value;
    uint16_t opAddress = 0x0000;

    if (mode == AddressMode::Accumulator) {
        value = _registers->accumulator;
    } else {
        opAddress = decodeOperandAddress(mode, *_registers, *_memory);
        value = _memory->read_uint8(opAddress);
    }

    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, value & 0x01);
    
    value = value >> 1;
    setNumberFlags(value);
    
    if (mode == AddressMode::Accumulator) {
        _registers->accumulator = value;
    } else {
        _memory->write(opAddress, value);
    }
}

void nes_emu::CPU::nop() {
}

void nes_emu::CPU::dop(AddressMode mode) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"

    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    (uint16_t)opAddress;
    
#pragma clang diagnostic pop
}

void nes_emu::CPU::ora(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);

    _registers->accumulator |= value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::pha() {
    stack_push(_registers->accumulator);
}

void nes_emu::CPU::php() {
    stack_push(_registers->statusRegister | nes_registers::StatusFlags::BFlag | nes_registers::StatusFlags::BreakCommand);
}

void nes_emu::CPU::pla() {
    _registers->accumulator = stack_pop();
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::plp() {
    popStackAndSetStatus();
}

void nes_emu::CPU::rol(AddressMode mode) {
    uint8_t value;
    uint16_t opAddress = 0x0000;

    if (mode == AddressMode::Accumulator) {
        value = _registers->accumulator;
    } else {
        opAddress = decodeOperandAddress(mode, *_registers, *_memory);
        value = _memory->read_uint8(opAddress);
    }

    auto oldCarryFlag = _registers->getStatusFlag(nes_registers::StatusFlags::CarryFlag);
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, value & 0x80);
    
    value = value << 1;
    if (oldCarryFlag) {
        value |= 0x01;
    }
    setNumberFlags(value);
    
    if (mode == AddressMode::Accumulator) {
        _registers->accumulator = value;
    } else {
        _memory->write(opAddress, value);
    }
}

void nes_emu::CPU::rla(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory, false);
    uint8_t value = _memory->read_uint8(opAddress);

    auto oldCarryFlag = _registers->getStatusFlag(nes_registers::StatusFlags::CarryFlag);
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, value & 0x80);
    
    value = value << 1;
    if (oldCarryFlag) {
        value |= 0x01;
    }
    
    _memory->write(opAddress, value);
    
    _registers->accumulator &= value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::ror(AddressMode mode) {
    uint8_t value;
    uint16_t opAddress = 0x0000;

    if (mode == AddressMode::Accumulator) {
        value = _registers->accumulator;
    } else {
        opAddress = decodeOperandAddress(mode, *_registers, *_memory);
        value = _memory->read_uint8(opAddress);
    }

    auto oldCarryFlag = _registers->getStatusFlag(nes_registers::StatusFlags::CarryFlag);
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, value & 0x01);
    
    value = value >> 1;
    if (oldCarryFlag) {
        value |= 0x80;
    }
    setNumberFlags(value);
    
    if (mode == AddressMode::Accumulator) {
        _registers->accumulator = value;
    } else {
        _memory->write(opAddress, value);
    }
}

void nes_emu::CPU::sre(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory, false);
    uint8_t value = _memory->read_uint8(opAddress);

    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, value & 0x01);
    
    value = value >> 1;
    _memory->write(opAddress, value);
    
    _registers->accumulator ^= value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::rra(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory, false);
    auto value = _memory->read_uint8(opAddress);
    
    auto oldCarry = _registers->getStatusFlag(nes_registers::StatusFlags::CarryFlag);
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, value & 0x01);
    auto newValue = value >> 1;
    if (oldCarry) {
        newValue |= 0x80;
    }
    
    _memory->write(opAddress, newValue);
    adc_impl(newValue);
}


void nes_emu::CPU::rti() {
    popStackAndSetStatus();
    _registers->programCounter = stack_popu16();
}

void nes_emu::CPU::rts() {
    _registers->programCounter = stack_popu16() + 1;
}

void nes_emu::CPU::sbc(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t argument = _memory->read_uint8(opAddress);
    adc_impl(~argument);
}

void nes_emu::CPU::sec() {
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, true);
}

void nes_emu::CPU::sed() {
    _registers->setStatusFlag(nes_registers::StatusFlags::DecimalModeFlag, true);
}

void nes_emu::CPU::sei() {
    _registers->setStatusFlag(nes_registers::StatusFlags::InterruptDisable, true);
}

void nes_emu::CPU::slo(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory, false);
    auto value = _memory->read_uint8(opAddress);
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, value & 0x80);
    value = value << 1;
    _memory->write(opAddress, value);
    
    _registers->accumulator |= value;
    
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::sta(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory, false);
    _memory->write(opAddress, _registers->accumulator);
}

void nes_emu::CPU::stx(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->x);
}

void nes_emu::CPU::sty(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->y);
}

void nes_emu::CPU::tax() {
    _registers->x = _registers->accumulator;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::tay() {
    _registers->y = _registers->accumulator;
    setNumberFlags(_registers->y);
}

void nes_emu::CPU::tsx() {
    _registers->x = _registers->stackPointer;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::txa() {
    _registers->accumulator = _registers->x;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::txs() {
    _registers->stackPointer = _registers->x;
}

void nes_emu::CPU::tya() {
    _registers->accumulator = _registers->y;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::top(AddressMode mode) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"

    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    (uint16_t)opAddress;
    
#pragma clang diagnostic pop
}

void nes_emu::CPU::setNumberFlags(uint8_t lastOperationValue) {
    if (lastOperationValue == 0) {
        _registers->statusRegister |= nes_registers::StatusFlags::ZeroFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::ZeroFlag;
    }
    
    if ((lastOperationValue & 0x80) != 0) {
        _registers->statusRegister |= nes_registers::StatusFlags::NegativeFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::NegativeFlag;
    }
}

void nes_emu::CPU::adc_impl(uint8_t argument) {
    uint16_t addResult = argument + _registers->accumulator + (_registers->statusRegister & nes_registers::StatusFlags::CarryFlag);

    if (addResult > 0xFF) {
        _registers->statusRegister |= nes_registers::StatusFlags::CarryFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::CarryFlag;
    }
    
    //overflow flag logic from: https://stackoverflow.com/a/29224684
    if (~(_registers->accumulator ^ argument) & (_registers->accumulator ^ addResult) & 0x80) {
        _registers->statusRegister |= nes_registers::StatusFlags::OverflowFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::OverflowFlag;
    }
    
    _registers->accumulator = addResult;
    
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::branch_impl(bool test) {
    auto incrementData = _memory->read_uint8(_registers->programCounter);
    int8_t incrementValue = *reinterpret_cast<int8_t*>(&incrementData);
    _registers->programCounter++;
    
    if (test) {
        uint16_t nextInstruction = _registers->programCounter + incrementValue;
        
        if ((nextInstruction & 0xFF00) != (_registers->programCounter & 0xFF00)) {
            _memory->advanceClock(1); //new page
        }
        
        _registers->programCounter = nextInstruction;
        _memory->advanceClock(1);
    }
}

void nes_emu::CPU::compare_impl(AddressMode mode, uint8_t registerInput) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    
    uint8_t result = registerInput - value;
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, registerInput >= value);
    _registers->setStatusFlag(nes_registers::StatusFlags::ZeroFlag, registerInput == value);
    _registers->setStatusFlag(nes_registers::StatusFlags::NegativeFlag, result & 0x80);
}

void nes_emu::CPU::popStackAndSetStatus() {
    auto statusFlags = stack_pop();
    
    if (_registers->statusRegister & nes_registers::StatusFlags::BreakCommand) {
        statusFlags |= nes_registers::StatusFlags::BreakCommand;
    } else {
        statusFlags &= ~nes_registers::StatusFlags::BreakCommand;
    }
    
    if (_registers->statusRegister & nes_registers::StatusFlags::BFlag) {
        statusFlags |= nes_registers::StatusFlags::BFlag;
    } else {
        statusFlags &= ~nes_registers::StatusFlags::BFlag;
    }
    
    _registers->statusRegister = statusFlags;

}

void nes_emu::CPU::stack_push(uint8_t value) {
    _memory->write(nes_emu::Memory::StackStart + _registers->stackPointer, value);
    _registers->stackPointer -= 1;
}

uint8_t nes_emu::CPU::stack_pop() {
    _registers->stackPointer += 1;
    auto value = _memory->read_uint8(nes_emu::Memory::StackStart + _registers->stackPointer);
    return value;
}

void nes_emu::CPU::stack_pushu16(uint16_t value) {
    uint8_t hiByte = (value >> 8);
    uint8_t loByte = value;
    
    stack_push(hiByte);
    stack_push(loByte);
}

uint16_t nes_emu::CPU::stack_popu16() {
    auto loByte = stack_pop();
    auto hiByte = stack_pop();
    
    uint16_t value = hiByte;
    value = value << 8;
    value += loByte;
    return value;
}

bool nes_emu::CPU::checkAndSetupInterrupt() {
    if (_interrupt == Interrupt::None) {
        return false;
    } else if (_interrupt != Interrupt::NMI && _registers->getStatusFlag(nes_registers::StatusFlags::InterruptDisable)) {
        _interrupt = Interrupt::None;
        return false;
    }
    
    stack_pushu16(_registers->programCounter);
    stack_push(_registers->statusRegister | nes_registers::StatusFlags::BFlag | nes_registers::StatusFlags::BreakCommand);
    
    _registers->setStatusFlag(nes_registers::StatusFlags::InterruptDisable, true);
    
    _registers->programCounter = _interrupt;
    _registers->programCounter = decodeOperandAddress(AddressMode::Absolute, *_registers, *_memory);
    
    _interrupt = Interrupt::None;
    
    _memory->advanceClock(7);
    
    return true;
}
