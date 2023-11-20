module Top(
    input clk, rst, enable, upDown,
    output reg [3:0] count, //4-bit output representing the counter value.
    output [6:0] seg7 //7-bit output for the 7-segment display.
); 

    wire clkCounter;  // divided clock for Counter module
    wire [3:0] count_out; // Output from the Count4 module

    // Instantiate ClkDiv module
    ClkDiv clk_div_inst(
        .clk(clk),
        .clkOut(clkCounter)
    );

    // Instantiate Count4 module
    Count4 counter_inst(
        .clk(clkCounter),
        .rst(rst),
        .enable(enable),
        .upDown(upDown),
        .count(count_out)
    );

    // Instantiate Seg7decode module
    Seg7decode seg7_inst(
        .bin(count_out),
        .seg7(seg7)
    );
    
    //This block synchronizes the count output with the main clock (clk), updating it when enable is active and not in reset.
    always @(posedge clk) begin
        // Synchronize the count output with the main clock
        if (enable && !rst) begin
            count <= count_out;
        end else begin
            count <= 4'b0000; // Reset to zero when not enabled or during reset
        end
    end

endmodule