#!/bin/bash

set -e
adapter=$1
$adapter --port 13131 "$2" & 

sleep 1

exec 3<>/dev/tcp/127.0.0.1/13131
echo -e "GET /services/examples/hello_world HTTP/1.1\nAccept: */*;\nConnection: close\n" >&3
cat <&3

kill %1