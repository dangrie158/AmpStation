#include "AdvancedIO.h"

using namespace IO;

PinChangeInterruptPort PinChangeInterruptPort::PortB = {&PCMSK0, PCIE0, Port::PortB};
PinChangeInterruptPort PinChangeInterruptPort::PortC = {&PCMSK1, PCIE1, Port::PortC};
PinChangeInterruptPort PinChangeInterruptPort::PortD = {&PCMSK2, PCIE2, Port::PortD};

ISR(PCINT0_vect)
{
    IO::PCI0Interrupt();
}

ISR(PCINT1_vect)
{
    IO::PCI1Interrupt();
}

ISR(PCINT2_vect)
{
    IO::PCI2Interrupt();
}

void IO::PCI0Interrupt()
{
    PinChangeInterruptPort::PortB.call_listeners();
}

void IO::PCI1Interrupt()
{
    PinChangeInterruptPort::PortC.call_listeners();
}

void IO::PCI2Interrupt()
{
    PinChangeInterruptPort::PortD.call_listeners();
}
