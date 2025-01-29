# TCP Destination Port Covert Channel

## Introduction
This project implements a covert channel using TCP destination port manipulation. The channel hides information by encoding binary data in the destination port numbers of TCP packets, using a complex bit-counting algorithm combined with threshold-based logic.

## Implementation Details

### Encoding Strategy
The encoding uses a sophisticated approach that combines:
1. Port threshold division
2. Bit counting in port numbers
3. Dynamic logic reversal based on bit position

The encoding logic works as follows:
- Ports are divided into two ranges using a threshold
- The number of '1' bits in the port number determines the encoded bit by checking whether it is divisible by 2
- The logic is reversed based on the bit position and mathematical operations

### Parameters
- Base Port: 49152 (Start of dynamic port range)
- Port Range: 16383
- Additional parameters (two, three, five, seven) for logic manipulation
- Timeout: 30 seconds for receiver

### Limitations
- Must use ports in the dynamic range (49152-65535) to avoid conflicts
- Port range must be large enough to find valid encodings
- Network latency and packet filtering may affect reliability

## Channel Capacity
The covert channel achieves approximately 53 bits per second under normal network conditions. This was measured by:
1. Sending a 128-bit message (16 characters)
2. Measuring the total transmission time
3. Calculating bits/second

## Usage
The channel requires matching configuration parameters on both sender and receiver sides, as specified in the JSON configuration file.

## Security Considerations
The channel uses mathematical operations and bit manipulation to make the covert communication less detectable, though deep packet inspection could potentially identify unusual port patterns.