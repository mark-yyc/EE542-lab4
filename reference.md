# Reference

- Tutorial for socket Programming: [Socket Programming](https://blog.csdn.net/cpp_learner/article/details/127813889)
- Tutorial for pthread: [Pthread](https://hpc-tutorials.llnl.gov/posix/)
- Multuthread socket: [Multithread socket](https://blog.51cto.com/liangchaoxi/4067639)
# TODO:
1. Send in parallel: pthread
   + Divide the file into several packets and deliver.  
   + Every packet need a seq number for ACK.
   + How large for one packet?
   + How many threads?
   + What sequence to send?
2. Resend stratery: ?
   + When to resend?
3. Receive in parrallel?
