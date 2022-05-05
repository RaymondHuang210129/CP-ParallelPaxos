# Purdue CS 525 Final Project
An attempt to implement Parallel Paxos for throughput improvement

This project is done by:
Tse-Jui Huang, [@RaymondHuang210129](https://github.com/RaymondHuang210129)
Po-Jen Hsu, [@alexpjhsu](https://github.com/alexpjhsu), and 
Liang-Yu Chen, [@fredchen000](https://github.com/fredchen000)

## Get Started

### Compile Programs
```shell
make
```
### Run Programs
Edit the config file `config.conf`

To run Client role on machines:
```shell
./client 127.0.0.1 4000 200 100000
```

To run Replica role on machines:
```shell
./replica 127.0.0.1 24000 
```

To run Leader role on machines:
```shell
./leader 127.0.0.1 25000 4
```

To run Acceptor role on machines:
```shell
./acceptor 127.0.0.1 27000
```

## Project Report

### Background (3 pages)
#### What is Paxos
- A classic protocol that could achieve agreement(consensus) in a distributed system
- Single Paxos: could only accept one value
- Multi Paxos: could accept multiple value by conducting leader election
- Note: In this project, we are going to implement parallel version of multi Paxos. Therefore, Paxos mentioned later would all be multi-paxos. We follow [Paxos Made Moderately Complex](https://www.cs.cornell.edu/courses/cs7412/2011sp/paxos.pdf) when implanting our code.
#### Paxos components overview
- Client: sending commands that wish to be executed(Here we assume only one outstanding request)
- Replica: 
    - on receiving client request
        - put the request in slotin and update slotin by 1
        - send the request with slot as a proposal to all leaders.
    - on receiving decisions
        - execute decision in slotout and update slotout by 1 until there is no decisions in slotout
        - clear the proposal which has the same slots as the decisions
        - send back the executing result to client
- Leader:
    - on receiving proposals, if it is the active leader, spawn commander thread
    - commander:
        - send out accept message to all acceptors (letting the acceptor to accept a proposal)
        - wait for accepted messages
        - after receiving a majority of accepted, send out decisions back to all replica to execute
- Acceptors
    - wait for accept message, 
* Note: We did not implement leader election logic. Reasons will be discussed in the following chapter.
#### Paxos Challenge
- Scalability for leader
    - Only the active leader could process the proposals from replica --> a practical limit on ops per second
- Scalability of replicas
    - Each replica stores the entire copy of the key-value store.
- Why sharding may not work?
    - Hot keys can't be sharded. Therefore, we still need to take a look and see if we can modify the Paxos algorithm to make it parallel.

#### Assumption 
1. For performance, we care about throughput, which is the number of requests per seconds that could be executed and send back to the client 
2. For a serial Paxos, after the leader is elected, it will need to handle all the proposals sent from all replicas.
    - If there are only few proposals, the active leader of the serial Paxos may can handle. However, it cannot handle the case well if suddenly a flood of proposals arrived at the active leader
    - A typical case in real life is trading system. A trading system may use Paxos to execute and replicate the transactions to satisfy linearizability. It is normal that there will be a huge number of transactions anytime during the trading hour. Therefore the performance is critical. 
3. Why not include leader election
- In a stable network, leader re-election won't happen frequently, therefore won't be a big issue to the overall performance.

### Reason and Motivation (2 pages)
- Why do we choose this topic (making Paxos parallel)?
1. two of us are also taking CS505(distributed systems), which asked us to implement [Paxos](https://github.com/emichael/dslabs) using Java. The projects provide thorough test such as depth first search to verify that our implementation satisfy safety property, breadth first search that our implementation satisfy the performance requirement.
    - Through debugging those tests, we soon figure out that the active leader would be the performance bottle neck since this server node will need to handle all replica's proposal and spawn commander to ask for votes from acceptor. Moreover, it will need to count for votes and send out decisions back to all replicas.
    - We wish to see if we can design the Paxos in parallel to solve the performance bottleneck.
2. Besides, since the [Paxos](https://github.com/emichael/dslabs) project uses pseudo network to simulate real network situation, it already provides all network api. Since the lab is very challenging, we really learn a lot from it, so we wish to implement the Paxos by ourself from scratch and really test the performance in a real-world network.

### Expected Result (Analysis before implementaion) (1~2 pages)
- The serial version cannot handle a large amount of data simultaneously.
- After making paxos parallel, the replica would become the new bottleneck since it will need to execute command serially to satisfy linearizability.
- Using more commander threads could help relieve the workload of sending and waiting for votes, thus increase the performance.
- Using more replica, leader and acceptor threads could lead to a better performance.

### Implementations

#### Node (1 page)

- The Node class is a network utility class for:
    1. Create UDP socket & Bind Socket to specific port 
    2. Send raw data to a specific IP:Port
    3. Broadcast raw data to a list of IPs:Ports
    4. Receive raw data from binded port

- A getter function for retrieving its binded port.

#### Message & Command (1 page)

- Each type of message and command is implemented in classes where all of them extend `serialize()` method to transmit accoss networks.

![](https://i.imgur.com/KDHtgBP.png)


#### Config file (1 page)

```
---- // Section 1: Replica Processes
128.10.12.218:24000 24001 1
---- // Section 2: Leader processes
128.10.12.218:25000 25001 1
---- // Section 3: Acceptor Processes
128.10.12.218:27000 27001 1
128.10.12.218:27100 27101 1
128.10.12.218:27200 27201 1
```
- Config Format: 
    ```
    [hostIP:hostPort] [startPort] [numthreads]
    ```
- The first argument is a process's identifier so that each process can get its corresponding entry. each process will have its unique hostIP:hostPort provided by program arguments.
- The second argument indicates the first thread's node port. conventionally, the nth thread will use port number `startPort + n - 1` except that leader's subprocess commander will have differnet port mapping.
- The third argument indicates the number of replica/leader/acceptor object instances. Some exceptions will show in following slides.

#### Client (1 page)
- In a distributed system, each client thread continuously:
    1. Broadcast a request (unexecuted command) to all replicas
    2. Wait & Receive a result (execution outcome) from replicas
    3. Check the result whether its command matches with sent request
        a. If match, go to step 4
        b. If not match, go to step 2
    4. Accept the result
    5. Repeat step 1~4 until no any unprocessed request (unexecuted commands)

- Client Input Arguments:
    ```
    ./client [client IP] [client Port] [Thread Count] [Command Count]
    ```
    - `IP` : client IP
    - `Port` : client Port
    - `Thread Count` : Workload, sepcify how many thread the client application needs to create
    - `Command Count` : Sepcify how many number of requests each thread needs to send.
        - In experiments: The `Command Number` is set to a large number to allow measuring the throughput in each second.

#### Replica (2~3 pages)

- Arguments: `hostIP`:`hostPort` as the process identifier

- A parallel Replica process has two kinds of thread instances:
    1. Handler threads: receive message from `node` member. when receving message, either propose requests from received message or old command in a decide slot, or push decided slot-comand pair into a log array and notify its semaphore
    2. Executor thread: consume the slot-command pair when the corresponding semaphore's `wait()` has returned and then execute the command and send response to clients.

- Mechanism of parallel Replica
    1. a handler thread `i` will expose port `startPort + i - 1` to receive new request. 
    2. the request message's hash value is calculated to determine which thread to accept the message.
    3. handler thread `i` only cares about slot `n * numthread + i` where `n ∈ Ｎ` and only send propose message with leader's thread `i` . This mechanism allows each thread to maintain their own `proposals`, `decision`, `slotIn` and `slotOut` instead of sharing the variables.
    4. A 100 ms timer is set when sending each propose message and the message is resent when timer arrives.
    5. All replica threads shares a log array, and each handler threads shares a semaphore with executor thread.
    6. When each handler thread runs decision logic, for each command in `slotOut` position, the thread put slot-command pair into the log and notify its semaphore.
    7. The execution thread tries to execute logs in order. before execute the command, it wait the semaphore belonging to the handle thread who put the command. This mechanism garuantee the thread to execute a decided slot.

- Reasoning of this implementation
    1. Limitation: Hard to maintain parallelism while maintain the execution order
    2. Compromise: Make this serial part of code as small as possible and limit the data to be shared by at most 2 threads.
    3. An optimistic assumption: The hash function used at request handling steps should be idealy uniform so that executor thread will executor the command without unboundly waiting slot holes. We intentionally use mod function to simulate hash function and each client will send requests with incremented ID. Hence, the workload of each thread will be well-balanced.

![](https://i.imgur.com/mtLps0c.png)


#### Leader (1~2 pages)

- Arguments: `hostIP`:`hostPort` as the process identifier, and `numCommander` as number of commander thread spawned by eaxh leader thread

- A parallel leader process has two kinds of thread instances:
    1. Leader thread: Responsible for receiving propose messages from replicas and assign phase 2 task to commander thread.
    2. Commander thread: Responsible for conduct phase 2 thread assigned by leaders.

- Mechanism of Parallel Leader: 
    1. Leader thread `i` will expose port `startPort + i - 1` to receive new propose message and spawns `numCommander` of commander threads with port `startPort + numLeaders + i * numCommander + commanderId` at initialization.
    2. When leader thread receives propose message, it choose one comander thread using round robbin and send assign message to it.
    3. Each commander thread broadcast accept message to each Acceptor's `i`th thread and collects accepted message just as original multi-Paxos does.
    4. When commander collects a majority of accepted message of a command, broadcast decision message back to the each replica's `i`th thread.

- Reasoning of this implementation
    1. Leader bottleneck: For each request being processed in original Paxos algorithm, each leader will create commander threads to sends and receives most number of messages. Therefore, creating threads for each propose message generates a considerable overhead.
    2. To decrease overhead, we decided to implement commander thread pool. Therefore, instead of spawning a new commander thread, we assign proposals to existing threads in the thread pool in a round robin fashion. Hence each commander thread in thread pool becomes the new performance bottleneck

#### Acceptor (1 pages)

- Arguments: `hostIP`:`hostPort` as the process identifier

- Mechanism of Parallel Acceptors:
    1. Split the workload into multiple threads.
    2. Each acceptor thread would send the accepted message back to the commander which means that the proposal has been accepted by this acceptor.


### Scaling results (3~5 pages)

#### Experiment environment:
- Testbed: 
    - mc17 workstation for an Active leader/a Replica/an Acceptor/a Client program (Paxos singleton)
    - mc18 for an Acceptor program
    - mc20 for an Acceptor program

#### Experiment 1: Client Workload - Commanders' in progress Task Count over time & throughput over time Relation
![](https://i.imgur.com/2ygANpg.png)
![](https://i.imgur.com/QoLlRTn.png)
![](https://i.imgur.com/jYzC36S.png)
- Configurations
    - Control Variables: 
        - Acceptor Count = 3 
        - Acceptor Thread Count = 1
        - Leader Thread Count = 1
        - Replica Thread Count = 1
        - Commander Thread Count of each Leader = 1
    - Independent Variable: 
        - Client workload (Number of Clients sending requests)
    - Dependent Variable: 
        - Throughput over time
        - Waitfor: number of tasks in the commander thread waiting for majority to accept

- Findings & Explanations
    - When workload < 50
        - Throughput maintains constant with different workload since each request can be served in time
        - waitfor variable remain lower than Client workload over time, indicating that few propose messages are resent and no late arrival
        - Commander threads has idle computation resource
    - When workload ≥ 50
        - Commander thread resource become saturated
        - Throughput starts to drop due to the vicious cycle of buffer overflow message dropping and flood of retransmitted messages arrival.
        - waitfor variable starts to pile up since more messages are lost while late arrival propose starts to accumulate garbage tasks which has already served in Commander.
    - When waitfor remain low
        - Few propose message is retransmited thus virtually no garbage tasks accumulate in commander threads
    - When waitfor start to grow
        - Lots of propose messages is retransmited where some of them arrives after the task has already completed, hence causing garbage tasks accumulate in commander threads


- From this experiment we concluded:
    1. A Commander thread can handle at most 40~50 Client workloads without perforamance drop
    2. Overwhelming messages in network can cause message congestions/drops, leading to significant throughput degradation
    3. waitfor variable can be a indicator if large amount of retransmissions happen.


#### Experiment 2: Commander Thread Count - Throughput Relation
![](https://i.imgur.com/xHxteZK.png)
- Configurations
    - Control Variables: 
        - Acceptor Count = 3 
        - Acceptor Thread Count = 1
        - Leader Thread Count = 1
        - Replica Thread Count = 1
        - Commander Thread Count of each Leader = 1
    - Independent Variable: 
        - Commander Thread Count for each Leader thread
        - Client workload
    - Dependent Variable: 
        - Average throughput 

- Findings & Explanations
    - When commander thread count = 1, the throughput is aligned with the result we get from Experiment 1
    - The graph can be seperated into two parts
        - More commander thread count (≥ 3)
            - Workload variable becomes a neglectable factor since commander threads can effectively serve all tasks without network buffer accumulation and message drops.
        - Less commander thread count (< 3)
            - Heavy workload
                - Using more commander threads can split the workload and increase throughput. This is exetremely obvious when workload ≥ 200. As shown, the throughput is horrible when using 2 or less commander threads (same reason as Experiment 1). By adding more commander threads, the retransmission issue can be solved thus improve the performance dramatically
            - Light workload
                - Since there are only few retransmissions, the performance are relatively good when using smaller number of threads. However, using more commander threads could still split the workload and increase the overall performance.

- From this experiment we concluded:
    1. Commander is not the bottleneck when the number of commander thread is higher than 3 for each leader thread.
    2. The touch-ground performance in small thread count/high workload configuration confirms the conclution 2 in experiment 1.


#### Experiment 3: Replica/Leader/Acceptor Thread Count - Throughput Relation
![](https://i.imgur.com/WIHUDRD.png)
- Configurations
    - Control Variables: 
        - Acceptor Count = 3 
        - Commander thread count of each Leader = 4
    - Independent Variables: 
        - Replica/Leader/Acceptor thread count
        - Client workload
    - Dependent Variable: 
        - Average throughput 

- Findings & Explanations
    - Based on the result from experiment 2, we choose the number of commander threads as 4 so that only occasional retransmission happens which would not affect throughput in this experiment.
    - When replica/leader/acceptor thread count is 1 ~ 5
        - Using more threads indicates that workload can be split in more threads thus performance is improved
        - leader/replica/acceptor threads does not saturate CPU cores' computing resource
        - Thread number increases → Have ability to proceed more workloads → Throughput increases
    - Throughput decrease when replica/leader/acceptor thread number is 6 ~ 16
        - When thread count is more than 5, the total thread count (leader, commander, replica, acceptors) is significant larger than the total number of CPU cores in mc17. Therefore, the overhead of context switches becomes significant
        - Since the only executor thread in replica shares semaphores/log slots with other handler threads, cache writebacks happens more frequently when the number of handler threads increases and cause more overhead.
        - Two factors of overhead lead to performance degradation and the performance becomes even worse than single thread execution


### Conclution (1 page)

- According to experiment 1, using only one commander cannot handle a large amount of data simultaneously and will have a lot of message lost and retransmission, hence degrading the throughput
- As expected, the replica would become the new bottleneck since it will need to execute command serially to satisfy linearizability. The executor thread's program logic should s simple as possible.
- Using more replica, leader and acceptor threads could lead to better performance. However, when using too many threads, the performance could decrease due to the heavy context switch.

#### What we can do in the future:
1. Experiment on "minimum commander thread count with no congestion - number of Acceptor instance" relation
    - We only able to reach mc17, mc18, mc20 to do our measurement
    - Use other machines with different subnet may have less predictable network situation and hence affect our measurement
2. Find a more feasible mechanism to load-balance replica threads' workload
    - We use round robbin policy at client side to decide which replica thread to send request
    - We maybe can use another thread to dispatch request workload to handler thread more evenly, but may increase the sequential part of program and increase overhead
3. Leader election logics can be implemented 
    - Just only need synchronize the local variables among threads and make it act as a serial Paxos


