# pwmWriter
pwm writer for raspberry pi that allows you write pwm without root access. Based on wiringPi

## How to use

```shell
make
sudo make install
```

This will create a systemd service, and this service will automatically start on boot. By default, it will listen for pwm command sent to pin 13 and 18, and set the mode of these pins to `PWM_OUTPUT`. To change the this, modify `pwmWriter.service`, then run `sudo make reload-service`.

To send pwm command to the specific pin, write an integer to `/dev/gpiopwm/x`.

```shell
echo 512 > /dev/pwmgpio/13 # This will invoke pwmWrite(13, 512) in wiringPi.
```