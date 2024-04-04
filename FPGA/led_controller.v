module led_controller
	(
		 input clk,
		 input blink_signal,       
		 output reg [9:0] led_pattern = 10'b1111111111
	);

// Blinking the LEDs
reg [24:0] counter = 0;
reg toggle = 1'b1; 

always @(posedge clk) begin
    if (blink_signal) begin
        counter <= counter + 1;
        if(counter >= 25'd24999999) begin 
            counter <= 0;
            toggle <= ~toggle;  
            led_pattern <= toggle ? 10'b1111111111 : 10'b0000000000;
        end
    end else begin
        led_pattern <= 10'b1111111111;
        counter <= 0;  
        toggle <= 1'b1; 
    end
end

endmodule
