module bcd_7segment(input [3:0] bcd, output reg [6:0] seven_segment);

always @ (bcd)
  begin
    case(bcd)
		4'b0000 : begin seven_segment = ~ 7'b0111111; end
		4'b0001 : begin seven_segment = ~ 7'b0000110; end
		4'b0010 : begin seven_segment = ~ 7'b1011011; end
		4'b0011 : begin seven_segment = ~ 7'b1001111; end
		4'b0100 : begin seven_segment = ~ 7'b1100110; end
		4'b0101 : begin seven_segment = ~ 7'b1101101; end
		4'b0110 : begin seven_segment = ~ 7'b1111101; end
		4'b0111 : begin seven_segment = ~ 7'b0000111; end
		4'b1000 : begin seven_segment = ~ 7'b1111111; end
		4'b1001 : begin seven_segment = ~ 7'b1101111; end
		default : begin seven_segment = ~ 7'b1111111; end
    endcase
  end
 endmodule