#ifndef _OSCC_TEST_MOCK_DAC_MCP49XX_H_
#define _OSCC_TEST_MOCK_DAC_MCP49XX_H_

class DAC_MCP49xx {

    public:
        enum Model {
            MCP4901 = 1, /* single, 8-bit */
            MCP4911,     /* single, 10-bit */
            MCP4921,     /* single, 12-bit */
            MCP4902,     /* dual, 8-bit */
            MCP4912,     /* dual, 10-bit */
            MCP4922      /* dual, 12-bit */
        };

        DAC_MCP49xx(Model _model, int _ss_pin, int _ldac_pin = -1);

        void outputA(unsigned short _out);
        void outputB(unsigned short _out);
};

#endif
