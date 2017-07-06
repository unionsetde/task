# task
A task about handle_communication

## Overview:
Both gateway and sensor share the same state machine as in the following graph:
https://drive.google.com/open?id=1jEINP6sOxEPb0p4XZDogq2kNqNEC7-aOI1NbVLBhqG8

The flow for communication start at IDLE, then to state LISTENING when there is "incoming_data".
In the state LISTENING, the message received is decoded and move on to state "TALKING" after event update "NEED_TO_TALK".
In the state TALKING, either message is sent or some required action (e.g. add_ki_store) is carried out.
After that, depends on the service requirement, it may require a reply/acknowledgement (event update "REQUIRE_REPLY") or simply wait for new request (event update "END_TALKING") after going back to state IDLE.

If a reply is required, it will go back to state LISTENING and repeat until ther is event update of either "TIME_OUT" or "RETURN_REPLY".

## Request handling:
All details for handling the requests are included in "gateway_service.c" and "sensor_service.c". One can modify the service handling easily in these files.
