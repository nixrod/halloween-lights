# Halloween Lights

## Introduction

For the year 2019 I built an interactive Halloween decoration for my house.

It consists of four components:
- A small pumpkin with a motion sensor located near the street, controlled by an esp8266

  [Add picture here]

- A bigger pumpkin with a ws2812 led ring, controlled by an esp8266

  [Add picture here]

- A 5m led strip placed on top of my balcony also controlled by an esp8266

  [Add picture here]

- A server running node-RED and an mqtt broker tying everything together. I also
built a small Node-RED dashboard, through with I could manually interact with the
installation

  [Add Screenshot]

If the motion sensor is not activated, the leds show a calm fire animation,
lighting the balcony and the big pumpkin. If the sensor is triggered the animation
shifts to a horror theme with flashing red lights for a few seconds before
returning to the nice calm fire theme.

### Video
  [Add video here]



## Hardware
  [Add hardware description with wiring schematics]


## Server configuration
On the server we run two docker containers:

### Node RED

`docker run -it -p 1880:1880 --name mynodered --link mybroker:broker nodered/node-red`

The flow defintion can be found in the server folder.
It uses the following extensions:
- node-red-dashboard
- node-red-contrib-counter

### Mosquito

`docker run -it -p 1883:1883 -p 9001:9001 -v /mosquitto/:/mosquitto/ --name mybroker eclipse-mosquitto`

As the server was reachable from the public internet the services were protected with
credentials. As it was a temporary setup, I did not bother adding ssl certificates.
