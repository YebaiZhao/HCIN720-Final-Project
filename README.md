# HCIN720-Final-Project
<img src="/picutres/IMG_20151215_072656.jpg" width="300">

A complete prototype including Software for [Photon](https://store.particle.io/), 3D Model, Laser cutting Material for recreating. 

.... is a clock like device for recording the data-- like light temperature and motion, happend around house in 12 hours.

### Docs & Help

- [Software for Photon]() (latest release)
- [3D printing](/3D%20printing%20and%20CAD)
- [laser cutting](/Laser%20Cut)


### 3D printing part

3D printing parts are for holding the minute step motor and hour step motor, which rotate the gears. The .gcode file is avalible [here](/3D%20printing%20and%20CAD/final-motor_holder.gcode) for 3D printing. You could also check the model outlook [here](/3D%20printing%20and%20CAD/final-motor_holder.stl).

<img src="/3D%20printing%20and%20CAD/final-motor_holder.png" width="300">

### laser cutting

Laser cutting are serveral part, including, the hour plate, minute plate, gears, acrylic bricks, center support material and the outter box, servo holder. All the parts are avaliable in this [.ai file](/Laser%20Cut/v7.ai) at different layers. The final printing might look like this.

<img src="/picutres/20151213_085424.jpg" width="300">

To customize your own clock, you could paint the hour plate, minute plate and acrylic bricks in different color. We printed the hour plate with white, and acrylic bricks with blue, red, green and yellow, like the first picture.

### Software installation

There are three different files in the software part. The .ino file are avaliable for two photon. [One](/Code/clockOperation.ino) take care of clock operation(including two [step motors](http://www.amazon.com/gp/product/B00JB22IQC?psc=1&redirect=true&ref_=oh_aui_detailpage_o09_s00) and two SG92 servo), and another [one](/Code/sensor.ino) for data collection(here we provide the light sensor and [motion sensor](http://www.amazon.com/gp/product/B013LA6MW0?psc=1&redirect=true&ref_=oh_aui_detailpage_o00_s00) code). Another [.html](/Code/test-version1.html) file is for get temperature data from [weather map API](http://openweathermap.org/api) and send it to photon.

