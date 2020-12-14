# int2intv
Intellivision rom converter for the Analogue NT mini noir jailbreak.

command line: int2intv -m X \<input file\> \<output file\>

  Switches: -m X, where X = 0-9,99 rom memory map to encode

To determine what map to use for a specific rom refer to https://docs.google.com/spreadsheet/ccc?key=0Ar_02usomyeqdHFyaGRnNWdLN1FzWTN3ajlacXBPWFE

Map #99 is used to convert the intellivision Executive ROM (CRC32 0xCBCE86F7) to the correct format for the Intellivision core on the NT mini noir jailbreak.

To build from source you can use your favorite flavor of the gnu c++ compiler.

Example:  g++ int2intv.cpp -s -static -o int2intv
