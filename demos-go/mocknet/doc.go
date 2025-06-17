/*
In cpp:
- An implementation of the data_transport_interface_t, called callback_data_transport_t, which gets a bunch of callbacks in the constructor

In cb-mpc-go/network:
- Create a bunch of callbacks as intefaces, data_transport_callback_interface

- Create a function that gets implementaion of data_transport_callback_interface and returns an instance of network_t
- Create another function that gets a network_t pointer, and jsid, etc that call cpp to return job_session_2p_t pointer

In cb-mpc-go/mocknetwork:
- A mock implementation of the data_transport_callback_interface, called demo_data_transpot
*/
package mocknet
