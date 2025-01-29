from CovertChannelBase import CovertChannelBase
from scapy.all import TCP, IP, sniff
import random
import time
from typing import List

class MyCovertChannel(CovertChannelBase):
    """
    Implementation of a covert channel using TCP destination port field manipulation.
    The sender encodes binary data by manipulating the destination port numbers.
    The receiver decodes the message by analyzing the port numbers of received packets.
    """
    def __init__(self):
        super().__init__()

    def find_valid_port(self, base_port, port_range, target_bit, threshold, reverse_logic, two, max_attempts=100):
        """
        Find a port number that encodes the target bit correctly with randomness.
        Randomly sample port numbers in the total range up to max_attempts.
        """
        for _ in range(max_attempts):
            port = random.randint(base_port, base_port + port_range - 1)
            ones_count = bin(port).count('1')
            is_even = ones_count % two == 0

            if reverse_logic:
                is_even = not is_even

            if port > threshold:
                if (is_even and target_bit == '1') or (not is_even and target_bit == '0'):
                    return port
            else:
                if (not is_even and target_bit == '1') or (is_even and target_bit == '0'):
                    return port

        return None  

    def encode_bit(self, bit, bit_index, base_port, port_range, two, three, five, seven):
        threshold = base_port + (port_range // 2)
        reverse_logic = (
            (bit_index % two != 0) 
            ^ (bit_index % three == 0)  
            ^ ((bit_index * five) % seven > two)  
        )

        port = self.find_valid_port(base_port, port_range, bit, threshold, reverse_logic, two)
        if port is None:
            raise ValueError("Could not find valid port for encoding")

        return port

    def decode_bit(self, port, bit_index, base_port, port_range, two, three, five, seven):
        threshold = base_port + (port_range // 2)
        ones_count = bin(port).count('1')
        is_even = ones_count % two == 0

        reverse_logic = (
            (bit_index % two != 0)  
            ^ (bit_index % three == 0)  
            ^ ((bit_index * five) % seven > two)  
        )

        if reverse_logic:
            is_even = not is_even

        if port > threshold:
            return '1' if is_even else '0'
        else:
            return '1' if not is_even else '0'

    def send(self, log_file_name, base_port, port_range, two, three, five, seven):
        """
        Send covert message by encoding bits in TCP destination ports.
        The message is sent as a series of packets, each encoding a single bit.
        Encoding is done using these steps:
        1. Determine the target bit to encode.
        2. Generate a random port number within the range.
        3. Check if the port number satisfies the parity condition for the target bit.
            Here is how the parity condition is determined:
            - If the port number is greater than the threshold, the parity should be even for '1' and odd for '0'.
            - If the port number is less than or equal to the threshold, the parity should be odd for '1' and even for '0'. 
            - Also to add an extra layer the parity condition is reversed based on the bit index and some randomness.
        4. If not, repeat the process until a valid port is found.
        5. Send the packet with the valid port number.
        """
        start_time = time.time()
        binary_message = self.generate_random_binary_message_with_logging(log_file_name)

        for bit_index, bit in enumerate(binary_message):
            port = self.encode_bit(bit, bit_index, base_port, port_range, two, three, five, seven)
            packet = IP(dst="receiver")/TCP(dport=port)
            super().send(packet)

        bits_per_second = len(binary_message) / (time.time() - start_time)
        print(f"Transmission rate: {bits_per_second} bits/second")

    def receive(self, base_port, port_range, timeout, log_file_name, two, three, five, seven):
        """
        Receive and decode covert message from TCP destination ports.
        The receiver listens for packets and decodes the bits from the destination ports.
        Decoding is done using the reverse of the encoding process:
        1. For each packet, extract the destination port number.
        2. Determine the bit encoded in the port number using the parity condition.
        3. Assemble the bits into bytes and decode the message.
        4. Stop when the '.' character is received.
        5. Log the received message to a file and print it.
        """

        received_chars = []
        received_bits = []
        current_byte = []
        stop_sniffing = False

        def process_packet(packet):
            nonlocal stop_sniffing, received_chars, current_byte

            if not packet.haslayer(TCP):
                return False

            dport = packet[TCP].dport

            if base_port <= dport < base_port + port_range:
                bit_index = len(received_bits)
                bit = self.decode_bit(dport, bit_index, base_port, port_range, two, three, five, seven)
                received_bits.append(bit)
                current_byte.append(bit)

                if len(current_byte) == 8:
                    char = self.convert_eight_bits_to_character(''.join(current_byte))
                    received_chars.append(char)
                    current_byte = []

                    if char == '.':
                        stop_sniffing = True
                        return True

        sniff(
            filter="tcp",
            prn=process_packet,
            timeout=timeout,
            stop_filter=lambda _: stop_sniffing
        )

        received_text = ''.join(received_chars)

        self.log_message(received_text, log_file_name)
        print(f"Received message: {received_text}")
