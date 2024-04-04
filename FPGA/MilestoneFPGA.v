module MilestoneFPGA
    (
		  // Main clock input
        input wire clk,

		 // Input to trigger the alarm
       input wire alarm_trigger,     
         
      // BCD inputs for each of the six seven-segment displays
		 input wire [3:0] bcd_input0,
		 input wire [3:0] bcd_input1,
		 input wire [3:0] bcd_input2,
		 input wire [3:0] bcd_input3,
		 input wire [3:0] bcd_input4,
		 input wire [3:0] bcd_input5,

		 // Output segments for each of the six seven-segment displays
		 output wire [6:0] seg0, // Extreme right
		 output wire [6:0] seg1,
		 output wire [6:0] seg2,
		 output wire [6:0] seg3,
		 output wire [6:0] seg4,
		 output wire [6:0] seg5, // Extreme left

		 // Outputs for the LEDs
		 output wire [9:0] leds 
    );
	 
	 //wire [3:0] bcd_input = 4'b0000; // Debugging ther BCD 7 Segment

// Instantiate six bcd_7segment modules for each seven-segment display
bcd_7segment display0(.bcd(bcd_input0), .seven_segment(seg0));
bcd_7segment display1(.bcd(bcd_input1), .seven_segment(seg1));
bcd_7segment display2(.bcd(bcd_input2), .seven_segment(seg2));
bcd_7segment display3(.bcd(bcd_input3), .seven_segment(seg3));
bcd_7segment display4(.bcd(bcd_input4), .seven_segment(seg4));
bcd_7segment display5(.bcd(bcd_input5), .seven_segment(seg5));

//reg blink_signal = 1'b1; // Testing the LEDs

// Corrected interface for led_controller
led_controller alarm_leds
    (
        .clk(clk),
        .blink_signal(alarm_trigger),  // Directly use alarm_trigger as blink_signal
        .led_pattern(leds)             // Ensure led_pattern is correctly connected
    );

endmodule
