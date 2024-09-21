## Process design
1. Sending packets with no data to measure RTT: (in parallel)
   - The packet is sent using format "RTT:[clock time]", when the receiver receives the packet, it just send back with the same format.
   - When client receive this packet, it will measure the timestamp for receiving packet. Then RTT can be measured.
   - Then set timeout to 2*RTT when sending 1GB files.

2. Sending files:
   - Seperate the file to 1024 bytes each (we name it "page"), then head it with sequence number. So the whole structure is [sequence number][file data].
   - Send in parallel. Every thread is responsible for FILE_SIZE / (PAGE_SIZE * THREAD_NUM) packets.

3. Receiving files:
   - Receive in parallel. When it receive it, write the data to file with OFFSET = sequence number * PAGE_SIZE
   - Reply ACK with format "ACK:[sequence number]"

4. Packet resending:
   - There is array in client for noting files that have been sent and received.
   - When it received the ACK with format "ACK:[sequence number]", it will update the array.
   - Set the timeout = 2 * RTT, when 2RTT has passed and still can't get a ACK, resend it.
