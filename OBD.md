### How OBD Works ###

OBD is a request-response protocol with several diagnostic services available. 
`Service 01` is used in this project to provide current information of the 
vehicle. Universal PIDs that may be supported can be found here
[here](https://en.wikipedia.org/wiki/OBD-II_PIDs#Service_01). However, 
vehicle manufacturers offer different PIDs. To see the PIDs supported by your 
vehicle, request data from PIDs: `0x00`, `0x20`, `0x40`, `0x60`, `0x80`, 
`0xA0`, `0xC0`. Keep in mind the reponse will be Big-endian.
