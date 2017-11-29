/* ************************************************************************ *
* GBN.cpp:                                                                 *
*                                                                          *
* A GBN simulator which performs:                                          *
*                                                                          *
*    (1) The sender host calculates and displays the time a packet has     *
*        been transmitted at the sender host (using GBN sliding-window     *
*        flow control).                                                    *
*                                                                          *
*    (2) The simulator randomly drops some packets that are specified as   *
*        "error" on the fly from the sender to the receiver (the error     *
*        packets are specified by another file (error.txt)).               *
*                                                                          *
*    (3) The receiver host calculates and displays the time it             *
*        successfully recieves a packet.                                   *
*        the receiver host.                                                *
*                                                                          *
*    (4) The receiver host sends an ACK for each successfully received     *
*        packet. The receiver calculates and displays the time it          *
*        transmits an ACK to the sender host.                              *
*                                                                          *
*    (5) When a packet is lost on the way to the receiver host, the        *
*        receiver host sends a NACK for each such lost packet.             *
*        The receiver host calculates and displays the time it sends       *
*        a NACK to the sender host.                                        *
*                                                                          *
*    (6) Calculate and display the time the snder host receices an ACK     *
*        and a NACK at the sender host.                                    *
*                                                                          *
* Requirements:                                                            *
*                                                                          *
*    (a) The number of packets to transfer is at most 10 million packets.  *
*                                                                          *
*    (b) The outputs from the sender should be made on the left side of    *
*        the 80-char standard console.  The outputs from the receiver      *
*        should be made on the right side (after 50 characters from the    *
*        left side of the consile).                                        *
*                                                                          *
*    (c) Dispalys the time based on the ascecing time order.               *
*                                                                          *
*    (d) Calculate the ellapsed time to fnish transfeerign all the packets *
*        and display the time at the end of each simulation.               *
*                                                                          *
*    (e) Calcualte the total number of packets to successfully transmit    *
*        all the packets using GBN sliding-window flow control.            *
*                                                                          *
*    (f) At the begnning of the simulation, prompt the following inputs:   *
*        (i) transmission bandwidth                                        *
*        (ii) transmission distance                                        *
*        (iii) number of packets to transmit                               *
*                                                                          *
*    (g) the following parameters whould be "hard coded" in your source    *
*        code:                                                             *
*        (i) packet size in bits (all packets are the same size)           *
*        (ii) the target utilization (80%)                                 *
*                                                                          *
*    (h) All the outputs should be made by "printf" C/C++ standard API.    *
*        The same outputs shoul be made to the output file (in the same    *
*        format displayed by "printf") using "fprintf".                    *
*                                                                          *
* TEST SAMPLE:                                                             *
*     BW = 100Mbps (100000000 bits/sec)                                    *
*     Distance = 1000 miles                                                *
*       => Window Size = 101 packets                                       *
*                                                                          *
* Last Updated: 9:34 AM, August 2017                                       *
* ************************************************************************ */

/*
 * Ryan Owens
 * 800502524
*/
#include <stdio.h>     // for "printf"
#include <math.h>      // for "ceil"
#include <stdlib.h>    // for "calloc"

#define  PACKET_SIZE         6400 // each packet = 3200 bits (= 800 bytes) 
#define  TARGET_U              80 // the target utilization (80%)

#define  NUM_PKT_MIN         1000 // the min. number of packets to transmit
#define  NUM_PKT_MAX    100000000 // the max. number of packets to transmit

#define  BW_MIN           1000000 // the lowest transmission bandwdith (1Mbps)
#define  BW_MAX        1000000000 // the lowest transmission bandwdith (100Mbps)

#define  DIST_MIN             100 // the min. transmission distance (100 miles)
#define  DIST_MAX           10000 // the max. transmission distance (10000 miles)

#define  PROP_SPEED          4000 // nano second for signals to propagate each mile

// global variables //////////////////////////////////////////////////////////
long long unsigned int sig_prop_delay;         // signal propagation delay in "nano second"
long long unsigned int pkt_transmission_delay; // packet transmission delay in "nano second"

