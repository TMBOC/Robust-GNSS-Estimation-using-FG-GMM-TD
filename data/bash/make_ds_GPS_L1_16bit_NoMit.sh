#!/bin/bash

echo "Making data for L1_16bit_GPS_NoMit"
rnx_2_gtsam --obs L1_16bit_GPS_NoMit.rnx --sp3 jpl20705.sp3 --brdc_nav hour2560.19n --break_thresh 5.5 --break_window 1500  > L1_16bit_GPS_NoMit.gtsam

echo "finished with data for L1_16bit_GPS_NoMit"
