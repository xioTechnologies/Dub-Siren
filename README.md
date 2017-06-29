# Dub Siren

A monophonic synthesiser comprised of an LFO, VCO, and Delay.  The synthesiser is controlled by 9 potentiometers and 3 buttons.  Configurations can be stored to one of 10 preset keys.  Pressing a preset key will override the potentiometers and buttons with the stored configuration.

The processor used is [PIC32MZ](http://www.microchip.com/wwwproducts/en/PIC32MZ2048EFH064).  This is a 32-bit device with a FPU, 512 KB of RAM, 2 MB of program space, clocked at 252 MHz.  The processor is deliberately over-spec'd to simplify development and allow new ideas to be explored in the future.  The [DAC](https://www.cirrus.com/products/cs4354/) is a 24-bit 192 kHz stereo device configured to run at 96 kHz mono.

## Synthesiser elements

##### LFO
- Waveforms: sine, triangle, sawtooth, square, stepped triangle, stepped sawtooth
- Shape: adjusts waveform shape
- Frequency: 0 Hz to 15 Hz
- Amplitude: -1 to +1
- Gate control: enable/disable (automatically opens gate after one cycle)

##### VCO
- Waveforms: sine, triangle, sawtooth, square, pulse, 1-bit noise
- Frequency: 0.5 Hz to 5 kHz

##### Delay
- Time: 0 s to 1.33 s
- Feedback: 0% to 100%
- Filter: 3rd-order low-pass with adjustable corner frequency, all-pass (filter disabled), 3rd-order high-pass with adjustable corner frequency

## User instructions (etched on the back panel)

![](https://github.com/xioTechnologies/Dub-Siren/blob/master/Images/User%20Instructions.png?raw=true)

## BOM

* PCBA
* Top plate
* Bottom plate
* Spacer (x4)
* [8mm hex stand-off](https://www.digikey.co.uk/products/en?keywords=36-24432-ND) (x4)
* [M3 5mm screw](http://www.ebay.co.uk/itm/M3-BLACK-HIGH-TENSILE-10-9-BUTTON-HEAD-ALLEN-BOLTS-SELF-COLOUR-DOME-SOCKET-SCREW/121829578789) (x4)
* [M3 10mm screw](http://www.ebay.co.uk/itm/M3-BLACK-HIGH-TENSILE-10-9-BUTTON-HEAD-ALLEN-BOLTS-SELF-COLOUR-DOME-SOCKET-SCREW/121829578789) (x4)
* [3M rubber feet](http://www.ebay.co.uk/itm/100x-Silicone-RUBBER-FEET-Bumpons-CLEAR-Round-Self-Adhesive-Anti-Slip-Circles-3M-/172636687398) (x4)
