# vme-easiroc-registers

Register values for VME-EASIROC boards.
Confirmed that it is the same as latest E40 setting. (/group/had/sks/E40/JPARC2020Feb/E40SubData2020Feb/vme-easiroc-registers)

## How to use
Create symbolic link for following three.
They will be loaded at the beginning of the frontend process.
(You can know more details in the userdevice.cc for the VME-EASIROC.)

- RegisterValue
- InputDAC
- PedeSup

## Role of each directory
**If you want to edit any values, you must create a new directory.**

1. RegisterValue
2. InputDAC
3. PedeSup