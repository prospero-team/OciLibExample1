# OciLibExample1

In order to reproduce the problem you can build bug_example.c https://github.com/prospero-team/OciLibExample1/blob/main/ConsoleApplication1/bug_example.cpp and run the following scenarios


In each cmd you must specify 
parameters %2 %3 %4



- send_raw.cmd



- send_nil.cmd



- send_blob.cmd



- read_all.cmd



- init_queues.cmd



- clean_queues.cmd

%2 - TNS name

%3 - schema name

%4 - password

for example
 
bug_example.exe init **OVA201 PROSPERO_196_MS qwertyFDSA**

## **1 scenario:** ##

1.  initialize raw and no blob reads

1. initialize the queue: [bug_example init](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/init_queues.cmd)

1. start reader (it will read all the transactions we will send next) [bug_example recive_all](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/read_all.cmd)

1. [bug_example send_raw](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/send_raw.cmd) (the reader should read 15 bytes)

1. [bug_example send_blob](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/send_blob.cmd) (reader reads the blank)

1. stop all cmd processes

1. kill the queues [bug_example clean](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/clean_queues.cmd)

## **Scenario 2:** ##

1. we initialize the blob and don't read raw

1. initialize the queue: [bug_example init](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/init_queues.cmd)

1. start reader (it will read all the transactions we will send next) [bug_example recive_all](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/read_all.cmd)

1. [bug_example send_blob](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/send_blob.cmd) (reader reads the blank) (reader should read 3000 bytes)

1. [ bug_example send_raw](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/send_raw.cmd) (reader should read empty)

1. stop all cmd processes

1. kill queues [bug_example clean](https://github.com/prospero-team/OciLibExample1/blob/main/Debug/clean_queues.cmd)


