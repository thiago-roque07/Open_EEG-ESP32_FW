This is the ESP32 Firmware of my Open_EEG project.
It is intended to be used with the hardware in my OpenEEG_HW repository

The ESP32 is supposed to acquire the EEG signal through the external ADC ADS1115 at a sampling rate of 300Hz and perform a two filterings.
One to remove DC level, through an 2 pole highpass IIR filter.
And another to remove the ubiquitous 60Hz noise from AC power grid. 
