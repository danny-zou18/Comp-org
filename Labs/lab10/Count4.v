module Count4(
  input clk, rst, enable, upDown, //clock input, reset input, enable counter, count up - count down
  output reg [3:0] count //4-bit output representing the counter value.
); 

  always @(posedge clk or posedge rst) begin // Code inside this block executes on positive edge of 'clk' or positive edge of 'rst'
    if (rst) begin
      count <= 4'b0000; // Reset to 0000 when rst = 1
    end else if (enable) begin
      if (upDown) begin
        if (count == 4'b1111) begin
          count <= 4'b0000; // Wrap around from 15 to 0
        end else begin
          count <= count + 1; // Increment count when upDown = 1
        end
      end else begin
        if (count == 4'b0000) begin
          count <= 4'b1111; // Wrap around from 0 to 15
        end else begin
          count <= count - 1; // Decrement count when upDown = 0
        end
      end
    end
  end

endmodule // Count4