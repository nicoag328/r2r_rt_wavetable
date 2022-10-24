if (TARGET tinyusb_device)
	add_executable(r2r_rt_wavetable
	        r2r_rt_wavetable.c
	        )
	
	# Pull in our pico_stdlib which pulls in commonly used features
	target_link_libraries(r2r_rt_wavetable pico_stdlib hardware_pwm hardware_adc hardware_interp)
	
	pico_enable_stdio_usb(r2r_rt_wavetable 1)
	pico_enable_stdio_uart(r2r_rt_wavetable 0) 
	
	# create map/bin/hex file etc.
	pico_add_extra_outputs(r2r_rt_wavetable)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
