# DFED_Plugin

This repository holds the code for the GCC plugin that can automatically add multiple DFE detection techniques which was developed during the PhD of Jens Vankeirsbilck and currently the techniques are being expaded by Brent De Blare and Mohaddaseh Nikseresht during their PhD. This plugin is meant to be compiled with g++-7 and to be executed with arm-none-eabi-gcc-7.3 .

# Build Instructions
Just execute `make` to build the plugin.

possibly needed, depending on your linux setup:

```$sudo apt install libgmp3-dev gcc-multilib g++-multilib linux-libc-dev -y```

## How to Use the Plugin
This section describes how to use the plugin. 

### Reserving Registers
Each supported DFE detection technique requires hardware registers to be implemented. Since the GCC plugin is a late RTL pass, these registers must be reserved while compiling the target code using the GCC option `-ffixed-r<number>` in the C and C++ flags. The table below shows which registers are needed by which technique for which supported ARM ISA.

Supported Technique | ARMv6-M | ARMv7-M | 
------------------- | ------- | ------- | 
FDEC_S_SP | Not Supported | r7 & r8 & r9 & r10 & r11 & r12|
FDEC | Not Supported | r7 & r8 & r9 & r10 & r11 & r12| 
FDFC |  Not Supported| r7 & r8 & r9 & r10 & r11 & r12| 
FDSC | Not Supported| r7 & r8 & r9 & r10 & r11 & r12| 
SWIFT | Not Supported | r7 & r8 & r9 & r10 & r11 & r12| 
SWIFT-R | Not Supported | r7 & r8 & r9 & r10 & r11 & r12|
S-SWIFT-R | Not Supported |  Need to be specified based on the user desired registers to protect |
DETECTOR | Not Supported | r5 & r6 & r11 | 
P-DETECTOR |Not Supported | r5 & r6 & r11 |

Since register r11 in ARMv7-M can be used as frame pointers, it might be necessary to add the GCC option `-fomit-frame-pointer` to the C and C++ flags of the target code.

### Second Stack
Important to know about this plugin, is that it needs a second descending stack to push and pop the used register(s) of the implemented technique. This means that the linker file must provide room for this second stack and that the startup code must initialize the stack pointer of the plugin.

The register used as stack pointer is register r6 on ARMv7-M. This register must thus also be reserved during compilation, using `-ffixed-r6` in the C and C++ flags of the target code. 

### Adding the Plugin to the Compilation Options
1) Specifying the plugin
To specify which plugin to use, the following must be added to the C and C++ flags of the target code: `-fplugin=<fullPathToPlugin>/DFED_plugin64.so` 

2) The plugin-arguments
To know which DFE detection technique to implement and which funtion(s) of the target code to protect, several plugin-arguments must be specified. 
* `-fplugin-arg-DFED_plugin64-function=<value>`: This argument specifies which function to protect with the selected DFE detection technique. <value> can have one out of two values:
   * *<functionName>*, then only that function of the target code is protected; or
   * *<empty>*, then all functions of the target code are protected. To explicitly exclude a function from protection, use the following function attribute for that function `__attribute__((noProtection))`
* `-fplugin-arg-DFED_plugin64-techniqueSpecific=<value>`: This argument specifies which technique to implement. For values, see the first column of the table above. 

## References to the Supported Techniques
Technique | DOI
--------- | ---
FDEC_S_SP | unpublished
FDEC | unpublished
FDFC | unpublished
FDSC | [10.1109/PRDC.2018.00037](10.1109/PRDC.2018.00037)
SWIFT | [10.1109/CGO.2005.34](https://doi.org/10.1109/CGO.2005.34)
SWIFT-R | [10.1109/DSN.2006.15](https://doi.org/10.1109/DSN.2006.15)
S-SWIFT-R | [10.1109/TII.2013.2248373](https://doi.org/10.1007/s10836-013-5416-6)
DETECTOR | [10.1109/ICSRS53853.2021.9660636](https://doi.org/10.1109/ICSRS53853.2021.9660636)
P-DETECTOR |[10.1109/TR.2023.3294795](https://doi.org/10.1109/TR.2023.3294795)

