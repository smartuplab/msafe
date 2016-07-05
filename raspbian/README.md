This is the link to the Raspbian Jessie based image for Raspberry Pi 3 that can be used to bootstrap the MSafe IoT broker
https://liuc.sharepoint.com/SmartUp/_layouts/15/guestaccess.aspx?guestaccesstoken=P23YRIOEQP6uIW7Rc5pOoEK5Ug4ndd5p4HKMXmHFeOA%3d&docid=092c1a3e0368d47a386a6b2431422f50a

It comes with this pre-installed set of packages:
- Node-RED (default Raspbian Jessie package) + additional nodes (MySQL, Freedboard) - port 1880
- Mosquitto MQTT Broker - port 1883
- MySQL server - port 3306

Static IP 192.168.0.50

Freeboard showcase
- Telemetry Station 1: http://192.168.0.50:1880/freeboard/#start-73261
- Telemetry Station 2: http://192.168.0.50:1880/freeboard/#start-13998

Raspbian Jessie root access is the default 
- user: pi
- password: raspberry

Node-RED admin access is the default
- user: admin
- password: password

Mosquitto access is user/password based
- user: test
- password: test

MySQL root access is
- user: root
- password: MSpj.2016

MySQL user access is
- user: msafe@localhost
- password: aejie7Ooshim

Please remember to change all the password if you plan to expose it to the Internet!

Please refer to the following websites for more information, installation guides and licensing details about each package:
- https://www.raspberrypi.org/downloads/raspbian/
- http://nodered.org/
- https://mosquitto.org/
