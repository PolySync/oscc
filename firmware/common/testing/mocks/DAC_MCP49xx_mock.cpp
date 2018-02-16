#include <stdio.h>

#include "DAC_MCP49xx.h"

unsigned short g_mock_dac_output_a;
unsigned short g_mock_dac_output_b;

DAC_MCP49xx::DAC_MCP49xx(Model _model, int _ss_pin, int _ldac_pin)
{
}

void DAC_MCP49xx::outputA(unsigned short _out)
{
    g_mock_dac_output_a = _out;
}

void DAC_MCP49xx::outputB(unsigned short _out)
{
    g_mock_dac_output_b = _out;
}
