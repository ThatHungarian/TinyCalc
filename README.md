# TinyCalc
A really small calculator

To compile it yourself use the command below:  

gcc -Os -flto -fno-asynchronous-unwind-tables -fno-ident -fno-stack-protector -fomit-frame-pointer -fmerge-all-constants -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--build-id=none -Wl,--file-alignment,16 -Wl,--section-alignment,16 -nostdlib -e entry -s -o calc.exe calc.c -lkernel32