unsigned int           TX_BW;             // transmission bandwidth
unsigned int           distance;          // transmuission (end-to-end) distance 
unsigned int           num_packets_tx;    // the number of packets to transfer

										  // function prototypes  //////////////////////////////////////////////////////
void display_results(unsigned int num_packets_tx, long long unsigned int transmission_at_sender[],
	long long unsigned int receive_at_receiver[], long long unsigned int ACK_at_sender[]);
void prompt_three_parameters(void);
unsigned int calculate_window_size(void);

// YOUR CUSTOM-MADE FUNCTION PROTOTYPES //////////////////////////////////////
#define DEBUG false
typedef long long unsigned int lint;
typedef unsigned int uint;
bool check_for_received_ack(lint simulation_time, int index, int sdr_window_size, int window_size,  lint* ack_times);
lint calculate_next_ACK_arrival_time(lint simulation_time, int index, lint* ack_times);




// the module MAIN ///////////////////////////////////////////////////////////
void main(void)
{
	long long unsigned int simulation_time;        // simulation time in "nano second"

	long long unsigned int simulation_completion_time; // simulation time

	unsigned int    num_completed_pkt;    // the number of packets completed
	unsigned int    window_size;          // the window size to achieve U = 80%
	unsigned int    sdr_window_size;      // the open window size at the sender

										  // timestamp vectors ------------------------------------------------
	long long unsigned int * transmission_at_sender;
	long long unsigned int * ACK_at_sender;
	long long unsigned int * receive_at_receiver;

	char                   * Used;  //HF

									// set initial parameters -------------------------------------------
	num_packets_tx = 32;   // 32 packets to simulat
	TX_BW = 100000000;     // 100M bps
	distance = 100;        // 100  miles
	window_size = 5;   // window size = 15

						// instantiate the timestamp vectors --------------------------------
	transmission_at_sender = (long long unsigned int *)calloc(num_packets_tx, sizeof(long long unsigned int));
	ACK_at_sender = (long long unsigned int *)calloc(num_packets_tx, sizeof(long long unsigned int));
	receive_at_receiver = (long long unsigned int *)calloc(num_packets_tx, sizeof(long long unsigned int));
	Used = (char *)calloc(num_packets_tx, sizeof(char));

	// initialize the timestamp vectors ---------------------------------
	for (unsigned int i = 0; i < num_packets_tx; i++)
	{
		transmission_at_sender[i] = 0;
		ACK_at_sender[i] = 0;
		receive_at_receiver[i] = 0;
		Used[i] = 0;
	}

	// calculate the window size ----------------------------------------
	//window_size = calculate_window_size();

	// set up the sender and the receiver window size -------------------
	sdr_window_size = window_size;

	simulation_time = 0;            // initialize the simulation time
	num_completed_pkt = 1;            // reset the number of the packets completed

									  // BELOW IS YOUR WORK PLACE ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool done = false;
	//sdr_window_size = 1; // Start with Window size of 1 for GBN
	sig_prop_delay = ((long long unsigned int)distance) * ((long long unsigned int)PROP_SPEED);
	pkt_transmission_delay = (1000000000 / ((long long unsigned int)TX_BW))*((long long unsigned int)PACKET_SIZE);

	int transmission_at_sender_index = 0;
	int ACK_at_sender_index = 0;
	int receive_at_receiver_index = 0;

	lint tx_time = -1;
	lint receive_time = -1;
	lint ack_time = -1;
	do
	{
		// check for ack
			// if ACK, increase window size
		if (check_for_received_ack(simulation_time, num_completed_pkt, sdr_window_size, window_size, ACK_at_sender))
		{
	      sdr_window_size++;
		}
		
		if (DEBUG) {
			printf("Window Size: %d\n", sdr_window_size);
		}

		// is Window size at sender > 0
	    // forward simulation time to next ACK time
		if (sdr_window_size <= 0)
		{
			if (DEBUG) {
				printf("Skipping time to the next ACK message\n");
			}
			simulation_time = calculate_next_ACK_arrival_time(simulation_time, num_completed_pkt - window_size, ACK_at_sender);
			if (DEBUG) {
				printf("\t\tNew Simulation time: %d\n", simulation_time);
			}
			sdr_window_size = window_size;
		}

		// Calculate transmission completion time
			// = current time  +  transmission delay + propogation delay
		tx_time = simulation_time + pkt_transmission_delay;
		if (DEBUG) {
			printf("TX Time: %d\n", tx_time);
		}

		transmission_at_sender[num_completed_pkt] = tx_time;
		// Calculate packet receive time
		receive_time = tx_time + sig_prop_delay;

		if (DEBUG) {
			printf("Receive Time: %d\n", receive_time);
		}

		receive_at_receiver[num_completed_pkt] = receive_time;
		// Calculate ACK receive time
		ack_time = receive_time + sig_prop_delay;

		if (DEBUG) {
			printf("Ack Time: %d\n", ack_time);
		}

		ACK_at_sender[num_completed_pkt] = ack_time;
		// Increase the number of packets completed by one
		simulation_time = transmission_at_sender[num_completed_pkt];
		sdr_window_size--;
		num_completed_pkt++;

		// If all packates have been sent
		if (num_completed_pkt > num_packets_tx)
		{
			done = true;
		}
		

	} while (!done);
	simulation_completion_time = simulation_time;

									  // ABOVE IS YOUR WORK PLACE ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

									  // display the results -------------------------------
	display_results(num_packets_tx, transmission_at_sender, receive_at_receiver, ACK_at_sender);

	// record the completion time of the simulation ------
	simulation_completion_time = ACK_at_sender[(num_completed_pkt - 1)];

	// display the simulation time -----------------------
	printf("\nsimulation has been completed at %llu...\n", simulation_completion_time);

	// the end of this simulator -------------------------
	exit(0);
}

