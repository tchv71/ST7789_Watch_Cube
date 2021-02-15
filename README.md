# ST7789_Watch_Cube
Special version for STM32F103C8TX (Blue Pill)

Compiled in STM32CubeIDE without Arduino

- Roger's Arduino core is mostly striped out.
- SPIClass is replaced by dummy implementation needed only for ST7789 graphics driver to be functional
- digitalRead and digitalWrite are implemented using HAL

See https://github.com/tchv71/ST7789_Watch_stm for Arduino's implementation
