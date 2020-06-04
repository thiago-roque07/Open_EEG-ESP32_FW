This is the ESP32 Firmware of my Open_EEG project.
It is intended to be used with the hardware in my OpenEEG_HW repository

The ESP32 is supposed to acquire 3 channels of the EEG signal through the external ADC ADS1115 at a fixed sampling rate and then perform some filtering. 
One to remove DC level, through an 2 pole highpass IIR filter.
And another to remove the ubiquitous 60Hz noise from AC power grid. 

To enable the ADS1115 to read 3 channels in continuous mode, the sampling rate was set to its maximum frequency (860Hz) and by each ADC read, the analog input is changed in a sequencial manner. In this way, each channel read is performed by a effectively sampling rate of (860/3)Hz, or 286,66Hz. 
