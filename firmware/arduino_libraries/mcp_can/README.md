CAN BUS Shield
---------------------------------------------------------
[![CAN BUS Shield](http://www.seeedstudio.com/depot/images/1130300211.jpg)](http://www.seeedstudio.com/depot/CANBUS-Shield-p-2256.html?cPath=19_88)



<br>
CAN-BUS is a common industrial bus because of its long travel distance, medium communication speed and high reliability. It is commonly found on modern machine tools and as an automotive diagnostic bus. This CAN-BUS Shield adopts MCP2515 CAN Bus controller with SPI interface and MCP2551 CAN transceiver to give your Arduino/Seeeduino CAN-BUS capibility. With an OBD-II converter cable added on and the OBD-II library imported, you are ready to build an onboard diagnostic device or data logger.

- Implements CAN V2.0B at up to 1 Mb/s
- SPI Interface up to 10 MHz
- Standard (11 bit) and extended (29 bit) data and remote frames
- Two receive buffers with prioritized message storage
- Industrial standard 9 pin sub-D connector
- Two LED indicators



<br>
# Usage:



## 1. Set the BaudRate

This function is used to initialize the baudrate of the CAN Bus system.

The available baudrates are listed as follws:

	#define CAN_5KBPS    1
	#define CAN_10KBPS   2
	#define CAN_20KBPS   3
	#define CAN_31K25BPS 4
	#define CAN_33KBPS   5
	#define CAN_40KBPS   6
	#define CAN_50KBPS   7
	#define CAN_80KBPS   8
	#define CAN_83K3BPS  9
	#define CAN_95KBPS   10
	#define CAN_100KBPS  11
	#define CAN_125KBPS  12
	#define CAN_200KBPS  13
	#define CAN_250KBPS  14
	#define CAN_500KBPS  15
	#define CAN_1000KBPS 16


<br>

##2. Set Receive Mask and Filter

There are 2 receive mask registers and 5 filter registers on the controller chip that guarantee you get data from the target device. They are useful especially in a large network consisting of numerous nodes.

We provide two functions for you to utilize these mask and filter registers. They are:

    init_Mask(unsigned char num, unsigned char ext, unsigned char ulData);
    init_Filt(unsigned char num, unsigned char ext, unsigned char ulData);

**num** represents which register to use. You can fill 0 or 1 for mask and 0 to 5 for filter.

**ext** represents the status of the frame. 0 means it's a mask or filter for a standard frame. 1 means it's for a extended frame.

**ulData** represents the content of the mask of filter.



<br>
## 3. Check Receive
The MCP2515 can operate in either a polled mode, where the software checks for a received frame, or using additional pins to signal that a frame has been received or transmit completed.  Use the following function to poll for received frames.

    INT8U MCP_CAN::checkReceive(void);

The function will return 1 if a frame arrives, and 0 if nothing arrives.



<br>
## 4. Get CAN ID

When some data arrive, you can use the following function to get the CAN ID of the "send" node. 

    INT32U MCP_CAN::getCanId(void);



<br>
## 5. Send Data

    CAN.sendMsgBuf(INT8U id, INT8U ext, INT8U len, data_buf);

This is a function to send data onto the bus. In which:

**id** represents where the data come from.

**ext** represents the status of the frame. '0' means standard frame. '1' means extended frame.

**len** represents the length of this frame.

**data_buf** is the content of this message.

For example, In the 'send' example, we have:

<pre>  
unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

CAN.sendMsgBuf(0x00, 0, 8, stmp); //send out the message 'stmp' to the bus and tell other devices this is a standard frame from 0x00.
</pre>



<br>
## 6. Receive Data

The following function is used to receive data on the 'receive' node:

    CAN.readMsgBuf(unsigned char len, unsigned char buf);

In conditions that masks and filters have been set. This function can only get frames that meet the requirements of masks and filters.

**len** represents the data length.

**buf** is where you store the data.

<br>
## 7. Check additional flags

When frame is received you may check whether it was remote request and whether it was an extended (29bit) frame.

    CAN.isRemoteRequest();
    CAN.isExtendedFrame();

**return value** is '0' for a negative response and '1' for a positive


<br>
For more information, please refer to [wiki page](http://www.seeedstudio.com/wiki/CAN-BUS_Shield).

    
----

This software is written by loovee ([luweicong@seeed.cc](luweicong@seeed.cc "luweicong@seeed.cc")) for seeed studio<br>
and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>






[![Analytics](https://ga-beacon.appspot.com/UA-46589105-3/CAN_BUS_Shield)](https://github.com/igrigorik/ga-beacon)


<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-65075738-1', 'auto');
  ga('send', 'pageview');

</script>
