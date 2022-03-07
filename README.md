# need-for-speed
Calculating linear distance travelled using only the Gyroscope on a STM32F429I Discovery Board.

## Table of Contents
- [Components Used](#components-used)
- [Method to Calculate Linear Distance](#method-to-calculate-linear-distance)
- [Observations and Calculations](#observations-and-calculations)
- [Conclusions](#conclusions)
- [References](#references)

## Components Used
- STM32F429 DISC Board (with ARM Cortex-M4 Processor)
- L3GD20 MEMS Gyroscope (On-Board)
- ILI9341 TFT LCD (On-Board)

## Method to Calculate Linear Distance
Our approach is based upon the relation between angular velocity and linear velocity, which is given by <br />
<p align="center">
  𝑣 = 𝑟𝑤 <br />
  Where 𝑣 = Linear Velocity; 𝑟 = Radius; 𝑤 = Angular Velocity
</p><br />
We have first created our own Gyroscope Driver for the inbuilt L3GD20 Gyroscope present on our board. The driver implements 3 functions: - the first is a constructor which initializes the SPI with `1MHz` clock rate, `8-bit` data, high steady state clock and Chip Select pins for the communication, sets the clock rate,  `8-bit` data, and configures the Gyroscope into to ‘Normal’ mode so that we can read from the X, Y and Z registers. The second function is to read data from the Gyroscope registers and the third is to write data to the Gyroscope.<br /><br />

We start by calculating the angular velocity along the X, Y and Z axes in degrees per second (DPS) from the Gyroscope. To convert the raw data to DPS, we must multiply the Scaling Factor `(0.00875 for 250 DPS)` with the raw data. Here, we are not subtracting any zero-rate error as that is negligible and can be avoided without affecting the accuracy. Based on the orientation in which we have fixed the Microcontroller on our leg, we observe that the changes in angular velocity occur significantly only on the Z-axis. After taking `400 samples` in `20 seconds`, taken at an interval `50 milliseconds`, we determine the average angular velocity in DPS along the Z-axis by taking the mean of the absolute values of the 400 samples. We consider the absolute values because both the positive and negative readings contribute towards the motion of the person.<br />
<p align="center">
  𝑎𝑛𝑔𝑢𝑙𝑎𝑟 𝑣𝑒𝑙𝑜𝑐𝑖𝑡𝑦 𝑖𝑛 𝐷𝑃𝑆 = 0.00875 ∗ 𝑟𝑎𝑤 𝑑𝑎𝑡𝑎
</p><br />
We have defined two calibration factors, one for walking and one for running, which are determined empirically based on our observations of the calculated and actual distance, to make our angular velocity data meaningful.<br /><br />
Walking factor = 1.8 <br /> Running factor = 4 <br />
<p align="center">
  𝑚𝑒𝑎𝑛𝑖𝑛𝑔𝑓𝑢𝑙 𝑎𝑣𝑔. 𝑎𝑛𝑔𝑢𝑙𝑎𝑟 𝑣𝑒𝑙𝑜𝑐𝑖𝑡𝑦 𝑖𝑛 𝐷𝑃𝑆 = 𝑎𝑣𝑔. 𝑎𝑛𝑔𝑢𝑙𝑎𝑟 𝑣𝑒𝑙𝑜𝑐𝑖𝑡𝑦 𝑖𝑛 𝐷𝑃𝑆 × 𝑓𝑎𝑐𝑡𝑜𝑟
</p><br />
After converting the raw data to meaningful data and determining the average, we calculate the angular velocity 𝑤 in radians per second (RPS).<br />
<p align="center">
  𝑤 = 𝑚𝑒𝑎𝑛𝑖𝑛𝑔𝑓𝑢𝑙 𝑎𝑣𝑔. 𝑎𝑛𝑔𝑢𝑙𝑎𝑟 𝑣𝑒𝑙𝑜𝑐𝑖𝑡𝑦 𝑖𝑛 𝐷𝑃𝑆 × 𝜋/180 𝑟𝑎𝑑𝑖𝑎𝑛𝑠 𝑝𝑒𝑟 𝑠𝑒𝑐𝑜𝑛𝑑 
</p><br />
Next step is to determine the linear velocity for the calculated angular velocity. In our project, we have set the radius as `0.51m`, which is the universal average knee length of a person. We are taking the knee length as the radius because the knee acts as the pivot for our foot.<br />
<p align="center">
  𝑣 = 0.51 × 𝑤
</p><br />
Finally, we calculate the linear distance by multiplying the linear velocity with a time of `20 seconds`.<br />
<p align="center">
  𝑙𝑖𝑛𝑒𝑎𝑟 𝑑𝑖𝑠𝑡𝑎𝑛𝑐𝑒 = 𝑣 × 20
</p><br />
We have made the use of the TFT LCD screen that is available with our board to display the calculated distance, speed and the angular velocity. In addition to that, we have also added touchscreen functionality, in which, every time a user taps on the screen, the view of the screen changes showing all the different kinds of data in different screens. The same has been shown in the demonstration video.

## Observations and Calculations
The following graph shows the angular velocity (in dps) that is read from the gyroscope over `20 seconds`, sampled at intervals of 50ms:
<br />
While walking
<br />
While running
<br />
We observe from the above graphs that the number of peaks while walking are less than number of peaks while running, which is correct and proves are hypothesis that while running, the average angular velocity will be more than that while walking.

| Motion      | Actual Dist(m) | DPS      | w (rps) | v (m/s) | Calculated Dist. (m) | Error % |
| ----------- | -------------- | -------- | ------- | ------- | -------------------- | ------- |
| Walking     | 27.3           | 156      | 2.732   | 1.388   | 27.77                | 1.72    |
| Walking     | 21             | 122.4    | 2.135   | 1.088   | 21.78                | 3.7     |
| Walking     | 28.04          | 155      | 2.150   | 1.100   | 27                   | 3.7     |
| Running     | 78.75          | 428      | 7.466   | 3.800   | 76.15                | 3.3     |
| Running     | 84.12          | 458      | 7.990   | 4.070   | 81.54                | 0.03    |

We observe from the observation table that we detect the type of motion (walking or running) of the person based on the degree per second values of angular velocity. If the angular velocity is above a threshold of `180 dps` (in our code, you will find that we have set the threshold as `95`, that is because we have made that comparison before multiplying the average dps with the calibration factor), we can say that the person is running.

## Conclusions
We have created a wearable speedometer which calculates the linear velocity and distance covered by the person in intervals of `20 seconds`, using a gyroscope only. As seen in the observations, the **maximum percentage error** in the actual distance and the distance calculated by our device is `5.76%`.

## References
- [L3GD20 Gyroscope Datasheet](https://www.st.com/en/mems-and-sensors/l3gd20.html)
- [3-axis Digital MEMS Gyroscope Technical Article TA0343](https://www.elecrow.com/download/TA0343.pdf#page29)
- [DISCO_F429ZI_LCDTS Library](https://os.mbed.com/teams/ST/code/DISCO-F429ZI_LCDTS_demo/)
