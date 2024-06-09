#include <Python.h>
#include <numpy/arrayobject.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include "TimeTagger4_interface.h"
const bool USE_TIGER_START = true;	// if false, external signal must be provided on start; not applicable if continuous mode is enabled
const bool USE_TIGER_STOPS = true; 	// if false please connect signals to some of channels A-D
// Function declarations
static PyObject* timetagger4vector_init(PyObject* self, PyObject* args);
static PyObject* timetagger4vector_config(PyObject* self, PyObject* args);
static PyObject* timetagger4vector_start(PyObject* self, PyObject* args);
static PyObject* timetagger4vector_stop(PyObject* self, PyObject* args);
static PyObject* timetagger4vector_close(PyObject* self, PyObject* args);
static PyObject* timetagger4vector_read(PyObject* self, PyObject* args);

// Method definitions
static PyMethodDef TimeTagger4VectorMethods[] = {
	{"init", timetagger4vector_init, METH_VARARGS, "Initialize the module"},
	{"config", timetagger4vector_config, METH_VARARGS, "Configure the module"},
	{"start", timetagger4vector_start, METH_VARARGS, "Start the module"},
	{"stop", timetagger4vector_stop, METH_VARARGS, "Stop the module"},
	{"close", timetagger4vector_close, METH_VARARGS, "Close the module"},
	{"read", timetagger4vector_read, METH_VARARGS, "Read data from the module"},
	{NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef timetagger4vector = {
	PyModuleDef_HEAD_INIT,
	"timetagger4vector",
	"A CPython module for cronologic timetagger",
	-1,
	TimeTagger4VectorMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_timetagger4vector(void) {
	import_array();  // Initialize the NumPy C API
	return PyModule_Create(&timetagger4vector);
}

timetagger4_device* device;
timetagger4_static_info static_info;
timetagger4_param_info parinfo;


void print_device_information(timetagger4_device* device, timetagger4_static_info* si, timetagger4_param_info* pi) {
	// print board information
	printf("Board Serial        : %d.%d\n", si->board_serial >> 24, si->board_serial & 0xffffff);
	printf("Board Configuration : %s\n", timetagger4_get_device_name(device));
	printf("Board Revision      : %d\n", si->board_revision);
	printf("Firmware Revision   : %d.%d\n", si->firmware_revision, si->subversion_revision);
	printf("Driver Revision     : %d.%d.%d\n", ((si->driver_revision >> 16) & 255), ((si->driver_revision >> 8) & 255), (si->driver_revision & 255));
	printf("Driver SVN Revision : %d\n", si->driver_build_revision);
	printf("\nTDC binsize         : %0.2f ps\n", pi->binsize);
}

// Function implementations
static PyObject* timetagger4vector_init(PyObject* self, PyObject* args) {
	// prepare initialization
	timetagger4_init_parameters params;

	timetagger4_get_default_init_parameters(&params);
	params.buffer_size[0] = 8 * 1024 * 1024;				// size of the packet buffer
	params.board_id = 0;							// value copied to "card" field of every packet, allowed range 0..255
	params.card_index = 0;						// which of the TimeTagger4 board found in the system to be used

	int error_code;
	const char* err_message;
	device = timetagger4_init(&params, &error_code, &err_message);
	if (error_code != CRONO_OK) {
		printf("Could not init TimeTagger4 compatible board: %s\n", err_message);
		return PyLong_FromLong(error_code);
	}
	timetagger4_static_info static_info;
	timetagger4_get_static_info(device, &static_info);
	return PyLong_FromLong(TIMETAGGER4_OK);
}

static PyObject* timetagger4vector_config(PyObject* self, PyObject* args) {
	// prepare configuration
	timetagger4_static_info static_info;
	timetagger4_get_static_info(device, &static_info);
	timetagger4_configuration config;
	// fill configuration data structure with default values
	// so that the configuration is valid and only parameters
	// of interest have to be set explicitly
	timetagger4_get_default_configuration(device, &config);

	// set config of the 4 TDC channels
	for (int i = 0; i < TIMETAGGER4_TDC_CHANNEL_COUNT; i++)
	{
		// enable recording hits on TDC channel
		config.channel[i].enabled = true;

		// define range of the group
		config.channel[i].start = 0;	// range begins right after start pulse

		config.channel[i].stop = 30000;	// recording window stops after ~15 us


		// measure only rising edge for tiger (positive) pulse or falling for user (negative) pulse
		config.trigger[TIMETAGGER4_TRIGGER_A + i].falling = USE_TIGER_STOPS ? 0 : 1;
		config.trigger[TIMETAGGER4_TRIGGER_A + i].rising = USE_TIGER_STOPS ? 1 : 0;
	}

	// generate an internal 25 kHz trigger, used for tiger and continuous mode
	config.auto_trigger_period = (int)(static_info.auto_trigger_ref_clock / 1000);
	config.auto_trigger_random_exponent = 0;

	// setup TiGeR
	// sending a signal to the LEMO outputs (and to the TDC on the same channel)
	// requires proper 50 Ohm termination on the LEMO output to work reliably

	// width of the 12ns pulse in the auto_trigger clock periods
	int pulse_width = (int)(12e-9 * static_info.auto_trigger_ref_clock);


	// use 200 kHz auto trigger to generate

	// generate above configured auto trigger to generate a 
	// signal with 12 ns pulse width on LEMO output Start
	config.tiger_block[0].enable = USE_TIGER_START ? 1 : 0;
	config.tiger_block[0].start = 0;
	config.tiger_block[0].stop = config.tiger_block[0].start + pulse_width;
	config.tiger_block[0].negate = 0;
	config.tiger_block[0].retrigger = 0;
	config.tiger_block[0].extend = 0;
	config.tiger_block[0].enable_lemo_output = 1;
	config.tiger_block[0].sources = TIMETAGGER4_TRIGGER_SOURCE_AUTO;
	// if TiGeR is used for triggering with positive pulses
	if (USE_TIGER_START)
		config.dc_offset[0] = TIMETAGGER4_DC_OFFSET_P_LVCMOS_18;
	else // user input expect NIM signal
		config.dc_offset[0] = TIMETAGGER4_DC_OFFSET_N_NIM;

	// start group on falling edges on the start channel 0
	config.trigger[TIMETAGGER4_TRIGGER_S].falling = USE_TIGER_START ? 0 : 1;
	config.trigger[TIMETAGGER4_TRIGGER_S].rising = USE_TIGER_START ? 1 : 0;


	for (int i = 1; i < TDC4_TIGER_COUNT; i++) {
		config.tiger_block[i].enable = USE_TIGER_STOPS ? 1 : 0;
		config.tiger_block[i].start = i * 100;
		config.tiger_block[i].stop = config.tiger_block[i].start + pulse_width;
		config.tiger_block[i].negate = 0;
		config.tiger_block[i].retrigger = 0;
		config.tiger_block[i].extend = 0;
		config.tiger_block[i].enable_lemo_output = USE_TIGER_STOPS ? 1 : 0;
		config.tiger_block[i].sources = TIMETAGGER4_TRIGGER_SOURCE_AUTO;


		if (USE_TIGER_STOPS)
			config.dc_offset[i] = TIMETAGGER4_DC_OFFSET_P_LVCMOS_18;
		else // user input expect NIM signal
			config.dc_offset[i] = TIMETAGGER4_DC_OFFSET_N_NIM;

		// this is not related to the tigers, but uses the same indexing (0 is start)
		// optionally increase input delay by 10 * 200 ps for each channel on new TT		
		// config.delay_config[i].delay = i * 10;
	}



	// write configuration to board
	int status = timetagger4_configure(device, &config);
	if (status != TIMETAGGER4_OK) {
		const char* err_message = timetagger4_get_last_error_message(device);
		printf("Could not init TimeTagger4 compatible board: %s\n", err_message);

		return PyLong_FromLong(status);
	}
	timetagger4_get_static_info(device, &static_info);

	timetagger4_get_param_info(device, &parinfo);

	print_device_information(device, &static_info, &parinfo);
	return PyLong_FromLong(TIMETAGGER4_OK);

}

static PyObject* timetagger4vector_start(PyObject* self, PyObject* args) {
	// start data capture
	int status = timetagger4_start_capture(device);
	if (status != CRONO_OK) {
		printf("Could not start capturing %s", timetagger4_get_last_error_message(device));
		timetagger4_close(device);
		return  PyLong_FromLong(status);
	}

	// start timing generator
	timetagger4_start_tiger(device);
	Py_RETURN_NONE;
}

static PyObject* timetagger4vector_stop(PyObject* self, PyObject* args) {

	// shut down packet generation and DMA transfers
	int status = timetagger4_stop_capture(device);
	Py_RETURN_NONE;

}
static PyObject* timetagger4vector_close(PyObject* self, PyObject* args) {
	// deactivate timetagger4
	timetagger4_close(device);
	Py_RETURN_NONE;

}


bool hasData = false;
// structure with packet pointers for read data
timetagger4_read_out read_data;
static PyObject* timetagger4vector_read(PyObject* self, PyObject* args) {

	// configure readout behaviour
	timetagger4_read_in read_config;
	// automatically acknowledge all data as processed
	// on the next call to timetagger4_read()
	// old packet pointers are invalid after calling timetagger4_read()
	read_config.acknowledge_last_read = 1;

	// Create a Python list to hold the NumPy arrays
	PyObject* py_list = PyList_New(0);
	if (!py_list) {
		return NULL;
	}

	// get pointers to acquired packets
	int status = timetagger4_read(device, &read_config, &read_data);
	if (status != CRONO_OK) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	else
	{
		// iterate over all packets received with the last read
		volatile crono_packet* p = read_data.first_packet;
		while (p <= read_data.last_packet)
		{

			int hit_count = 2 * (p->length);
			// Two hits fit into every 64 bit word. The second in the last word might be empty
			// This flag  tells us, whether the number of hits in the packet is odd
			if ((p->flags & TIMETAGGER4_PACKET_FLAG_ODD_HITS) != 0)
				hit_count -= 1;

			uint32_t* packet_data = (uint32_t*)(p->data);
			uint32_t rollover_count = 0;
			npy_intp dims[1] = { hit_count + 1 };
			PyObject* array = PyArray_SimpleNew(1, dims, NPY_DOUBLE);
			if (!array) {
				Py_DECREF(py_list);
				return NULL;
			}
			double* array_data = (double*)PyArray_DATA((PyArrayObject*)array);
			double group_abs_time = p->timestamp * parinfo.packet_binsize / 1000.0;
			// first value is the absolute time
			array_data[0] = group_abs_time;
			// 
			uint64_t rollover_period_bins = static_info.rollover_period;
			for (int i = 0; i < hit_count; i++)
			{
				uint32_t hit = packet_data[i];
				uint32_t channel = hit & 0xf;
				// extract hit flags
				uint32_t flags = hit >> 4 & 0xf;

				if ((flags & TIMETAGGER4_HIT_FLAG_TIME_OVERFLOW) != 0) {
					// this is a overflow of the 23/24 bit counter)
					rollover_count++;
				}
				else {
					// extract hit timestamp
					uint32_t ts_offset = hit >> 8 & 0xffffff;

					// Convert timestamp to ns, this is relative to the start of the group
					double ts_offset_ns = (ts_offset + rollover_count * rollover_period_bins) * parinfo.binsize / 1000.0;
					array_data[i + 1] = ts_offset_ns;
				}
			}


			PyList_Append(py_list, array);
			p = crono_next_packet(p);
		}



	}
	return py_list;
}







