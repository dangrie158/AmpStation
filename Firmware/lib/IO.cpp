#include "IO.h"

using namespace IO;

const Port Port::PortB = {&PORTB, &DDRB, &PINB};
const Port Port::PortC = {&PORTC, &DDRC, &PINC};
const Port Port::PortD = {&PORTD, &DDRD, &PIND};
