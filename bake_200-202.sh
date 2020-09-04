#!/bin/sh
# for #200,201,202
temp_start=$1
temp_end=650
set_value=$temp_start
bake_time=3600
# pre=bake : 1.5hours 3600sec * 1.5 = 5400sec. 
pre_bake=0
# post_bake : 10hours 3600sec * 16 = 64800sec.
post_bake=64800
delta_t=1
# heat up : 20min
# heat up by 20(min) * 60(sec/min) = 1200 sec
# heat up frmo 38 deg to 65deg. so delta T is 65 - 38 = 27degree.
# 1200sec / 27deg = 44.4sec
sleep_delay=5

echo "prebake has been started..."
sleep $pre_bake

# skip heat up process
# while [ $set_value -le $temp_end ]
# do
#     ./sdc15 /dev/ttyUSB0 1 $set_value
#     ./sdc15 /dev/ttyUSB0 2 $set_value
#     ./sdc15 /dev/ttyUSB0 3 $set_value
#     set_value=`expr $set_value + $delta_t`
#     sleep $sleep_delay
# done

echo "postbake has been started..."
sleep $post_bake
    ./sdc15 /dev/ttyUSB0 1 0
    ./sdc15 /dev/ttyUSB0 2 0
    ./sdc15 /dev/ttyUSB0 3 0
echo "Baked...u._.u"

