#!/bin/sh
temp_start=$1
temp_end=70
set_value=$temp_start
bake_time=3600
# pre_bake=5400 is No.186
pre_bake=5400
# pre=bake : 4hours 3600sec * 4 = 14400sec. 
# pre_bake=14400
# post_bake : 12hours 3600sec * 12 = 43200sec.
post_bake=43200
#pre_bake=10
delta_t=1
# heat up : 1 hour
# heat up by 1 hour: 1 * 3600sec = 3600 sec
# heat up frmo 38 deg to 70deg. so delta T is 70 - 38 = 32degree.
# 3600sec / 32deg = 112.5sec
sleep_delay=113

#sleep_delay=3
# heat up ; 6hours
# heat up by 6hours 6 * 3600sec = 21600sec
# heat up frmo 38 deg to 70deg. so delta T is 70 - 38 = 32degree.
# 21600sec / 32deg = 675sec / deg.
# sleep_delay=675

echo "prebake has been started..."
sleep $pre_bake

while [ $set_value -le $temp_end ]
do
    ./sdc15 /dev/ttyUSB0 1 $set_value
    ./sdc15 /dev/ttyUSB0 2 $set_value
    ./sdc15 /dev/ttyUSB0 3 $set_value
    ./sdc15 /dev/ttyUSB0 4 $set_value
    set_value=`expr $set_value + $delta_t`
    sleep $sleep_delay
done

echo "postbake has been started..."
sleep $post_bake
    ./sdc15 /dev/ttyUSB0 1 0
    ./sdc15 /dev/ttyUSB0 2 0
    ./sdc15 /dev/ttyUSB0 3 0
    ./sdc15 /dev/ttyUSB0 4 0


