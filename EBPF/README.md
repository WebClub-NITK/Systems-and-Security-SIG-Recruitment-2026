# Networking/eBPF: Programmable VXLAN Overlay Network
### Tags: eBPF, VXLAN, network namespace
## Introduction:
Modern cloud networks rely on virtual interfaces, Layer-2 overlays, programmable packet processing, and dynamic control planes. Linux provides all the building blocks needed to recreate a simplified version of such a network entirely in software.
In this task, you will build a programmable multi-tenant overlay network using Linux networking primitives and eBPF. You will construct the data plane, implement packet-processing policies, and build a lightweight userspace component to monitor and manage the network.
## Problem Statement(s):
1. Create the topology and VXLAN(Virtual Extensible LAN) overlay:
   - Create host1, host2, tenant1, and tenant2 namespaces with virtual network interfaces and bridges.
   - Configure the underlay transport network, VXLAN VNI(VXLAN Network Identifier), interface MTUs(Maximum Transmission Unit) and required FDB(Forwarding Database) entries, and demonstrate Layer-2 connectivity between the tenants.
   - Connect them as follows:
     ```
      ┌──────────┐      ┌──────────────────┐              ┌──────────────────┐      ┌──────────┐
      │ Tenant 1 │ <──> │  Bridge / VXLAN  │ <==========> │  VXLAN / Bridge  │ <──> │ Tenant 2 │
      └──────────┘      │      Host 1      │              │      Host 2      │      └──────────┘
                        └──────────────────┘              └──────────────────┘
     ```

2. Implement eBPF Packet Processing and traffic policy:
   - Attach a tc eBPF classifier to a tenant-facing interface.
   - Implement configurable IPv4/TCP filtering based on IP addresses or ports.
   - Track packet statistics using BPF maps and demonstrate connectivity and filtering.
3. Build a lightweight network controller:
   - Implement a userspace controller in C/Python/Go/Rust that takes a configuration describing the desired namespaces, interfaces, bridges, VXLAN parameters, MTUs, FDB entries, and eBPF policies, and uses Netlink/iproute2 to configure them.
   - Verify that the required VXLAN and FDB configuration matches the desired state, and restore missing or incorrect entries. Demonstrate this by removing an FDB entry and showing that the controller detects and restores it.
   - **[Bonus]** Make the controller idempotent, so running it multiple times produces the same network configuration without creating duplicate interfaces, bridges, VXLAN devices, or FDB entries, and without disrupting an already working network.
4. **[Bonus]** Implement per-flow telemetry:
   - Track source/destination IPs, ports, protocol, and last-seen time using eBPF maps.
   - Provide a userspace tool to read and display the flows.
5. **[Bonus**] Implement an eBPF Layer-4 load balancer:
   - Expose multiple backend namespaces through a virtual service IP/port.
   - Select a backend for each new flow using round-robin and ensure subsequent packets of the same flow continue to use the selected backend.
## Resources:
- [Introduction to Linux Network Namespaces](https://blogs.igalia.com/dpino/2016/04/10/network-namespaces/)
- [Linux Bridge and FDB](https://docs.kernel.org/networking/bridge.html)
- [VXLAN Networking](https://docs.kernel.org/networking/vxlan.html)
- [Linux TAP/TUN](https://blog.cloudflare.com/virtual-networking-101-understanding-tap/)
- [eBPF](https://ebpf.io/what-is-ebpf/)
- [Linux Netlink](http://cdn.kernel.org/doc/html/latest/userspace-api/netlink/intro.html)
- [tc](https://www.man7.org/linux/man-pages/man8/tc.8.html)
## Submission:
- Create a private GitHub repository with solutions for each of the parts. The repository must be private until asked to be made public.
- Add mentors as collaborators.
- Include a README file explaining the implementation process, supported by screenshots and shell logs wherever necessary. Document failures(if any) and the resolution steps.
### Mentors: 
1. Nischay Bharadwaj Mahesh (Ph. No.: +91 9980543867, GitHub ID: N-tronics)
2. Ananya A. K. (Ph. No.: +91 7483599090, GitHub ID: Ananya-A-K)
