#!/bin/bash
python3 pgm_fpga.py -s=../eda/output_files/DE1_SoC_top.sof
if [[ -e "irq" ]]
then
    python3 upld_hps.py -a=../../soft/proj/app_timer_irq/Debug/app_timer_irq.axf
else
    python3 upld_hps.py
fi