// display_results ///////////////////////////////////////////////////////////
void display_results(unsigned int num_packets_tx,
	long long unsigned int transmission_at_sender[],
	long long unsigned int receive_at_receiver[],
	long long unsigned int ACK_at_sender[])
{
	unsigned int                    i;   // loop counter

	for (i = 1; i <= num_packets_tx; i++)
	{
		printf("%u, tx time: %llu, rcv time: %llu, ACK time: %llu\n", i,
			transmission_at_sender[i], receive_at_receiver[i], ACK_at_sender[i]);
	}
}


// calculate_window_size /////////////////////////////////////////////////////
unsigned int calculate_window_size(void)
{
	const unsigned int     packet_size = PACKET_SIZE; // packet size (in bits)
	const unsigned int     target_U = TARGET_U;    // target utilization w/out  
												   //  packet error
	unsigned int window_size;
	float    f_temp;  // neede to calculate the open window size

					  // claculate the window size to achieve U = 80% ---------------------
	sig_prop_delay = ((long long unsigned int)distance) * ((long long unsigned int)PROP_SPEED);
	pkt_transmission_delay = (1000000000 / ((long long unsigned int)TX_BW))*((long long unsigned int)packet_size);

	f_temp = (float)(2 * sig_prop_delay + pkt_transmission_delay) / (float)pkt_transmission_delay;
	f_temp = f_temp*((float)target_U / 100);
	window_size = ceil(f_temp);

	// return the calculated window size --------------------------------
	return(window_size);
}


// YOUR CUSTOM-MADE FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool check_for_received_ack(lint simulation_time, int index, int sdr_window_size, int window_size, lint* ack_times)
{
	if (simulation_time == 0 || index == 0 || window_size == 0)
		return false;
	if (sdr_window_size > 0 || num_packets_tx == 0)
	{
		return false;
	}

	if (index - window_size < 0 || ack_times[index-window_size] == 0)
	{
		return false;
	}
	if(DEBUG)
	printf("Simulation Time: %llu, Ack Time: %llu\n", simulation_time, ack_times[index - window_size]);
	if (simulation_time >= ack_times[index - window_size])
		return true;
	return false;
}

lint calculate_next_ACK_arrival_time(lint simulation_time, int index, lint* ack_times)
{
	if(DEBUG)
	printf("\t\tSimulation time: %llu\n\t\tAck Times[index]: %llu\n", simulation_time, ack_times[index]);
	if (simulation_time >= ack_times[index])
		return simulation_time;
	return ack_times[index];
}