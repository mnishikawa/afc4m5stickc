#!/bin/bash

echo "run" > /var/www/html/afc_command

echo "Content-type: text/html"
echo
echo "<html>"
echo "<head>"
echo "<title>AutoFeederController for M5StickC</title>"
echo "</head>"
echo "<body>Send run command to M5StickC auto feeder</body>"
echo "</html>"



